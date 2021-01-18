/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "threadrenderer.h"
#include "logorenderer.h"

#include <QtCore/QMutex>
#include <QtCore/QThread>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QGuiApplication>
#include <QtGui/QOffscreenSurface>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

QList<QThread *> ThreadRenderer::threads;

/*
 * The render thread shares a context with the scene graph and will
 * render into two separate FBOs, one to use for display and one
 * to use for rendering
 */
class RenderThread : public QThread
{
    Q_OBJECT
public:
    RenderThread(const QSize &size)
        : surface(nullptr)
        , context(nullptr)
        , m_renderFbo(nullptr)
        , m_displayFbo(nullptr)
        , m_logoRenderer(nullptr)
        , m_size(size)
    {
        qDebug()<<"QThread (RenderThread) created";
        ThreadRenderer::threads << this;
    }

    QOffscreenSurface *surface;
    QOpenGLContext *context;




public slots:
    void renderNext()
    {
#if VERBOSE > 0
        qDebug()<<"RenderThread::renderNext slot";
#endif
        context->makeCurrent(surface);

        if (!m_renderFbo) {
            // Initialize the buffers and renderer
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            m_renderFbo = new QOpenGLFramebufferObject(m_size, format);
            m_displayFbo = new QOpenGLFramebufferObject(m_size, format);
            m_logoRenderer = new LogoRenderer();
            m_logoRenderer->initialize();
        }

        m_renderFbo->bind();
        context->functions()->glViewport(0, 0, m_size.width(), m_size.height());

        m_logoRenderer->render();

        // We need to flush the contents to the FBO before posting
        // the texture to the other thread, otherwise, we might
        // get unexpected results.
        context->functions()->glFlush();

        m_renderFbo->bindDefault();
        qSwap(m_renderFbo, m_displayFbo);
#if VERBOSE > 0
        qDebug()<<"RenderThread::textureReady signal";
#endif
        emit textureReady(m_displayFbo->texture(), m_size);
    }

    void shutDown()
    {
        qDebug()<<"QQuickWindow::sceneGraphInvalidated signal in RenderThread::shutDown slot";
        context->makeCurrent(surface);
        delete m_renderFbo;
        delete m_displayFbo;
        delete m_logoRenderer;
        context->doneCurrent();
        delete context;

        // schedule this to be deleted only after we're done cleaning up
        surface->deleteLater();

        // Stop event processing, move the thread to GUI and make sure it is deleted.
        exit();
        moveToThread(QGuiApplication::instance()->thread());
    }


    void setViewRotation(float r_x, float r_y, float r_z)
    {
        m_logoRenderer->addViewRotation(r_x, r_y, r_z);
    }
    void setViewTranslation(float t_x, float t_y, float t_z)
    {
        m_logoRenderer->addViewTranslation(t_x, t_y, t_z);
    }
signals:
    void textureReady(int id, const QSize &size);

private:
    QOpenGLFramebufferObject *m_renderFbo;
    QOpenGLFramebufferObject *m_displayFbo;

    LogoRenderer *m_logoRenderer;
    QSize m_size;

};



class TextureNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    TextureNode(QQuickWindow *window)
        : m_id(0)
        , m_size(0, 0)
        , m_texture(nullptr)
        , m_window(window)
    {
        // Our texture node must have a texture, so use the default 0 texture.
        m_texture = m_window->createTextureFromId(0, QSize(1, 1));
        setTexture(m_texture);
        setFiltering(QSGTexture::Linear);
        qDebug()<<"QSGSimpleTextureNode (TextureNode) created";
    }

    ~TextureNode() override
    {
        delete m_texture;
    }

signals:
    void textureInUse();
    void pendingNewTexture();

public slots:

    // This function gets called on the FBO rendering thread and will store the
    // texture id and size and schedule an update on the window.
    void newTexture(int id, const QSize &size) {
#if VERBOSE > 0
        qDebug()<<"TextureNode::newTexture - calling QQuickWindow::update";
#endif
        m_mutex.lock();
        m_id = id;
        m_size = size;
        m_mutex.unlock();

        // We cannot call QQuickWindow::update directly here, as this is only allowed
        // from the rendering thread or GUI thread.
        emit pendingNewTexture();
    }


    // Before the scene graph starts to render, we update to the pending texture
    void prepareNode() {
#if VERBOSE > 0
        qDebug()<<"QQuickWindow::beforeRendering signal in TextureNode::prepareNode slot";
#endif
        m_mutex.lock();
        int newId = m_id;
        QSize size = m_size;
        m_id = 0;
        m_mutex.unlock();
        if (newId) {
            delete m_texture;
            // note: include QQuickWindow::TextureHasAlphaChannel if the rendered content
            // has alpha.
            m_texture = m_window->createTextureFromId(newId, size);
            QSGSimpleTextureNode::setTexture(m_texture);

            QSGNode::markDirty(DirtyMaterial);

            // This will notify the rendering thread that the texture is now being rendered
            // and it can start rendering to the other one.
#if VERBOSE > 0
            qDebug()<<"TextureNode::textureInUse signal";
#endif
            emit textureInUse();
        }
    }

private:

    int m_id;
    QSize m_size;

    QMutex m_mutex;

    QSGTexture *m_texture;
    QQuickWindow *m_window;
};

ThreadRenderer::ThreadRenderer()
    : m_renderThread(nullptr)
{
    qDebug()<<"QQuickItem (ThreadRenderer) created";
    setFlag(ItemHasContents, true); // Enable QQuickItem::update() function

    connect(this, &QQuickItem::windowChanged, this, &ThreadRenderer::handleWindowChanged);

    m_renderThread = new RenderThread(QSize(512, 512));
    connect(this, &ThreadRenderer::rotationChanged, m_renderThread, &RenderThread::setViewRotation);
    connect(this, &ThreadRenderer::translationChanged, m_renderThread, &RenderThread::setViewTranslation);
}

void ThreadRenderer::moveCursor(QPointF p)
{
    int x = p.x();
    int y = p.y();
    QCursor::setPos(x,y);
}

void ThreadRenderer::setKeyButton(int button, bool isPressed)
{
    _keyMap[button] = isPressed;
}

void ThreadRenderer::setMouseButton(int button, bool isPressed)
{
    _mouseMap[button] = isPressed;
}

void ThreadRenderer::ready()
{
    qDebug()<<"ThreadRenderer::ready";
    m_renderThread->surface = new QOffscreenSurface();
    m_renderThread->surface->setFormat(m_renderThread->context->format());
    m_renderThread->surface->create();

    m_renderThread->moveToThread(m_renderThread);

    connect(QQuickItem::window(), &QQuickWindow::sceneGraphInvalidated, m_renderThread, &RenderThread::shutDown, Qt::QueuedConnection);

    m_renderThread->start();
    QQuickItem::update(); // calling ThreadRenderer::updatePaintNode
}

void ThreadRenderer::handleWindowChanged(QQuickWindow *window)
{
    connect(window, &QQuickWindow::beforeSynchronizing, this, &ThreadRenderer::beforeSync, Qt::DirectConnection);
    connect(window, &QQuickWindow::afterSynchronizing, this, &ThreadRenderer::afterSync, Qt::DirectConnection);
    connect(window, &QQuickWindow::afterRendering, this, &ThreadRenderer::afterRend, Qt::DirectConnection);
    connect(window, &QQuickWindow::afterAnimating, this, &ThreadRenderer::afterAnim, Qt::DirectConnection);
    connect(window, &QQuickWindow::afterRenderPassRecording, this, &ThreadRenderer::afterRendPass, Qt::DirectConnection);
    connect(window, &QQuickWindow::beforeRenderPassRecording, this, &ThreadRenderer::beforeRendPass, Qt::DirectConnection);

}

void ThreadRenderer::beforeSync()
{
    //    qDebug()<<"QQuickWindow::beforeSynchronizing";
}

void ThreadRenderer::afterSync()
{
    //    qDebug()<<"QQuickWindow::afterSynchronizing";
}

void ThreadRenderer::afterRend()
{
    //   qDebug()<<"QQuickWindow::afterRendering";
}

void ThreadRenderer::afterAnim()
{
    //        qDebug()<<"QQuickWindow::afterAnimating";
}

void ThreadRenderer::afterRendPass()
{
    //   qDebug()<<"QQuickWindow::afterRenderPassRecording";
}

void ThreadRenderer::beforeRendPass()
{
    //    qDebug()<<"QQuickWindow::beforeRenderPassRecording";
}

QSGNode *ThreadRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    qDebug()<<"ThreadRenderer::updatePaintNode";
    TextureNode *node = static_cast<TextureNode *>(oldNode);

    if (!m_renderThread->context) {
        QOpenGLContext *current = QQuickItem::window()->openglContext();
        // Some GL implementations requres that the currently bound context is
        // made non-current before we set up sharing, so we doneCurrent here
        // and makeCurrent down below while setting up our own context.
        current->doneCurrent();

        m_renderThread->context = new QOpenGLContext();
        m_renderThread->context->setFormat(current->format());
        m_renderThread->context->setShareContext(current);
        m_renderThread->context->create();
        m_renderThread->context->moveToThread(m_renderThread);

        current->makeCurrent(window());

        QMetaObject::invokeMethod(this, "ready");
        return nullptr;
    }

    if (!node) {
        node = new TextureNode(window());

        /* Set up connections to get the production of FBO textures in sync with vsync on the
         * rendering thread.
         *
         * When a new texture is ready on the rendering thread, we use a direct connection to
         * the texture node to let it know a new texture can be used. The node will then
         * emit pendingNewTexture which we bind to QQuickWindow::update to schedule a redraw.
         *
         * When the scene graph starts rendering the next frame, the prepareNode() function
         * is used to update the node with the new texture. Once it completes, it emits
         * textureInUse() which we connect to the FBO rendering thread's renderNext() to have
         * it start producing content into its current "back buffer".
         *
         * This FBO rendering pipeline is throttled by vsync on the scene graph rendering thread.
         */
        connect(m_renderThread, &RenderThread::textureReady, node, &TextureNode::newTexture, Qt::DirectConnection);
        connect(node, &TextureNode::pendingNewTexture, window(), &QQuickWindow::update, Qt::QueuedConnection);
        connect(window(), &QQuickWindow::beforeRendering, node, &TextureNode::prepareNode, Qt::DirectConnection);
        connect(node, &TextureNode::textureInUse, m_renderThread, &RenderThread::renderNext, Qt::QueuedConnection);


        // Get the production of FBO textures started..
        qDebug()<<"ThreadRenderer::updatePaintNode calling RenderThread::renderNext";
        QMetaObject::invokeMethod(m_renderThread, "renderNext", Qt::QueuedConnection);
    }

    node->setRect(boundingRect());

    return node;
}

#include "threadrenderer.moc"
