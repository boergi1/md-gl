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

import QtQuick 2.12
import QtQuick.Controls 1.4

import SceneGraphRendering 1.0
import "."

import Shared 1.0

Item {
    id: rootItem
    width: 1280
    height: 720

    Component.onCompleted: {

        AppConstants.wWidth = Qt.binding(function() {return width})
        AppConstants.wHeight = Qt.binding(function() {return height})
    }

//    Connections {
//        target: deviceInterface
//        onInitializeDevice: {

//        }
//    }


    //MenuBar: {}

    focus: true
    Keys.onPressed: {
        //  console.log("key pressi",event.key)
        renderer.setKeyButton(event.key, true)
        event.accepted = true;
    }
    Keys.onReleased: {
        renderer.setKeyButton(event.key, false)
        event.accepted = true;
    }

    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: AppConstants.backgroundColor
    }


    CustomMenuBar {
        id: mainMenu
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20
    }





    Rectangle {
        id: sceneGraphItem
        width: height*aspect_ratio

        anchors.margins: 20
        anchors.top: mainMenu.bottom
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        property double aspect_ratio: 1//16/9

        // Background
        ShaderEffect {
            id: tileBackground
            anchors.fill: parent

            property real tileSize: 16
            property color color1: Qt.rgba(0.9, 0.9, 0.9, 1);
            property color color2: Qt.rgba(0.85, 0.85, 0.85, 1);

            property size pixelSize: Qt.size(width / tileSize, height / tileSize);

            fragmentShader:
                "
            uniform lowp vec4 color1;
            uniform lowp vec4 color2;
            uniform highp vec2 pixelSize;
            varying highp vec2 qt_TexCoord0;
            void main() {
                highp vec2 tc = sign(sin(3.14159265358979323846 * qt_TexCoord0 * pixelSize));
                if (tc.x != tc.y)
                    gl_FragColor = color1;
                else
                    gl_FragColor = color2;
            }
            "
        }

        // Scene Graph
        Renderer {
            id: renderer
            anchors.fill: parent
            anchors.margins: 10

            //            // The transform is just to show something interesting..
            //            transform: [
            //                Rotation { id: rotation; axis.x: 0; axis.z: 0; axis.y: 1; angle: 0; origin.x: renderer.width / 2; origin.y: renderer.height / 2; },
            //                Translate { id: txOut; x: -renderer.width / 2; y: -renderer.height / 2 },
            //                Scale { id: scale; },
            //                Translate { id: txIn; x: renderer.width / 2; y: renderer.height / 2 }
            //            ]

            Behavior on opacity { NumberAnimation { duration: 500 } }
            opacity: 0
            Component.onCompleted: renderer.opacity = 1;


        }


        MouseArea {
            anchors.fill: renderer
            acceptedButtons: Qt.AllButtons
            hoverEnabled: false // onPositionChanged only called when mouse pressed

            property bool mouseFreezed: false
            property point mousePosFreezed
            property double mouseSensRotation: 0.001 // rad/pix
            property double mouseSensTranslation: 0.001 // m/pix
            property double mouseWheelSens : 0.01 // m/15deg

            onPositionChanged:
            {
                if (mouse.buttons == Qt.LeftButton || mouse.buttons == Qt.MidButton)
                {
                    if (!mouseFreezed)
                    {
                        mouseFreezed = true;
                        mousePosFreezed = Qt.point(mouse.x, mouse.y)
                    }
                    else
                    {
                        var diffX = mousePosFreezed.x - mouse.x
                        var diffY = mousePosFreezed.y - mouse.y
                        // LMB rotation, MMB translation
                        if (mouse.buttons == Qt.LeftButton)
                            renderer.rotationChanged(-diffY*mouseSensRotation, diffX*mouseSensRotation, 0.0)
                        if (mouse.buttons == Qt.MidButton)
                            renderer.translationChanged(diffX*mouseSensTranslation, diffY*mouseSensTranslation, 0.0)
                        // Reset cursor
                        renderer.moveCursor(mapToGlobal(mousePosFreezed.x, mousePosFreezed.y))
                    }
                }



            }

            onPressed: { renderer.setMouseButton(mouse.button, true) }
            onReleased: {
                renderer.setMouseButton(mouse.button, false)
                if (mouseFreezed)
                    mouseFreezed = false;
            }

            onWheel:
            {
                if (wheel.angleDelta.y !== 0)
                {
                    var angleY = wheel.angleDelta.y / 8
                    renderer.translationChanged(0.0, 0.0, angleY*mouseWheelSens)
                }

                if (wheel.angleDelta.x !== 0)
                {
                    var angleX = wheel.angleDelta.x / 8 * mouseWheelSens
                    renderer.translationChanged(angleY*mouseWheelSens, 0.0, 0.0)
                }
            }
        }


        // Just to show something interesting
        //    SequentialAnimation {
        //        PauseAnimation { duration: 5000 }
        //        ParallelAnimation {
        //            NumberAnimation { target: scale; property: "xScale"; to: 0.6; duration: 1000; easing.type: Easing.InOutBack }
        //            NumberAnimation { target: scale; property: "yScale"; to: 0.6; duration: 1000; easing.type: Easing.InOutBack }
        //        }
        //        NumberAnimation { target: rotation; property: "angle"; to: 80; duration: 1000; easing.type: Easing.InOutCubic }
        //        NumberAnimation { target: rotation; property: "angle"; to: -80; duration: 1000; easing.type: Easing.InOutCubic }
        //        NumberAnimation { target: rotation; property: "angle"; to: 0; duration: 1000; easing.type: Easing.InOutCubic }
        //        NumberAnimation { target: renderer; property: "opacity"; to: 0.5; duration: 1000; easing.type: Easing.InOutCubic }
        //        PauseAnimation { duration: 1000 }
        //        NumberAnimation { target: renderer; property: "opacity"; to: 0.8; duration: 1000; easing.type: Easing.InOutCubic }
        //        ParallelAnimation {
        //            NumberAnimation { target: scale; property: "xScale"; to: 1; duration: 1000; easing.type: Easing.InOutBack }
        //            NumberAnimation { target: scale; property: "yScale"; to: 1; duration: 1000; easing.type: Easing.InOutBack }
        //        }
        //        running: true
        //        loops: Animation.Infinite
        //    }

        Rectangle {
            id: labelFrame
            anchors.margins: -10
            radius: 5
            color: "white"
            border.color: "black"
            opacity: 0.8
            anchors.fill: label
        }

        Text {
            id: label
            anchors.bottom: renderer.bottom
            anchors.left: renderer.left
            anchors.right: renderer.right
            anchors.margins: 20
            wrapMode: Text.WordWrap
            text: "\nROTATE:\t\tLMB \nTRANSLATE:\tMMB\n\nThe blue rectangle with the vintage 'Q' is an FBO, rendered by the application in a dedicated background thread. The background thread juggles two FBOs, one that is being rendered to and one for displaying. The texture to display is posted to the scene graph and displayed using a QSGSimpleTextureNode."
        }


    }

    DeviceView { // ExpandableList // DeviceView
        id: deviceList
        anchors.left: parent.left
        anchors.right: sceneGraphItem.left
        anchors.top: mainMenu.bottom
        anchors.bottom: parent.bottom
    //    anchors.margins: 10
    }




}
