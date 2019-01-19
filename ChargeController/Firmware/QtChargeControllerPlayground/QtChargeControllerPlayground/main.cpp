#include "QtServerAdapter.h"
#include "ChargeControllerServer.h"

#include <QtCore/QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto&& serverAdapter = std::make_unique<QtServerAdapter>();

    if (!serverAdapter->startListening())
    {
        qCritical() << "failed to start listening";
        a.exit(1);
    }

    ChargeControllerServer server{ std::move(serverAdapter) };

    server.onSwitch([](bool on) {
        qDebug() << "Switch, on =" << on;
    });

    return a.exec();
}
