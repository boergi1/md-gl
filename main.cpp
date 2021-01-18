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

#include <QtCore/QThread>

#include <QGuiApplication>

#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformintegration.h>

#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "threadrenderer.h"
#include "deviceinterface.h"

#include "realsensedevice.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    //    app.setFont(QFont("Helvetica", 5));
    QQmlApplicationEngine engine;

    if (!QGuiApplicationPrivate::platform_integration->hasCapability(QPlatformIntegration::ThreadedOpenGL)) {
        qDebug()<<"ERROR";
        QQuickView view(&engine, 0);
        view.setSource(QUrl("qrc:/qml/error.qml"));
        view.show();
        return app.exec();
    }

    qRegisterMetaType<DeviceBaseClass::DeviceState>("DeviceState");
    qmlRegisterUncreatableType<DeviceBaseClass>("Shared", 1, 0, "DeviceState", "Enum is not a type");


    qRegisterMetaType<DeviceInterface::DeviceType>("DeviceType");
    qmlRegisterUncreatableType<DeviceInterface>("Shared", 1, 0, "DeviceType", "Enum is not a type");
    qRegisterMetaType<DeviceInterface::DeviceUserType>("DeviceUserType");
    qmlRegisterUncreatableType<DeviceInterface>("Shared", 1, 0, "DeviceUserType", "Enum is not a type");
    //    qRegisterMetaType<RealsenseDevice::SyncTypeRS>("SyncType");
    //    qmlRegisterUncreatableType<RealsenseDevice>("Shared", 1, 0, "SyncType", "Enum is not a type");

//    qRegisterMetaType<RealsenseDevice::rs2_format_e>("RS2Format");
//    qmlRegisterUncreatableType<DeviceInterface>("Shared", 1, 0, "RS2Format", "Enum is not a type");

    DeviceInterface* device_interface = new DeviceInterface();
    engine.rootContext()->setContextProperty("deviceInterface", device_interface);

    qmlRegisterType<ThreadRenderer>("SceneGraphRendering", 1, 0, "Renderer");

    int execReturn = 0;
    {
        QQuickView view(&engine, 0);
        //     view.engine()->rootContext()->setContextProperty("deviceInterface", device_interface);

        // Rendering in a thread introduces a slightly more complicated cleanup
        // so we ensure that no cleanup of graphics resources happen until the
        // application is shutting down.
        view.setPersistentOpenGLContext(true);
        view.setPersistentSceneGraph(true);

        view.setResizeMode(QQuickView::SizeRootObjectToView);
        view.setSource(QUrl("qrc:/qml/main.qml"));
        view.show();

        execReturn = app.exec();
    }

    // As the render threads make use of our QGuiApplication object
    // to clean up gracefully, wait for them to finish before
    // QGuiApp is taken off the heap.
    for (QThread *t : qAsConst(ThreadRenderer::threads)) {
        t->wait();
        delete t;
    }
    //    delete viewInterface;

    return execReturn;
}
