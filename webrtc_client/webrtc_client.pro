QT += quick websockets multimedia
CONFIG += c++20
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += WEBRTC_WIN NOMINMAX WINDOWS_LEAN_AND_MEAN QT_DEPRECATED_WARNINGS QT_NO_KEYWORDS

QMAKE_CXXFLAGS_RELEASE += /MT /bigobj
QMAKE_CFLAGS_RELEASE += /MT /bigobj
QMAKE_CXXFLAGS_DEBUG += /MTd /bigobj
QMAKE_CFLAGS_DEBUG += /MTd /bigobj
QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrtd.lib

SOURCES += \
        D:/Workplace/webrtc-checkout/src/test/test_video_capturer.cc \
        D:/Workplace/webrtc-checkout/src/test/vcm_capturer.cc \
        Observer_client.cpp \
        capturer_track_source.cpp \
        conductor.cpp \
        data_socket.cpp \
        main.cpp \
        video_renderer.cpp \
        websocket_client.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

INCLUDEPATH += D:/Workplace/webrtc-checkout/src
INCLUDEPATH += D:/Workplace/webrtc-checkout/src/base
INCLUDEPATH += D:/Workplace/webrtc-checkout/src/third_party
INCLUDEPATH += D:/Workplace/webrtc-checkout/src/third_party/abseil-cpp
INCLUDEPATH += D:/Workplace/webrtc-checkout/src/third_party/libyuv/include
DEPENDPATH += D:/Workplace/webrtc-checkout/src

LIBS += -LD:/Workplace/webrtc-checkout/src/out/webrtc_clang/obj \
        -lwebrtc

LIBS += -lws2_32 -lole32 -loleaut32 -lstrmiids -luser32 -lWinmm -liphlpapi -ldmoguids -lwmcodecdspuuid -lamstrmid -lmsdmo -lSecur32
LIBS += -ldbghelp -ldnsapi -lmsimg32 -lpsapi -lshlwapi -lusp10 -lversion -lwininet -lwinmm -lws2_32 -ldelayimp -lcrypt32 -liphlpapi -lsecur32 -ldmoguids -lwmcodecdspuuid -lamstrmid -lmsdmo -lstrmiids -ld3d11 -ldxgi -lAdvapi32 -lOle32 -lOleAut32


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    D:/Workplace/webrtc-checkout/src/test/test_video_capturer.h \
    D:/Workplace/webrtc-checkout/src/test/vcm_capturer.h \
    Observer_client.h \
    capturer_track_source.h \
    conductor.h \
    data_socket.h \
    video_renderer.h \
    websocket_client.h
