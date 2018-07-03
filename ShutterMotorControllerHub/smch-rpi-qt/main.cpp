#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimer>

Q_LOGGING_CATEGORY(MainLog, "Main")

#include "wiringPi.h"
#include "wiringPiSPI.h"

#include "DiagTerminal.h"
#include "Hub.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qCDebug(MainLog) << "initializing WiringPi";

    if (wiringPiSetup() != 0)
    {
        qCCritical(MainLog) << "failed to initialize WiringPi";
        return EXIT_FAILURE;
    }

    qCDebug(MainLog) << "initializing WiringPi SPI";

    int spiFd = wiringPiSPISetup(0, 4000000);

    if (spiFd < 0)
    {
        qCCritical(MainLog) << "failed to initialize WiringPi SPI";
        return EXIT_FAILURE;
    }

    qCDebug(MainLog) << "SPI file descriptor:" << spiFd;

    DiagTerminal diagTerminal;
    Hub hub;

    QTimer taskTimer;

    QObject::connect(&taskTimer, &QTimer::timeout, [&hub] {
        hub.task();
    });

    taskTimer.start(10);

    QObject::connect(&diagTerminal, &DiagTerminal::actionTriggered, [&diagTerminal, &hub](DiagTerminal::Action action) {
        switch (action)
        {
            case DiagTerminal::Action::ReadStatus:
                hub.readStatus();
                break;

            case DiagTerminal::Action::Shutter1Up:
                hub.shutter1Up();
                break;

            case DiagTerminal::Action::Shutter1Down:
                hub.shutter1Down();
                break;

            case DiagTerminal::Action::Shutter2Up:
                hub.shutter2Up();
                break;

            case DiagTerminal::Action::Shutter2Down:
                hub.shutter2Down();
                break;

            case DiagTerminal::Action::ScanUnits:
                hub.scanUnits();
                break;

            case DiagTerminal::Action::SelectDevice:
                hub.selectDevice(diagTerminal.selectedDeviceIndex());
                break;

            default:
                break;
        }
    });

    return a.exec();
}

//millis_time_t BlynkMillis()
//{
//    static QElapsedTimer t;

//    if (!t.isValid())
//    {
//        t.start();
//    }

//    return t.elapsed();
//}
