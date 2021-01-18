import QtQuick 2.12
import QtQuick.Controls 2.12


import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.12


MenuBar {
    id: menuBar

    //    font.pointSize: AppConstants.giganticFontSize
    //    style: MenuBarStyle {
    //        font.pixelSize: 14
    //    }

    Menu {
        title: qsTr("File")
        Action { text: qsTr("New...") }
        Action { text: qsTr("Open...") }
        Action { text: qsTr("Save") }
        Action { text: qsTr("Save As...") }
        MenuSeparator { }
        Action { text: qsTr("Quit") }
    }
    Menu {
        title: qsTr("Edit")
        Action { text: qsTr("Cut") }
        Action { text: qsTr("Copy") }
        Action { text: qsTr("Paste") }
    }
    Menu {
        title: qsTr("Help")
        //        font.pixelSize: 100
        Action { text: qsTr("About") }
    }

    delegate: MenuBarItem {
        id: menuBarItem
        height: menuBar.height-1
        contentItem: Text {
            text: menuBarItem.text
            font: menuBarItem.font

            opacity: enabled ? 1.0 : 0.3
            color: menuBarItem.highlighted ? AppConstants.textColor : AppConstants.textColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight

            minimumPixelSize: 5
            //      font.pixelSize: AppConstants.smallFontSize
            fontSizeMode: Text.Fit

            Component.onCompleted: font.pixelSize = AppConstants.giganticFontSize
        }

        background: Rectangle {
            id: menuBackground
//            implicitWidth: 100
            implicitHeight: menuBar.height-1
            opacity: enabled ? 1 : 0.3
            color: menuBarItem.highlighted ? "#21be2b" : AppConstants.buttonColor

            Rectangle {
                color: AppConstants.lineColor
                height: parent.height
                width: 1
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }


        }
    }

    background: Rectangle {
        id: menuBarBackground
        //        implicitWidth: 40
        //        implicitHeight: 40
        color: AppConstants.viewColor

        Rectangle {
            color: Qt.lighter("gray", 1)
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
        }
    }


    //    Menu {

    //        title: qsTr("&File")

    //        Action { text: qsTr("&New...") }
    //        Action { text: qsTr("&Open...") }
    //        Action { text: qsTr("&Save") }
    //        Action { text: qsTr("Save &As...") }
    //        MenuSeparator { }
    //        Action { text: qsTr("&Quit") }
    //    }
    //    Menu {
    //        title: qsTr("&Edit")
    //        Action { text: qsTr("Cu&t") }
    //        Action { text: qsTr("&Copy") }
    //        Action { text: qsTr("&Paste") }
    //    }
    //    Menu {
    //        title: qsTr("&Help")
    //        Action { text: qsTr("&About") }
    //    }

}
