QT -= gui
QT += network

CONFIG += console
CONFIG += c++14
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = smch-rpi-qt
target.path = smch-rpi-qt

INSTALLS += target

QMAKE_CFLAGS += -std=gnu11 # gnu11 is used instead of c11 to support POSIX P1003.1b-1993

contains(DEFINES, RASPBERRY_PI) {
    message("Compiling for Raspberry Pi")

    CONFIG += gnu++14
    DEFINES += LINUX

    SOURCES += \
        main.cpp \
        DiagTerminal.cpp \
        radio/nrf24.c \
        radio/Radio.cpp \
        3rdparty/wiring-pi/wiringPi/wiringPi.c \
        3rdparty/wiring-pi/wiringPi/wiringPiSPI.c \
        3rdparty/wiring-pi/wiringPi/softPwm.c \
        3rdparty/wiring-pi/wiringPi/softTone.c \
        3rdparty/wiring-pi/wiringPi/wiringPiI2C.c \
        3rdparty/wiring-pi/wiringPi/piHiPri.c \
        3rdparty/blynk-library/src/utility/BlynkDebug.cpp \
        3rdparty/blynk-library/src/utility/BlynkHandlers.cpp \
        3rdparty/blynk-library/src/utility/BlynkTimer.cpp \
        3rdparty/blynk-library/src/utility/utility.cpp

    HEADERS += \
        DiagTerminal.h \
        radio/nrf24.h \
        radio/Radio.h \
        3rdparty/wiring-pi/wiringPi/wiringPi.h \
        3rdparty/wiring-pi/wiringPi/wiringPiSPI.h \
        3rdparty/wiring-pi/version.h \
        3rdparty/wiring-pi/wiringPi/softPwm.h \
        3rdparty/wiring-pi/wiringPi/softTone.h \
        3rdparty/wiring-pi/wiringPi/wiringPiI2C.h \
        3rdparty/blynk-library/src/Blynk/BlynkApi.h \
        3rdparty/blynk-library/src/Blynk/BlynkConfig.h \
        3rdparty/blynk-library/src/Blynk/BlynkDebug.h \
        3rdparty/blynk-library/src/Blynk/BlynkDetectDevice.h \
        3rdparty/blynk-library/src/Blynk/BlynkEveryN.h \
        3rdparty/blynk-library/src/Blynk/BlynkHandlers.h \
        3rdparty/blynk-library/src/Blynk/BlynkParam.h \
        3rdparty/blynk-library/src/Blynk/BlynkProtocol.h \
        3rdparty/blynk-library/src/Blynk/BlynkProtocolDefs.h \
        3rdparty/blynk-library/src/Blynk/BlynkTemplates.h \
        3rdparty/blynk-library/src/Blynk/BlynkTimer.h \
        3rdparty/blynk-library/src/Blynk/BlynkWidgetBase.h \
        3rdparty/blynk-library/src/utility/BlynkDateTime.h \
        3rdparty/blynk-library/src/utility/BlynkFifo.h \
        3rdparty/blynk-library/src/utility/BlynkUtility.h \
        blynk/BlynkSocket.h \
        PrivateConfig.h

    INCLUDEPATH += \
        3rdparty/wiring-pi/wiringPi \
        3rdparty/blynk-library/src
} else {
    message("Compiling for Desktop")

    HEADERS += \
        mock/MockHub.h

    SOURCES += \
        main.cpp \
        mock/MockHub.cpp
}

HEADERS += \
    mock/MockRadio.h \
    radio/IRadio.h \
    hub/IRemoteControl.h \
    hub/Hub.h \
    hub/IHub.h \
    hub/Task.h \
    hub/RemoteDevice.h \
    hub/RemoteDeviceStatus.h \
    radio/Command.h \
    radio/Result.h \
    hub/Command.h \
    hub/DeviceIndex.h \
    hub/Device.h \
    hub/SubDevice.h \
    logging/LoggingHelpers.h \
    radio/Response.h \
    radio/Task.h \
    OperationQueue.h \
    radio/Request.h \
    radio/Utils.h \
    radio/radio_protocol.h

SOURCES += \
    mock/MockRadio.cpp \
    hub/Hub.cpp \
    logging/LoggingHelpers.cpp \
    radio/radio_protocol.c \
    radio/RadioUtils.cpp
