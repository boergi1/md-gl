import QtQuick 2.0
import Shared 1.0



Rectangle {
    id: deviceViewRoot
    //    width: 1406; height: 536
    color: AppConstants.viewColor
    radius: AppConstants.viewRadius

    anchors.margins: 10


    ListModel {
        id: devicesModel
        ListElement { name: "Name 1"; type: "Type 1"; aVisible: false}
        //        ListElement { name: "Name 2"; type: "Type 1"; aVisible: false }
        //        ListElement { name: "Name 3"; type: "Type 2"; aVisible: false }
        //        ListElement { name: "Name 4"; type: "Type 2"; aVisible: false }
        //        ListElement { name: "Name 5"; type: "Type 3"; aVisible: false }
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
            spacing: 5
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



    Component {
        id: sectionHeader



        Rectangle {
            id: sectionHeaderRect
            width: 181
            color:"green"
            height: 50



            Component.onCompleted: { console.log("header alive")}

            property bool isExpanded: false
            property string currentExpandedSection: ListView.view.expandedSection

            //            onCurrentExpandedSectionChanged: {
            //                if(currentExpandedSection === section)
            //                    isExpanded = true;
            //                else
            //                    isExpanded = false;
            //            }

            onIsExpandedChanged: {
                console.log("Expanded to",isExpanded)
                if(isExpanded){
                    color = "blue";
                    ListView.view.expandedSection = section;
                }
                else
                    color = "green";
                for(var i=0; i<devicesModel.count; i++){
                    var animal = devicesModel.get(i);
                    if(section === animal.type)
                        animal.aVisible = sectionHeaderRect.isExpanded;
                }
            }

            Text {
                id: sectionHeaderText
                text: section
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    sectionHeaderRect.isExpanded = !sectionHeaderRect.isExpanded;
                    console.log("clicki Header")
                }
            }
        }
    }


    ListView {
        id: listing
        //        width: 181
        //        height: parent.height


        anchors.top: deviceViewHeader.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        //        anchors.fill: parent
        anchors.margins: 10


        model: deviceInterface.deviceInfo

        property string expandedSection: ""
        property int itemHeight: 55

        delegate: listdelegate

        section.property: "deviceType"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeader

        onModelChanged:
        {
            console.log("model to",model)
        }

        section.onPropertyChanged:
        {
            console.log("property to",section.property)
        }

    }

    Component {
        id: listdelegate

        Rectangle {
            id: menuItem
            width: listing.width
            height: listing.itemHeight

            color: ListView.isCurrentItem ? "lightblue" : "white"
            //    visible: modelData.isVisible



            onVisibleChanged: {

                console.log("menuItem visible",visible,modelData.isVisible)
                if(visible)
                    height = listing.itemHeight;
                else
                    height = 0;
            }

            Behavior on height {
                NumberAnimation { duration: 100 }
            }


            Text {
                id: text
                text: modelData.deviceName
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listing.currentIndex = index;
                    console.log("clicki menuItem",listing.currentIndex)
                }
            }
        }
    }
}
