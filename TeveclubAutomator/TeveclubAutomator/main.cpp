#include <QtCore/QCoreApplication>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>

#include <iostream>

#include "Configuration.h"
#include "TeveclubService.h"

void messageHandler(const QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    auto t = ' ';

    switch (type)
    {
    case QtDebugMsg: t = 'D'; break;
    case QtWarningMsg: t = 'W'; break;
    case QtCriticalMsg: t = 'E'; break;
    case QtFatalMsg: t = 'F'; break;
    case QtInfoMsg: t = 'I'; break;
    }

    auto entry = QString{ "[%1][%2]" }
        .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs))
        .arg(t);

    if (context.category && strcmp(context.category, "default") != 0)
        entry += QString{ "[%1]" }.arg(context.category);

    entry.append(' ').append(msg);

    std::cout << entry.toUtf8().constData() << std::endl;
}

void run(TeveclubService& teveclubService, bool feedEnabled, bool teachEnabled)
{
    struct State
    {
        State(const bool feed, const bool teach)
            : feed{ feed }
            , teach{ teach }
        {}

        void fed()
        {
            alreadyFed = true;

            if (!teach || (teach && alreadyTaught))
                QCoreApplication::exit();
        }

        void taught()
        {
            alreadyTaught = true;

            if (!feed || (feed && alreadyFed))
                QCoreApplication::exit();
        }

    private:
        const bool feed;
        const bool teach;
        bool alreadyFed = false;
        bool alreadyTaught = false;
    };

    auto state = std::make_shared<State>(feedEnabled, teachEnabled);

    qDebug() << "Logging in";

    teveclubService.login([&teveclubService, feedEnabled, teachEnabled, state](ITeveclubService::LoginResult result) {
        if (result != ITeveclubService::LoginResult::Ok)
        {
            qCritical() << "Login failed";
            QCoreApplication::exit(1);
            return;
        }

        qDebug() << "Login finished";

        if (feedEnabled)
        {
            teveclubService.feed([state](ITeveclubService::FeedResult) {
                qDebug() << "Feeding finished";

                state->fed();
            });
        }

        if (teachEnabled)
        {
            teveclubService.teach([state](ITeveclubService::TeachResult) {
                qDebug() << "Teaching finished";

                state->taught();
            });
        }
    });
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    QCoreApplication application{ argc, argv };

    qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));

    QCoreApplication::setApplicationName("Teveclub Automator");
    QCoreApplication::setApplicationVersion("1.1.1");

    Configuration configuration{ QCoreApplication::applicationDirPath() + "/TeveclubAutomator.ini" };

    TeveclubService teveclubService{ configuration };

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription("This application automates certain actions on teveclub.hu");
    (void)commandLineParser.addHelpOption();
    (void)commandLineParser.addVersionOption();

    commandLineParser.addOption(QCommandLineOption{
        QStringList{ "f", "feed" },
        "Fills the drink and food slots"
    });

    commandLineParser.addOption(QCommandLineOption{
        QStringList{ "t", "teach" },
        "Teaches a lesson of the current skill"
    });

    commandLineParser.process(application);

    const auto feedEnabled = commandLineParser.isSet("feed");
    const auto teachEnabled = commandLineParser.isSet("teach");

    if (!feedEnabled && !teachEnabled)
    {
        std::cout << "No action defined." << std::endl << std::endl;
        commandLineParser.showHelp(1);
    }

    qInfo().noquote() << "Version:" << QCoreApplication::applicationVersion();

    run(teveclubService, feedEnabled, teachEnabled);

    return application.exec();
}
