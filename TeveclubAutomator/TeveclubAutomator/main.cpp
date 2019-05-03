#include <QtCore/QCoreApplication>

#include "Configuration.h"
#include "TeveclubService.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Configuration configuration{ QCoreApplication::applicationDirPath() + "/config.ini" };

    TeveclubService teveclubService{ configuration };

    qDebug() << "Logging in";

    teveclubService.login([](ITeveclubService::LoginResult result) {
        qDebug() << "Login finished";
    });

    return a.exec();
}
