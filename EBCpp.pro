TEMPLATE = app

TARGET = EBCpp

CONFIG -= qt
CONFIG += c++11
CONFIG -= app_bundle
CONFIG += console

DEFINES += E2000_PLUS_CONTROL

win32 {
    DEFINES += _WIN32_WINNT=0x0600
    LIBS += -lws2_32
}


SOURCES += \
    examples/main.cpp \
    \
    src/ebeventloop.cpp \
    src/ebfile.cpp \
    src/ebsemaphore.cpp \
    src/ebserialport.cpp \
    src/ebtimer.cpp \
    \
    src/socket/tcp/ebtcpclient.cpp \
    src/socket/tcp/ebtcpserver.cpp \
    src/socket/tcp/ebtcpserversocket.cpp \
    src/socket/tcp/ebtcpsocket.cpp \
    \
    src/socket/tcp/ssl/ebsslclient.cpp \
    src/socket/tcp/ssl/ebsslserver.cpp \
    src/socket/tcp/ssl/ebsslserversocket.cpp


HEADERS += \
    src/ebconfig.h \
    src/ebevent.h \
    src/ebeventloop.h \
    src/ebfile.h \
    src/ebsemaphore.h \
    src/ebserialport.h \
    src/ebtimer.h \
    \
    src/socket/ebserver.h \
    src/socket/ebserversocket.h \
    \
    src/socket/tcp/ebtcpclient.h \
    src/socket/tcp/ebtcpserver.h \
    src/socket/tcp/ebtcpserversocket.h \
    src/socket/tcp/ebtcpsocket.h \
    src/socket/tcp/tcp.h \
    \
    src/socket/tcp/ssl/ebsslclient.h \
    src/socket/tcp/ssl/ebsslserver.h \
    src/socket/tcp/ssl/ebsslserversocket.h
