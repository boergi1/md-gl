import QtQuick 2.0

Item {


    height: expanded ? bodyHeight+headerHeight : headerHeight


    property bool expanded: false

    property int headerHeight: AppConstants.fieldHeight
    property int bodyHeight: AppConstants.fieldHeight*10




Rectangle
{
    id: expanderHeader
    anchors { top: parent.top; left: parent.left; right: parent.right }
    height: device.itemHeight

    radius: AppConstants.fieldRadius
    color: expanded ? AppConstants.blueColor : Qt.lighter(AppConstants.viewColor,1.5)
    border.color: "black"
    border.width: 2

    MouseArea {
        anchors.fill: parent
        onClicked:
        {


        }
    }
}
Rectangle
{
       id: expanderBody
}


}
