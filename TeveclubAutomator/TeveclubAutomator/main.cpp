#include <QtCore/QCoreApplication>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>

#include <iostream>

#include "Actions.h"
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

void run(TeveclubService& teveclubService, Actions& actions)
{
    qDebug() << "Logging in";

    teveclubService.login([&teveclubService, &actions](ITeveclubService::LoginResult result) {
        if (result != ITeveclubService::LoginResult::Ok)
        {
            qCritical() << "Login failed";
            QCoreApplication::exit(1);
            return;
        }

        qDebug() << "Login finished";

        if (actions.isRequired(Actions::Kind::Feed))
        {
            teveclubService.feed([&actions](ITeveclubService::FeedResult) {
                qDebug() << "Feeding finished";

                actions.complete(Actions::Kind::Feed);
            });
        }

        if (actions.isRequired(Actions::Kind::Teach))
        {
            teveclubService.teach([&actions](ITeveclubService::TeachResult) {
                qDebug() << "Teaching finished";

                actions.complete(Actions::Kind::Teach);
            });
        }

        if (actions.isRequired(Actions::Kind::Play))
        {
            // TODO implement
            qDebug() << "Playing finished";

            actions.complete(Actions::Kind::Play);
        }
    });
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    QCoreApplication application{ argc, argv };

    qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));

    QCoreApplication::setApplicationName("Teveclub Automator");
    QCoreApplication::setApplicationVersion("1.2.0");

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

    commandLineParser.addOption(QCommandLineOption{
        QStringList{ "p", "play" },
        "Plays the Wonderful One Number game"
    });

    commandLineParser.process(application);

    Actions actions;

    if (commandLineParser.isSet("feed"))
        actions.require(Actions::Kind::Feed);

    if (commandLineParser.isSet("teach"))
        actions.require(Actions::Kind::Teach);

    if (commandLineParser.isSet("play"))
        actions.require(Actions::Kind::Play);

    if (actions.isEmpty())
    {
        std::cout << "No action defined." << std::endl << std::endl;
        commandLineParser.showHelp(1);
    }

    actions.setCompletionCallback([] {
        qInfo() << "Completed";
        QCoreApplication::exit();
    });

    qInfo().noquote() << "Version:" << QCoreApplication::applicationVersion();

    run(teveclubService, actions);

    return application.exec();
}
