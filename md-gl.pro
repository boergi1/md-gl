QT += quick quickcontrols2 qml
CONFIG += c++11

# To make threaded gl check...
QT += core-private gui-private

HEADERS += threadrenderer.h logorenderer.h \
    devicebaseclass.h \
    deviceinfo.h \
    deviceinterface.h \
    globaltypes.h \
    realsensedevice.h
SOURCES += threadrenderer.cpp logorenderer.cpp main.cpp \
    devicebaseclass.cpp \
    deviceinfo.cpp \
    deviceinterface.cpp \
    realsensedevice.cpp


RESOURCES += \
    images.qrc \
    qml.qrc

#target.path = $$[QT_INSTALL_EXAMPLES]/quick/scenegraph/textureinthread
#INSTALLS += target

OTHER_FILES +=

INCLUDES += /usr/include
LIBS += -lrealsense2

DISTFILES += \
    python/img_classification_pretrained.py \
    python/img_classification_tl.py \
    python/mask_rcnn.py \
    python/tensors.py
