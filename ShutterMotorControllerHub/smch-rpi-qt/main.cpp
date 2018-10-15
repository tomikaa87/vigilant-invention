#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimer>

Q_LOGGING_CATEGORY(MainLog, "Main")

#ifdef RASPBERRY_PI
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "DiagTerminal.h"
#include "radio/Radio.h"
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
#endif

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    qsrand(time(nullptr));

    QCoreApplication a(argc, argv);

    if (setupHardware() != EXIT_SUCCESS)
        return EXIT_FAILURE;

#ifdef RASPBERRY_PI
    auto radio = std::make_shared<radio::Radio>(0);
    auto hub = std::make_shared<hub::Hub>(radio);

    hub->scanDevices().wait();

//    auto f = radio->readStatus("SMRR4");
//    f.wait();
//    auto res = f.get();

//    for (const auto& msg: res.messages)
//    {
//        qCDebug(MainLog) << "received message:";
//        print_protocol_message(&msg);
//    }

//    int x = 0;
//    auto hub = std::make_shared<hub::Hub>(radio);
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

//    std::thread t1{ [hub] {
//        for (int i = 0; i < 500; ++i)
//        {
//            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
//            hub->scanDevices().wait();
//        }

//        qCDebug(MainLog) << "scan 1 finished";
//    }};

//    std::thread t2{ [hub] {
//        for (int i = 0; i < 500; ++i)
//        {
//            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
//            hub->scanDevices().wait();
//        }

//        qCDebug(MainLog) << "scan 2 finished";
//    }};

//    std::thread t3{ [hub] {
//        for (int i = 0; i < 500; ++i)
//        {
//            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
//            hub->scanDevices().wait();
//        }

//        qCDebug(MainLog) << "scan 3 finished";
//    }};

//    hub->scanDevices();

//    hub->execute(hub::Command::ShutterDown, {
//                 hub::DeviceIndex::D0_0,
//                 hub::DeviceIndex::D0_1,
//                 hub::DeviceIndex::D1_0,
//                 hub::DeviceIndex::D2_1,
//    });

//    qCDebug(MainLog) << "hub command executed";

//    f.wait();

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
