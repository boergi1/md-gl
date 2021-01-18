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

#include "logorenderer.h"
#include <QPainter>
#include <QPaintEngine>
#include <qmath.h>

LogoRenderer::LogoRenderer()
{
}

LogoRenderer::~LogoRenderer()
{
}


void LogoRenderer::paintQtLogo()
{
    _shaderProgram_1.enableAttributeArray(_normalAttrID);
    _shaderProgram_1.enableAttributeArray(_vertexAttrID);
    _shaderProgram_1.setAttributeArray(_vertexAttrID, _vertices.constData());
    _shaderProgram_1.setAttributeArray(_normalAttrID, _normals.constData());
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
    _shaderProgram_1.disableAttributeArray(_normalAttrID);
    _shaderProgram_1.disableAttributeArray(_vertexAttrID);
}


void LogoRenderer::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    const char *vsrc1 =
            "attribute highp vec4 sl_Vertex;\n"
            "attribute mediump vec3 sl_Normal;\n"

            "uniform mat4 sl_M;\n"
            "uniform mat4 sl_V;\n"
            "uniform mat4 sl_P;\n"

            "varying mediump vec4 sl_Color;\n"
            "void main(void)\n"
            "{\n"
            "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
            "    float angle = max(dot(sl_Normal, toLight), 0.0);\n"
            "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
            "    sl_Color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
            "    sl_Color = clamp(sl_Color, 0.0, 1.0);\n"
            "    gl_Position = sl_V * sl_Vertex;\n"
            //     gl_Position = P*V*M * Position;
            "}\n";

    const char *fsrc1 =
            "varying mediump vec4 sl_Color;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = sl_Color;\n"
            "}\n";

    _shaderProgram_1.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
    _shaderProgram_1.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc1);
    _shaderProgram_1.link();

    _vertexAttrID = _shaderProgram_1.attributeLocation("sl_Vertex");
    _normalAttrID = _shaderProgram_1.attributeLocation("sl_Normal");
    _modelMatID = _shaderProgram_1.uniformLocation("sl_M");
    _viewMatID = _shaderProgram_1.uniformLocation("sl_V");
    _projMatID = _shaderProgram_1.uniformLocation("sl_P");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    _modelMat.fill(1.0f);
    m_fScale = 1;
    createGeometry();
}

void LogoRenderer::render()
{
    glDepthMask(true);

    glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);

    if (_newViewRotation)
    {
        _viewRotationMutex.lock();
        // Direction : Spherical coordinates to Cartesian coordinates conversion
        _viewDirection = QVector3D( cos(_verticalViewAngle) * sin(_horizontalViewAngle),
                                    sin(_verticalViewAngle),
                                    cos(_verticalViewAngle) * cos(_horizontalViewAngle) );
        // Right vector
        _viewRight = QVector3D( sin(_horizontalViewAngle - M_PI/2.0f),
                                0,
                                cos(_horizontalViewAngle - M_PI/2.0f) );
        // Up vector
        _viewUp = QVector3D::crossProduct( _viewRight, _viewDirection );
        _newViewRotation = false;
        _viewRotationMutex.unlock();
    }


    if (_newViewPosition)
    {
        _viewPositionMutex.lock();
        // Forward / Backward -> by projection matrix
        if (false){
            _viewPosition += _viewDirection * _translationViewDeltaZ;
            _translationViewDeltaZ = 0.0;
        }
        // Right / Left
        if (_translationViewDeltaX != 0.0){

            _viewPosition += _viewRight * _translationViewDeltaX;
            _translationViewDeltaX = 0.0;
        }
        // Up / Down
        if (_translationViewDeltaY != 0.0){

            _viewPosition += _viewUp * _translationViewDeltaY;
            _translationViewDeltaY = 0.0;
        }
        _newViewPosition = false;
        _viewPositionMutex.unlock();
    }






    //        float FoV = _initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    //        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    //            _ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    float verticalAngle = 45.f;
    float aspectRatio = 1;
    _projMat.perspective(verticalAngle, aspectRatio, 0.001f, 100.0f);



    // Camera matrix
    QMatrix4x4 viewMat;
    viewMat.lookAt(
                _viewPosition,           // eye / Camera is here
                _viewPosition+_viewDirection, // center / and looks here : at the same position, plus "direction"
                _viewUp                      // upvec / Head is up (set to 0,-1,0 to look upside-down)
                );
    _viewMat = viewMat;

    _shaderProgram_1.bind();
    _shaderProgram_1.setUniformValue(_modelMatID, _modelMat);
    _shaderProgram_1.setUniformValue(_viewMatID, _viewMat);
    _shaderProgram_1.setUniformValue(_projMatID, _projMat);
    paintQtLogo();
    _shaderProgram_1.release();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

}

void LogoRenderer::createGeometry()
{
    _vertices.clear();
    _normals.clear();

    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const qreal Pi = M_PI;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i) {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

    for (int i = 0;i < _vertices.size();i++)
        _vertices[i] *= 2.0f;
}

void LogoRenderer::quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4)
{
    _vertices << QVector3D(x1, y1, -0.05f);
    _vertices << QVector3D(x2, y2, -0.05f);
    _vertices << QVector3D(x4, y4, -0.05f);

    _vertices << QVector3D(x3, y3, -0.05f);
    _vertices << QVector3D(x4, y4, -0.05f);
    _vertices << QVector3D(x2, y2, -0.05f);

    QVector3D n = QVector3D::normal
            (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(x4 - x1, y4 - y1, 0.0f));

    _normals << n;
    _normals << n;
    _normals << n;

    _normals << n;
    _normals << n;
    _normals << n;

    _vertices << QVector3D(x4, y4, 0.05f);
    _vertices << QVector3D(x2, y2, 0.05f);
    _vertices << QVector3D(x1, y1, 0.05f);

    _vertices << QVector3D(x2, y2, 0.05f);
    _vertices << QVector3D(x4, y4, 0.05f);
    _vertices << QVector3D(x3, y3, 0.05f);

    n = QVector3D::normal
            (QVector3D(x2 - x4, y2 - y4, 0.0f), QVector3D(x1 - x4, y1 - y4, 0.0f));

    _normals << n;
    _normals << n;
    _normals << n;

    _normals << n;
    _normals << n;
    _normals << n;
}

void LogoRenderer::extrude(qreal x1, qreal y1, qreal x2, qreal y2)
{
    _vertices << QVector3D(x1, y1, +0.05f);
    _vertices << QVector3D(x2, y2, +0.05f);
    _vertices << QVector3D(x1, y1, -0.05f);

    _vertices << QVector3D(x2, y2, -0.05f);
    _vertices << QVector3D(x1, y1, -0.05f);
    _vertices << QVector3D(x2, y2, +0.05f);

    QVector3D n = QVector3D::normal
            (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    _normals << n;
    _normals << n;
    _normals << n;

    _normals << n;
    _normals << n;
    _normals << n;
}
