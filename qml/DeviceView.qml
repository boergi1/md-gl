import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import Shared 1.0

Rectangle {
    id: deviceViewRoot
    //    width: 1406; height: 536
    color: AppConstants.viewColor
    radius: AppConstants.viewRadius

    anchors.margins: 10



    Connections
    {
        id: deviceConnections
        target: deviceInterface
        Component.onCompleted:
        {
            console.log("Connections")
        }
        onDevicesChanged:
        {
            console.log("devices changed (",index,")", deviceInterface.devices)
        }
        onScanningChanged:
        {
            console.log("Scanning", deviceInterface.scanning)
        }

        onInvokeDeviceStateUpdate:
        {
            console.log("onInvokeDeviceStateUpdate",idx,state)
            //            if (deviceList.count > idx)
            //                deviceList.itemAtIndex(idx).deviceState = state;

        }
    }






    Rectangle
    {
        id: deviceViewHeader
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: "transparent"
        height: AppConstants.fieldHeight
        anchors.margins: 5

        Row {
            id: buttonLayout
            spacing: AppConstants.viewSpacing
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: AppConstants.mediumFontSize

            Repeater {
                id: buttonRepeater
                model: ["Scan", "Enable", "Disable"]
                AppButton {
                    id: repeatedButton
                    //    anchors.fill: parent
                    //      color: AppConstants.buttonColor
                    baseColor: AppConstants.infoColor
                    height: AppConstants.fieldHeight
                    width: (deviceViewHeader.width-(buttonRepeater.count-1)*buttonLayout.spacing)/buttonRepeater.count

                    clip: true

                    enabled: switch (index)
                             {
                             case 0: /*return (!deviceInterface.scanning /*&& deviceInterface.*/ return true;
                             case 1: return true;
                             case 2: return true;
                             default: return true;
                             }

                    onClicked: {
                        console.log("Clicked button:",modelData, index)
                        switch (index)
                        {
                        case 0: deviceInterface.startSearch(); break;
                        case 1:
                            //                            for (var i=0; i<deviceList.count; i++)
                            //                                deviceInterface.initializeDevice(i);
                            break;
                        case 2: break;
                        default: break;
                        }

                    }
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: AppConstants.mediumFontSize
                        //        font.bold: true
                        text: modelData

                        //   color: dropButton.enabled ? AppConstants.errorColor : AppConstants.disabledTextColor
                    }
                }
            }
        }
    }





    Rectangle {
        id: deviceListContainer
        color: "transparent"
        anchors.top: deviceViewHeader.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        //        anchors.fill: parent
        anchors.margins: 10


        ListView {
            id: deviceList
            width: parent.width
            height: parent.height

            model: deviceInterface.deviceInfo

            delegate: deviceComponent

            interactive: true // todo
            clip: true




            onCountChanged: {
                console.log("Count:",count)
                //                for(var i=0; i<deviceInterface.devices.length; i++)
                //                {
                //                    var device = deviceList.itemAtIndex(i)
                //                    console.log(device.deviceType)
                //                }
            }

            onModelChanged: {
                console.log("Model:",model)
                //                console.log("Section:",ListView.section,"current:",ListView.currentSection)
            }

            onCurrentItemChanged:
            {
                console.log("onCurrentItemChanged:",deviceList.currentIndex)
                //                    // Update the currently-selected item
                //                    currentSelectedItem = myDelegateModel.items.get(currentIndex).model;
                //                    // Log the Display Role
                //                    console.log(currentSelectedItem.display);
            }


        }



        Component {
            id: deviceComponent
            Item {
                id: device
                height: expanded ? device.totalHeight : headerHeight
                width: deviceList.width

                property bool currentItem: ListView.isCurrentItem

                property bool expanded: false
                property int headerHeight: AppConstants.fieldHeight
                property int totalHeight: deviceMenuHeader.height+deviceMenuBody.height

                property int deviceType: modelData.deviceType
                property int deviceState: modelData.deviceState

                onDeviceStateChanged:
                {
                    console.log("onDeviceStateChanged",index,deviceState);
                    if (deviceState === DeviceState.RECORD) deviceStateLED.startSlowBlinking()
                    else deviceStateLED.stopBlinking()
                }

                Rectangle
                {
                    id: deviceMenuHeader
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    height: device.headerHeight
                    radius: AppConstants.fieldRadius
                    color: expanded ? AppConstants.blueColor : Qt.lighter(AppConstants.viewColor,1.5)
                    border.color: "black"
                    border.width: 2


                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            ListView.currentIndex = deviceList.currentIndex;
                            device.expanded = !device.expanded
                        }
                    }

                    RowLayout
                    {
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: AppConstants.viewSpacing
                        clip: true

                        Rectangle
                        {
                            id: typeIcon
                            color: Qt.lighter(deviceMenuHeader.color, 1.5)
                            border.color: "black"
                            border.width: 1
                            radius: 5

                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: false
                            Layout.preferredHeight: parent.height
                            Layout.minimumHeight: parent.height*2/3
                            Layout.preferredWidth: Layout.preferredHeight

                            Image {
                                id: typeImage
                                anchors.fill: parent
                                source: switch (modelData.deviceType)
                                        {
                                        case DeviceType.Realsense: return "images/realsense_icon.png";
                                        case DeviceType.UVC: return "";
                                        default: return "";
                                        }
                                fillMode: Image.PreserveAspectFit
                            }
                        }


                        Text
                        {
                            text: modelData.deviceName
                            color: AppConstants.textColor
                            font.pixelSize: AppConstants.smallTinyFontSize

                            Layout.minimumWidth: 10
                            Layout.preferredWidth: parent.width-typeIcon.width-deviceStateLED.width -20
                            Layout.alignment: Qt.AlignCenter
                        }

                        Item
                        {
                            id: deviceStateLED

                            Layout.preferredHeight: AppConstants.fieldHeightSmall
                            Layout.preferredWidth: Layout.preferredHeight
                            Layout.alignment: Qt.AlignRight
                            Layout.rightMargin: Layout.preferredHeight

                            property bool blinkActive: false
                            property color baseColor: "darkgray"
                            property color stateColor: switch(device.deviceState)
                                                       {
                                                       case DeviceState.FOUND: "lightgray"; break;
                                                       case DeviceState.UNINIT: AppConstants.yellowColor; break;
                                                       case DeviceState.INIT: AppConstants.infoColor ; break;
                                                       case DeviceState.RECORD: AppConstants.errorColor; break;
                                                       case DeviceState.STOP: AppConstants.orangeColor ; break;
                                                       default: "black"
                                                       }

                            Rectangle
                            {
                                id: ledBackground
                                anchors.fill: parent
                                width: height
                                radius: height/2
                                color: "gray"
                                property int borderWidth: 2
                            }

                            Rectangle
                            {
                                id: ledLight
                                anchors.centerIn: ledBackground
                                height: ledBackground.height - 2*ledBackground.borderWidth
                                width: height
                                radius: height/2
                                color: deviceStateLED.stateColor
                            }

                            RectangularGlow {
                                anchors.fill: ledLight
                                glowRadius: ledLight.radius/2
                                spread: 0.01
                                color: Qt.lighter(ledLight.color, 1.5)
                                cornerRadius: ledLight.radius + glowRadius
                                visible: ledLight.color !== deviceStateLED.baseColor
                            }

                            Timer
                            {
                                id: ledBlinkTimer
                                interval: 500
                                repeat: true
                                onTriggered:
                                {
                                    if (deviceStateLED.blinkActive)
                                    {
                                        if ( ledLight.color === deviceStateLED.stateColor)
                                            ledLight.color = deviceStateLED.baseColor
                                        else
                                            ledLight.color = deviceStateLED.stateColor
                                    }
                                }
                            }

                            function startSlowBlinking()
                            {
                                deviceStateLED.blinkActive = true;
                                ledBlinkTimer.stop();
                                ledBlinkTimer.interval = 750;
                                ledBlinkTimer.start();
                            }

                            function startFastBlinking()
                            {
                                deviceStateLED.blinkActive = true;
                                ledBlinkTimer.stop();
                                ledBlinkTimer.interval = 175;
                                ledBlinkTimer.start();
                            }

                            function stopBlinking()
                            {
                                deviceStateLED.blinkActive = false;
                                ledBlinkTimer.stop();
                                button.color = baseColor;
                            }


                        }

                    } // !RowLayout

                } // !Rectangle

                Rectangle
                {
                    id: deviceMenuBody
                    anchors { top: deviceMenuHeader.bottom;  right: parent.right; left: parent.left }
                    radius: AppConstants.fieldRadius
                    color:  "gray"
                    height: device.expanded ? deviceMenuBody.totalHeight : 0

                    property bool expanded: false
                    property int totalHeight: streamProfilesHeader.height+streamProfilesBody.height+sensorOptionsHeader.height+sensorOptionsBody.height+3*AppConstants.viewSpacing
                    property int headerHeight: AppConstants.fieldHeightMedium
                    clip: true

                    Rectangle
                    {
                        id: streamProfilesHeader
                        anchors { /*top: parent.top;*/ bottom: streamProfilesBody.top; right: parent.right; left: parent.left; margins: AppConstants.viewSpacing }
                        height: deviceMenuBody.headerHeight
                        //   y: y_0
                        color:  Qt.lighter(AppConstants.viewColor, 1.2)
                        border.color: streamProfilesBody.expanded ? AppConstants.blueLightColor : AppConstants.textColor
                        border.width: 2
                        radius: AppConstants.fieldRadius

                        opacity: device.expanded ? 1 : 0

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: {
                                streamProfilesBody.expanded = !streamProfilesBody.expanded
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "Stream profile settings"
                            color: AppConstants.textColor
                            font.pixelSize: AppConstants.smallFontSize
                        }
                    }

                    Item
                    {
                        id: streamProfilesBody
                        anchors {/* top: streamProfilesHeader.bottom; */ right: parent.right; left: parent.left/*; bottom: parent.bottom*/ ; bottom: sensorOptionsHeader.top; }
                        height: streamProfilesBody.expanded ? streamProfilesBody.totalHeight : 0

                        visible: streamProfilesBody.expanded

                        property bool expanded: false

                        property int totalHeight: streamProfilesBody.notExpandedHeight+streamProfilesBody.expandedHeights

                        property int notExpandedHeight: deviceMenuBody.headerHeight*streamSettingsList.count+(streamSettingsList.spacing*(streamSettingsList.count+2))
                        property int expandedHeights: 0

                        property int headerHeight: deviceMenuBody.headerHeight
                        property int totalHeightSettings: AppConstants.fieldHeight*5

                        ListView
                        {
                            id: streamSettingsList
                            anchors.fill: parent
                            anchors.topMargin: AppConstants.viewSpacing
                            anchors.leftMargin: AppConstants.viewSpacing
                            anchors.rightMargin: AppConstants.viewSpacing
                            model: modelData.streamProfiles
                            delegate: streamSettingComponent
                            interactive: false
                            clip: true
                              spacing: AppConstants.viewSpacing

                            signal childExpandedChanged(bool expanded, int idx)
                            onChildExpandedChanged: {
                                if (expanded)
                                    streamProfilesBody.expandedHeights += streamProfilesBody.totalHeightSettings
                                else
                                    streamProfilesBody.expandedHeights -= streamProfilesBody.totalHeightSettings
                            }
                        } // !ListView

                        Component
                        {
                            id: streamSettingComponent
                            Item
                            {
                                id: streamSettingItem
                                height: streamSettingHeader.height+streamSettingBody.height
                                width: streamSettingsList.width

                                property bool expanded: false

                                onExpandedChanged:
                                    streamSettingsList.childExpandedChanged(expanded, index)

                                Rectangle
                                {
                                    id: streamSettingHeader
                                    anchors { top: parent.top; left: parent.left; right: parent.right}

                                    height: streamProfilesBody.headerHeight

                                    radius: AppConstants.fieldRadius
                                    color: Qt.lighter(AppConstants.viewColor,1.5)
                                    border.color: "black"
                                    border.width: 2

                                    RowLayout
                                    {
                                        id: settingMenuItemContent
                                        anchors.fill: parent
                                        Text
                                        {
                                            text: modelData.streamName
                                            font.pixelSize: AppConstants.smallFontSize
                                            color: AppConstants.textColor
                                            Layout.alignment: Qt.AlignLeft
                                            Layout.preferredHeight: font.pixelSize
                                            Layout.preferredWidth: streamSettingItem.width/2
                                            Layout.leftMargin: font.pixelSize
                                        }
                                        Switch
                                        {
                                            id: profileSwitch
                                            checked: false
                                            Layout.preferredHeight: indicator.implicitHeight
                                            Layout.preferredWidth: indicator.implicitWidth
                                            Layout.alignment: Qt.AlignRight
                                            Layout.rightMargin: indicator.implicitHeight

                                            enabled: streamSettingApplyButton.applied && ( streamSettingApplyButton.enabled || profileSwitch.checked )
                                            onCheckedChanged:
                                            {
                                                if (checked) deviceInterface.initializeDevice(device.index)
                                                else deviceInterface.uninitializeDevice(device.index)
                                            }
                                            indicator: Rectangle
                                            {
                                                id: switchIndicator
                                                implicitHeight: AppConstants.smallTinyFontSize
                                                implicitWidth: implicitHeight*2//60
                                                anchors.left: parent.left
                                                anchors.leftMargin: AppConstants.tinyFontSize/2
                                                x: profileSwitch.leftPadding
                                                y: parent.height / 2 - height / 2
                                                radius: height/2
                                                color:  profileSwitch.checked ? AppConstants.infoColor : "gray" //streamSettingApplyButton.applied ? ( profileSwitch.checked ? AppConstants.infoColor : AppConstants.yellowColor ) : "gray"
                                                border.color: profileSwitch.checked ? Qt.lighter(AppConstants.infoColor) : AppConstants.backgroundColor
                                                Rectangle {
                                                    x: profileSwitch.checked ? maxX : 0
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    height: parent.height + parent.height * 0.2
                                                    width: height
                                                    radius: height/2
                                                    color: profileSwitch.enabled ? ( profileSwitch.down ? "darkgray" : AppConstants.textColor ) : "gray"
                                                    property int maxX: parent.width - width
                                                }
                                            }
                                        }

                                        Rectangle
                                        {
                                            id: profileExpanderIcon

                                            color: streamSettingItem.expanded ? AppConstants.blueLightColor : Qt.lighter(streamSettingHeader.color, 1.5)
                                            border.color: "black"
                                            border.width: 2
                                            radius: height/2

                                            Layout.alignment: Qt.AlignRight
                                            Layout.preferredHeight: AppConstants.fieldHeightSmall
                                            Layout.preferredWidth: Layout.preferredHeight
                                            Layout.rightMargin: Layout.preferredHeight
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked:
                                                {
                                                    streamSettingItem.expanded = !streamSettingItem.expanded
                                                }
                                            }
                                            Image {
                                                anchors.fill: parent
                                                anchors.margins: 4
                                                source: "images/arrow-down.png"
                                                fillMode: Image.PreserveAspectFit
                                                rotation: streamSettingItem.expanded ? 180 : 0
                                                Behavior on rotation {
                                                    NumberAnimation { duration: 200 }
                                                }
                                            }
                                                                                   }
                                    } // !RowLayout
                                } // !Rectangle


                                Item
                                {
                                    id: streamSettingBody
                                    anchors { top: streamSettingHeader.bottom;  right: parent.right; left: parent.left/*; bottom: parent.bottom*/ }
                                    height: streamSettingItem.expanded ? streamProfilesBody.totalHeightSettings : 0
                                    anchors.topMargin: -streamSettingBackground.border.width
                                    property int totalHeight: streamSettingDelegates.height+streamSettingApplyButton.height

                                    Rectangle
                                    {
                                        id: streamSettingBackground
                                        anchors.fill: parent
                                        color: AppConstants.viewColor
                                        radius: AppConstants.fieldRadius
                                        border.color: "black"
                                        border.width: 2
                                    }
                                    RowLayout
                                    {
                                        id: streamSettingDelegates
                                        anchors { top: parent.top; /*bottom: parent.bottom;*/ left: parent.left; right: parent.right }
                                        anchors.margins: AppConstants.fieldMarginSmall
                                        height: streamSettingItem.expanded ? AppConstants.fieldHeight*streamSettingDelegates.count : 0
                                        clip: true

                                        property int count: streamSettingNameRepeater.count
                                        ColumnLayout
                                        {
                                            Layout.preferredWidth: parent.width/2
                                            Layout.preferredHeight: parent.height
                                            Repeater
                                            {
                                                id: streamSettingNameRepeater
                                                model: modelData.settingNames
                                                Layout.preferredWidth: parent.width
                                                Layout.preferredHeight: parent.height
                                                TextField
                                                {
                                                    id: streamSettingName
                                                    text: settingName+" :"
                                                    color: AppConstants.textColor

                                                    property string settingName: modelData

                                                    readOnly: true
                                                    font.bold: true
                                                    font.pixelSize: AppConstants.smallFontSize
                                                    Layout.preferredHeight: streamSettingDelegates.height/parent.count
                                                    Layout.preferredWidth: parent.width
                                                    Layout.alignment: Qt.AlignRight

                                                    horizontalAlignment: TextInput.AlignRight

                                                    background: Rectangle { color: "transparent" }
                                                }
                                            }
                                        }
                                        ColumnLayout
                                        {
                                            Layout.preferredWidth: parent.width/2
                                            Layout.preferredHeight: parent.height
                                            Repeater
                                            {
                                                id: streamSettingValuesRepeater
                                                model: modelData.settingValues
                                                Layout.preferredWidth: parent.width
                                                Layout.preferredHeight: parent.height
                                                ComboBox
                                                {
                                                    id: streamSettingValues
                                                    model: modelData
                                                    font.pixelSize: AppConstants.smallFontSize
                                                    height: streamSettingBody.height/parent.count
                                                    Layout.preferredHeight: streamSettingBody.height/parent.count
                                                    Layout.preferredWidth: parent.width*0.9
                                                    Layout.alignment: Qt.AlignCenter

                                                    onCurrentValueChanged:
                                                    {
                                                        switch (index)
                                                        {
                                                        case 3:
                                                            if (device.deviceType == DeviceType.Realsense)
                                                            {
                                                                var rs2_format = deviceInterface.rs2FormatToString(device.index, streamSettingValues.currentValue)
                                                                console.log("Format changed to" , rs2_format, "("+streamSettingValues.currentValue+")")
                                                            }
                                                            break;
                                                        default: break;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                    }


                                    AppButton {
                                        id: streamSettingApplyButton
                                        anchors { top: streamSettingDelegates.bottom; bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; margins: AppConstants.fieldMarginSmall }
                                        baseColor: AppConstants.infoColor
                                        height: streamSettingItem.expanded ? AppConstants.fieldHeight : 0
                                        width: parent.width/2
                                        clip: true
                                        enabled: profileSwitch.checked ? false : deviceInterface.validateStreamProfile(device.index, modelData.streamName, currentValues)

                                        property bool applied: appliedValues === currentValues
                                        property variant appliedValues
                                        property variant currentValues: getCurrentValues()
                                        Component.onCompleted: { appliedValues = currentValues } // Initially read values are the current values of the device

                                        function getCurrentValues()
                                        {
                                            var values = [];
                                            for ( var i=0;i<streamSettingDelegates.count;i++ )
                                                values.push(streamSettingValuesRepeater.itemAt(i).currentValue)
                                            return values;
                                        }
                                        onClicked: {
                                            deviceInterface.setStreamProfile(device.index, modelData.streamName, currentValues)
                                            appliedValues = currentValues
                                        }
                                        Text {
                                            anchors.centerIn: parent
                                            visible:  streamSettingItem.expanded
                                            horizontalAlignment: TextInput.AlignHCenter
                                            verticalAlignment: TextInput.AlignVCenter
                                            font.pixelSize: AppConstants.smallFontSize
                                            //font.bold: true
                                            text: "Apply"

                                            color: AppConstants.textDarkColor
                                        }
                                    }
                                } // !Item
                            } // !Item
                        } // !Component

                    } // !Rectangle
                    Rectangle
                    {
                        id: sensorOptionsHeader
                        anchors { /*top: streamProfilesBody.bottom;*/ bottom: deviceMenuBody.bottom;  right: parent.right; left: parent.left; margins: AppConstants.viewSpacing }
                        height: deviceMenuBody.headerHeight
                        //    y: y_0
                        color:  Qt.lighter(AppConstants.viewColor, 1.2)
                        border.color: sensorOptionsBody.expanded ? AppConstants.blueLightColor : AppConstants.textColor
                        border.width: 2
                        radius: AppConstants.fieldRadius


                        Text {
                            anchors.centerIn: parent
                            text: "Sensor settings"
                            color: AppConstants.textColor
                            font.pixelSize: AppConstants.smallFontSize
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                sensorOptionsBody.expanded = !sensorOptionsBody.expanded
                            }
                        }

                    }
                    Item
                    {
                        id: sensorOptionsBody
                        height: 0

                        anchors { top: sensorOptionsHeader.bottom;  right: parent.right; left: parent.left/*; bottom: parent.bottom*/ }


                        property bool expanded: false
                    }


                }






                //                states: State {
                //                    name: "Details"

                //                    //                    PropertyChanges { target: background; color: "white" }
                //                    //                    PropertyChanges { target: recipeImage; width: 130; height: 130 } // Make picture bigger
                //                    //  PropertyChanges { target: device; detailsOpacity: 1; x: 0 } // Make details visible
                //                    //                    PropertyChanges { target: recipe; height: listView.height } // Fill the entire list area with the detailed view

                //                    //                    // Move the list so that this item is at the top.
                //                    //                    PropertyChanges { target: recipe.ListView.view; explicit: true; contentY: recipe.y }

                //                    //                    // Disallow flicking while we're in detailed view
                //                    //                    PropertyChanges { target: recipe.ListView.view; interactive: false }
                //                }

                //                transitions: Transition {
                //                    // Make the state changes smooth
                //                    ParallelAnimation {
                //                        ColorAnimation { property: "color"; duration: 500 }
                //                        NumberAnimation { duration: 300; properties: "detailsOpacity,x,contentY,height,width" }
                //                    }
                //                }


                Component.onCompleted: {
                    console.log("completed",index,"visible",visible)
                    //   visible = true
                }

                onVisibleChanged: {
                    console.log("device visible",visible)
                    //                    if(visible)
                    //                    {
                    //                   //     opacity = 1;
                    //                        height = iHeight;
                    //                    }
                    //                    else
                    //                    {
                    //                   //     opacity = 1;
                    //                        height = 0;
                    //                    }
                }

                //                Behavior on height {
                //                    NumberAnimation { duration: 1000 }
                //                }



                //                Connections {
                //                    id: deviceConnections
                //                    target: modelData
                //                    Component.onCompleted: { console.log("modelData connections")}
                //                    onDeviceStateChanged: {console.log("device state")}
                //                }






                //                MouseArea {
                //                    anchors.fill: parent
                //                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                //                    onClicked: {
                //                        deviceList.currentIndex = index;
                //                        console.log("clicki menuItem", deviceList.currentIndex)
                //                        //                        if (mouse.button == Qt.LeftButton)
                //                        //                        {
                //                        //                            selected = true
                //                        //                            deviceList.currentIndex = index;
                //                        //                        }
                //                        //                        else if (mouse.button == Qt.RightButton)
                //                        //                        {
                //                        //                            selected = false
                //                        //                        }
                //                    }
                //                }

            }

        }
    }
}

