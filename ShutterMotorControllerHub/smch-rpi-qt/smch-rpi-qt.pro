QT -= gui
QT += network

CONFIG += gnu++14 console
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

DEFINES += LINUX

SOURCES += \
    main.cpp \
    nrf24.c \
    DiagTerminal.cpp \
    Radio.cpp \
    radio_protocol.c \
    wiring-pi/wiringPi/wiringPi.c \
    wiring-pi/wiringPi/wiringPiSPI.c \
    wiring-pi/wiringPi/softPwm.c \
    wiring-pi/wiringPi/softTone.c \
    wiring-pi/wiringPi/wiringPiI2C.c \
    wiring-pi/wiringPi/piHiPri.c \
    Hub.cpp \
    blynk-library/src/utility/BlynkDebug.cpp \
    blynk-library/src/utility/BlynkHandlers.cpp \
    blynk-library/src/utility/BlynkTimer.cpp \
    blynk-library/src/utility/utility.cpp

HEADERS += \
    nrf24.h \
    DiagTerminal.h \
    Radio.h \
    radio_protocol.h \
    wiring-pi/wiringPi/wiringPi.h \
    wiring-pi/wiringPi/wiringPiSPI.h \
    wiring-pi/version.h \
    wiring-pi/wiringPi/softPwm.h \
    wiring-pi/wiringPi/softTone.h \
    wiring-pi/wiringPi/wiringPiI2C.h \
    Hub.h \
    blynk-library/src/Blynk/BlynkApi.h \
    blynk-library/src/Blynk/BlynkConfig.h \
    blynk-library/src/Blynk/BlynkDebug.h \
    blynk-library/src/Blynk/BlynkDetectDevice.h \
    blynk-library/src/Blynk/BlynkEveryN.h \
    blynk-library/src/Blynk/BlynkHandlers.h \
    blynk-library/src/Blynk/BlynkParam.h \
    blynk-library/src/Blynk/BlynkProtocol.h \
    blynk-library/src/Blynk/BlynkProtocolDefs.h \
    blynk-library/src/Blynk/BlynkTemplates.h \
    blynk-library/src/Blynk/BlynkTimer.h \
    blynk-library/src/Blynk/BlynkWidgetBase.h \
    BlynkSocket.h \
    PrivateConfig.h \
    blynk-library/src/utility/BlynkDateTime.h \
    blynk-library/src/utility/BlynkFifo.h \
    blynk-library/src/utility/BlynkUtility.h \
    IHub.h \
    IRemoteControl.h \
    IRadio.h

INCLUDEPATH += \
    wiring-pi/wiringPi \
    blynk-library/src
