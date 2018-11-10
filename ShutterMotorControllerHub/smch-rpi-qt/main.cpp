#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimer>

Q_LOGGING_CATEGORY(MainLog, "Main")

#ifdef RASPBERRY_PI
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "radio/Radio.h"
#else
#include "mock/MockRadio.h"
#include <QDateTime>
#endif

#include "diagmenu/DiagMenu.h"
#include "diagmenu/DiagMenuTcpBackend.h"
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

namespace menu
{
    namespace id
    {
        static uint32_t deviceMenu;
    }

    void addBackNavigator(DiagMenu& m, uint32_t menuId)
    {
        m.addSubItem(menuId, DiagMenuItem::createBackNavigator('-', "Back"));
    }

    void createMainMenu(DiagMenu& m, DiagMenuTcpBackend& tcp)
    {
        id::deviceMenu = m.addItem(DiagMenuItem::createSubMenu('d', "Device Management"));

        m.addItem(DiagMenuItem::createSeparator());

        m.addItem(DiagMenuItem::createAction('x', "Exit", [&tcp]{ tcp.disconnectClient(); }));
    }

    uint32_t addScanDevicesItem(DiagMenu& m, const std::shared_ptr<hub::Hub>& hub)
    {
        return m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('s', "Scan remote devices", [&] {
            m.printMessage("Scanning...");
            hub->scanDevices().wait();
            m.printMessage("Finished");
        }));
    }

    void addDeviceControlItems(DiagMenu& m, const std::shared_ptr<hub::Hub>& hub)
    {
        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('q', "Query Status", []{}));

        m.addSubItem(id::deviceMenu, DiagMenuItem::createSeparator());

        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('e', "All: Shutter 1 & 2 Up", []{}));
        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('d', "All: Shutter 1 & 2 Down", []{}));
        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('r', "All: Shutter 1 Up", []{}));
        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('f', "All: Shutter 1 Down", []{}));
        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('t', "All: Shutter 2 Up", []{}));
        m.addSubItem(id::deviceMenu, DiagMenuItem::createAction('g', "All: Shutter 2 Down", []{}));

        m.addSubItem(id::deviceMenu, DiagMenuItem::createSeparator());

        for (int i = 0; i < 10; ++i)
        {
            uint32_t id = m.addSubItem(id::deviceMenu, DiagMenuItem::createSubMenu(static_cast<char>('0' + i), QString{ "Device %1 (SMRR%1)" }.arg(i)));

            m.addSubItem(id, DiagMenuItem::createAction('q', "Query Status", []{}));

            m.addSubItem(id, DiagMenuItem::createSeparator());

            m.addSubItem(id, DiagMenuItem::createAction('e', "Shutter 1 & 2 Up", []{}));
            m.addSubItem(id, DiagMenuItem::createAction('d', "Shutter 1 & 2 Down", []{}));
            m.addSubItem(id, DiagMenuItem::createAction('r', "Shutter 1 Up", []{}));
            m.addSubItem(id, DiagMenuItem::createAction('f', "Shutter 1 Down", []{}));
            m.addSubItem(id, DiagMenuItem::createAction('t', "Shutter 2 Up", []{}));
            m.addSubItem(id, DiagMenuItem::createAction('g', "Shutter 2 Down", []{}));

            m.addSubItem(id, DiagMenuItem::createSeparator());

            addBackNavigator(m, id);
        }

        m.addSubItem(id::deviceMenu, DiagMenuItem::createSeparator());
    }

    void createDeviceMenu(DiagMenu& m, const std::shared_ptr<hub::Hub>& hub)
    {
        addScanDevicesItem(m, hub);
        addDeviceControlItems(m, hub);
        addBackNavigator(m, menu::id::deviceMenu);
    }
}


int main(int argc, char *argv[])
{
    qsrand(time(nullptr));

    QCoreApplication a(argc, argv);

    if (setupHardware() != EXIT_SUCCESS)
        return EXIT_FAILURE;

    DiagMenuTcpBackend diagMenuTcpBackend(QHostAddress::Any, 23456);
    DiagMenu diagMenu(&diagMenuTcpBackend);

    QObject::connect(&diagMenuTcpBackend, &DiagMenuTcpBackend::connected, [&diagMenu] {
        diagMenu.reset();
    });

#ifdef RASPBERRY_PI
    auto radio = std::make_shared<radio::Radio>(0);
    auto hub = std::make_shared<hub::Hub>(radio);

    menu::createMainMenu(diagMenu, diagMenuTcpBackend);
    menu::createDeviceMenu(diagMenu, hub);

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

    diagMenuTcpBackend.startListening();

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
