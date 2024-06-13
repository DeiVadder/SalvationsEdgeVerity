QT += quick

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

OBJECTS_DIR = generated/obj
MOC_DIR = generated/moc
RCC_DIR = generated/rsc
UI_DIR = generated/ui
QMLCACHE_DIR = generated/qml

SOURCES += \
        calculatesteps.cpp \
        main.cpp

RESOURCES += qml.qrc \
    rsc.qrc

OBJECTS_DIR = generated/obj
MOC_DIR = generated/moc
RCC_DIR = generated/rsc
UI_DIR = generated/ui
QMLCACHE_DIR = generated/qml

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    calculatesteps.h

DISTFILES += \
    build/WebView/SalvationsEdgeVerity.html \
    build/WebView/SalvationsEdgeVerity.js \
    build/WebView/SalvationsEdgeVerity.wasm \
    build/WebView/qtloader.js \
    build/WebView/qtlogo.svg
