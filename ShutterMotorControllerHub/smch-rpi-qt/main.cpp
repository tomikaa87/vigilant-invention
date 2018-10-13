#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimer>

Q_LOGGING_CATEGORY(MainLog, "Main")

#ifdef RASPBERRY_PI
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "DiagTerminal.h"
#include "Radio.h"
#else
#include "mock/MockRadio.h"
#include <QDateTime>
#endif

#include "hub/Hub.h"

#include <cstdio>
#include <memory>

int setupHardware()
{
#ifdef LINUX
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
#else
    qCDebug(MainLog) << "Setting up the hardware";

    qsrand(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);

    return EXIT_SUCCESS;
#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (setupHardware() != EXIT_SUCCESS)
        return EXIT_FAILURE;

#ifdef RASPBERRY_PI
    auto radio = std::make_shared<Radio>();
    auto hub = std::make_shared<Hub2>(radio);
//    DiagTerminal diagTerminal{ hub };

//    QTimer taskTimer;

//    QObject::connect(&taskTimer, &QTimer::timeout, [&hub] {
//        hub->task();
//    });

//    taskTimer.start(10);
#else
    auto mockRadio = std::make_shared<MockRadio>();
    auto hub = std::make_shared<hub::Hub>(mockRadio);
#endif

//    hub->execute(IRemoteControl::Command::ShutterDown, {
//                     IRemoteControl::DeviceIndex::D0_0,
//                     IRemoteControl::DeviceIndex::D0_1,
//                     IRemoteControl::DeviceIndex::D1_0,
//                     IRemoteControl::DeviceIndex::D2_1,
//                 });

//    hub->execute(IRemoteControl::Command::ShutterDown, {
//                     IRemoteControl::DeviceIndex::D4_0,
//                     IRemoteControl::DeviceIndex::D4_1,
//                     IRemoteControl::DeviceIndex::D5_0,
//                     IRemoteControl::DeviceIndex::D6_1,
//                 });

//    QObject::connect(&diagTerminal, &DiagTerminal::actionTriggered, [&diagTerminal, &hub](DiagTerminal::Action action) {
//        switch (action)
//        {
//            case DiagTerminal::Action::ReadStatus:
//                hub.readStatus();
//                break;

//            case DiagTerminal::Action::Shutter1Up:
//                hub.shutter1Up();
//                break;

//            case DiagTerminal::Action::Shutter1Down:
//                hub.shutter1Down();
//                break;

//            case DiagTerminal::Action::Shutter2Up:
//                hub.shutter2Up();
//                break;

//            case DiagTerminal::Action::Shutter2Down:
//                hub.shutter2Down();
//                break;

//            case DiagTerminal::Action::ScanUnits:
//                hub.scanUnits();
//                break;

//            case DiagTerminal::Action::SelectDevice:
//                hub.selectDevice(diagTerminal.selectedDeviceIndex());
//                break;

//            default:
//                break;
//        }
//    });

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
