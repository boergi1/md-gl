import QtQuick 2.0

Rectangle {
    id: root
    width: 1406; height: 536
    color: AppConstants.viewColor
    radius: AppConstants.viewRadius

    ListModel {
        id: animalsModel
        ListElement { name: "Name 1"; type: "Type 1"; aVisible: false}
        //        ListElement { name: "Name 2"; type: "Type 1"; aVisible: false }
        //        ListElement { name: "Name 3"; type: "Type 2"; aVisible: false }
        //        ListElement { name: "Name 4"; type: "Type 2"; aVisible: false }
        //        ListElement { name: "Name 5"; type: "Type 3"; aVisible: false }
    }

    Component {
        id: sectionHeader

        Rectangle {
            id: sectionHeaderRect
            width: 181
            color:"green"
            height: 50

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
                for(var i=0; i<animalsModel.count; i++){
                    var animal = animalsModel.get(i);
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
        width: 181
        height: parent.height
        model: animalsModel

        property string expandedSection: ""

        delegate: listdelegate

        section.property: "type"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeader

    }

    Component {
        id: listdelegate

        Rectangle {
            id: menuItem
            width: 181

            color: ListView.isCurrentItem ? "lightblue" : "white"
            visible: aVisible



            onVisibleChanged: {
                console.log("menuItem visible",visible)
                if(visible)
                    height = 55;
                else
                    height = 0;
            }

            Behavior on height {
                NumberAnimation { duration: 100 }
            }


            Text {
                id: text
                text: name
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
