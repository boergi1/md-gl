pragma Singleton
import QtQuick 2.9

Item {
    property int wHeight
    property int wWidth

    property bool sessionPopupFinished


    // Colors
    readonly property color backgroundColor: Qt.lighter("#2d3037", 1.4)

    readonly property color buttonColor: "#202227"
    readonly property color buttonHoveredColor: Qt.lighter(buttonColor, 1.2)
    readonly property color buttonPressedColor: "#6ccaf2"
    readonly property color disabledButtonColor: "#555555"
    readonly property color activatedButtonColor: Qt.darker(infoColor, 1.3)

    readonly property color viewColor: Qt.lighter(buttonColor, 1.4)
    readonly property color delegate1Color: Qt.lighter(viewColor, 1.3)
    readonly property color delegate2Color: Qt.lighter(viewColor, 1.2)

    readonly property color textColor: Qt.darker(textActivatedColor, 1.2)
    readonly property color textActivatedColor: "#ffffff"
    readonly property color textDarkColor: "#232323"
    readonly property color disabledTextColor: "#777777"

    readonly property color lineColor: Qt.darker("#21be2b",1.2)
    readonly property color sliderColor: "#6ccaf2"

    readonly property color errorColor: "#ba3f62"
    readonly property color infoColor: "#3fba62"

    readonly property color yellowColor: Qt.hsla(0.169444444, 1, 0.4, 1)
    readonly property color orangeColor: Qt.hsla(0.091666667, 1, 0.64, 1)


//    readonly property color blueColor: Qt.hsla(200, 84, 36, 1)
      readonly property color blueColor: Qt.hsla(0.5555, 0.84, 0.36, 1)
    readonly property color blueLightColor: Qt.lighter(AppConstants.blueColor, 1.5)

    // Font sizes
    property real microFontSize: hugeFontSize * 0.2
    property real tinyFontSize: hugeFontSize * 0.4
    property real smallTinyFontSize: hugeFontSize * 0.5
    property real smallFontSize: hugeFontSize * 0.6
    property real mediumFontSize: hugeFontSize * 0.7
    property real bigFontSize: hugeFontSize * 0.8
    property real largeFontSize: hugeFontSize * 0.9
    property real hugeFontSize: (wHeight) * 0.05
    property real giganticFontSize: (wHeight) * 0.06

    // Some other values
    property real fieldMargin: fieldHeight * 0.3
    property real fieldMarginMedium: fieldMargin * 4/5
    property real fieldMarginSmall: fieldMargin * 2/5

    property real fieldHeight: wHeight * 0.08
    property real fieldHeightMedium: fieldHeight * 4/5
    property real fieldHeightSmall: fieldHeight * 2/5

    property real fieldRadius: fieldHeight * 0.1

    property real viewRadius: fieldRadius*2
    property real viewSpacing: 5

    property real animationStep: 200

    // Some helper functions
    function widthForHeight(h, ss)
    {
        return h/ss.height * ss.width;
    }

    function heightForWidth(w, ss)
    {
        return w/ss.width * ss.height;
    }

}
