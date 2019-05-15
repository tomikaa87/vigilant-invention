#include "TeveclubService.h"

#include <QBuffer>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>

#include "MainPage.h"
#include "TeachingPage.h"
#include "GamePage.h"

Q_LOGGING_CATEGORY(TeveclubServiceLog, "TeveclubService")

TeveclubService::TeveclubService(IConfiguration& configuration, QObject *parent)
    : QObject{ parent }
    , m_configuration{ configuration }
    , m_network{ new QNetworkAccessManager{this} }
{
}

void TeveclubService::login(std::function<void(LoginResult)>&& callback)
{
    QUrlQuery query;
    query.addQueryItem("tevenev", m_configuration.readValue("login/username").toString());
    query.addQueryItem("pass", m_configuration.readValue("login/password").toString());
    query.addQueryItem("x", "31");
    query.addQueryItem("y", "26");
    query.addQueryItem("login", "Gyere!");

    auto reply = m_network->post(createPostRequest(), query.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }] {
        if (reply->error() != QNetworkReply::NoError)
        {
            qCWarning(TeveclubServiceLog) << "Cannot login because of a network error:" << reply->errorString();
            callback(LoginResult::NetworkError);
            return;
        }

        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode != 302)
        {
            qCWarning(TeveclubServiceLog) << "Invalid response status code after login:" << statusCode;
            callback(LoginResult::NetworkError);
            return;
        }

        auto&& location = reply->header(QNetworkRequest::LocationHeader).toUrl();

        if (location.path() == "/error.pet")
        {
            qCWarning(TeveclubServiceLog) << "Invalid credentials";
            callback(LoginResult::InvalidCredentials);
            return;
        }

        qCDebug(TeveclubServiceLog) << "Logged in successfully";

        callback(LoginResult::Ok);
    });
}

void TeveclubService::feed(std::function<void(FeedResult)>&& callback)
{
    getPage("/myteve.pet", [this, callback{ std::move(callback) }](const bool succeeded, QByteArray&& content) {
        if (!succeeded)
        {
            qCWarning(TeveclubServiceLog) << "Cannot feed because main page couldn't be loaded";
            callback(FeedResult::NetworkError);
            return;
        }

        MainPage mainPage{ content };

        if (!mainPage.hasFeedingForm())
        {
            qCWarning(TeveclubServiceLog) << "Camel is already fed";
            callback(FeedResult::AlreadyFed);
            return;
        }

        const auto drinkAmount = mainPage.countEmptyDrinkSlots();
        const auto foodAmount = mainPage.countEmptyFoodSlots();

        qCDebug(TeveclubServiceLog).nospace()
            << "drinkAmount: " << drinkAmount
            << ", foodAmount: " << foodAmount;

        if (drinkAmount == 0 || foodAmount == 0)
        {
            qCWarning(TeveclubServiceLog) << "Camel is already fed";
            callback(FeedResult::AlreadyFed);
            return;
        }

        QUrlQuery query;
        query.addQueryItem("kaja", QString::number(foodAmount));
        query.addQueryItem("pia", QString::number(drinkAmount));
        query.addQueryItem("etet", "Mehet!");

        auto reply = m_network->post(createPostRequest("/myteve.pet"), query.toString(QUrl::FullyEncoded).toUtf8());

        connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }] {
            if (reply->error() != QNetworkReply::NoError)
            {
                qCWarning(TeveclubServiceLog) << "Cannot feed because of a network error:" << reply->errorString();
                callback(FeedResult::NetworkError);
                return;
            }

            callback(FeedResult::Ok);
        });
    });
}

void TeveclubService::teach(std::function<void(TeachResult)>&& callback)
{
    getPage("/tanit.pet", [this, callback{ std::move(callback) }](const bool succeeded, QByteArray&& content) {
        if (!succeeded)
        {
            qCWarning(TeveclubServiceLog) << "Cannot teach because main page couldn't be loaded";
            callback(TeachResult::NetworkError);
            return;
        }

        // Must convert from Latin-1 because teveclub ignores Accept-Charset.
        TeachingPage teachingPage{ QString::fromLatin1(content) };

        QUrlQuery query;

        if (teachingPage.hasFormWithSkillSelector())
        {
            auto skill = teachingPage.selectRandomSkill();
            if (skill.first.isEmpty())
            {
                qCWarning(TeveclubServiceLog) << "Cannot teach because skill could not be selected";
                callback(TeachResult::SkillSelectionFailed);
                return;
            }

            qCInfo(TeveclubServiceLog).noquote().nospace() << "Selected skill: " << skill.second << " (" << skill.first << ")";

            query.addQueryItem("tudomany", skill.first);
            query.addQueryItem("learn", "Tanulj teve!");
            query.addQueryItem("farmdoit", "tanit");
        }
        else if (teachingPage.hasForm())
        {
            query.addQueryItem("learn", "Tanulj teve!");
            query.addQueryItem("farmdoit", "tanit");
        }
        else
        {
            qCWarning(TeveclubServiceLog) << "Camel is already taught";
            callback(TeachResult::AlreadyTaught);
            return;
        }

        auto reply = m_network->post(createPostRequest("/tanit.pet"), query.toString(QUrl::FullyEncoded).toUtf8());

        connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }] {
            if (reply->error() != QNetworkReply::NoError)
            {
                qCWarning(TeveclubServiceLog) << "Cannot teach because of a network error:" << reply->errorString();
                callback(TeachResult::NetworkError);
                return;
            }

            auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            auto content = reply->readAll();

            callback(TeachResult::Ok);
        });
    });
}

void TeveclubService::play(std::function<void(PlayResult)>&& callback)
{
    bool ok;

    const auto minBet = m_configuration.readValue("game/minBet").toInt(&ok);
    if (!ok || minBet < 0 || minBet > 99999)
    {
        qCWarning(TeveclubServiceLog) << "Cannot play because of 'minBet' is configured improperly. Valid range is 0-99999.";
        callback(PlayResult::InvalidConfiguration);
        return;
    }

    const auto maxBet = m_configuration.readValue("game/maxBet").toInt(&ok);
    if (!ok || maxBet < 0 || maxBet > 99999 || maxBet < minBet)
    {
        qCWarning(TeveclubServiceLog) << "Cannot play because of 'maxBet' is configured improperly. Valid range is 0-99999 and it must be greater than or equal to 'minBet'.";
        callback(PlayResult::InvalidConfiguration);
        return;
    }

    getPage("/egyszam.pet", [this, callback{ std::move(callback) }, minBet, maxBet](const bool succeeded, QByteArray&& content) {
        if (!succeeded)
        {
            qCWarning(TeveclubServiceLog) << "Cannot play because main page couldn't be loaded";
            callback(PlayResult::NetworkError);
            return;
        }

        GamePage gamePage{ content };

        if (!gamePage.hasForm())
        {
            qCWarning(TeveclubServiceLog) << "Camel has already played";
            callback(PlayResult::AlreadyPlayed);
            return;
        }

        const auto bet =
            minBet == maxBet
                ? minBet
                : (qrand() % (maxBet - minBet) + minBet);

        qCInfo(TeveclubServiceLog) << "Placing bet:" << bet;

        QUrlQuery query;
        query.addQueryItem("honnan", QString::number(bet));
        query.addQueryItem("tipp", "Ez a tippem!");

        auto reply = m_network->post(createPostRequest("/egyszam.pet"), query.toString(QUrl::FullyEncoded).toUtf8());

        connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }] {
            if (reply->error() != QNetworkReply::NoError)
            {
                qCWarning(TeveclubServiceLog) << "Cannot play because of a network error:" << reply->errorString();
                callback(PlayResult::NetworkError);
                return;
            }

            callback(PlayResult::Ok);
        });
    });
}

void TeveclubService::getPage(const QString& path, std::function<void(bool succeeded, QByteArray&& content)>&& callback)
{
    auto&& request = createRequest(path);

    auto reply = m_network->get(request);

    connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }, path] {
        if (reply->error() != QNetworkReply::NoError)
        {
            qCWarning(TeveclubServiceLog).nospace() << "Network error while loading page '" << path << "': " << reply->errorString();
            callback(false, {});
            return;
        }

        qCDebug(TeveclubServiceLog) << "Page loaded:" << path;

        callback(true, reply->readAll());
    });
}

QNetworkRequest TeveclubService::createRequest(const QString& path)
{
    QNetworkRequest request{ QUrl{ "https://teveclub.hu" + path } };

    return request;
}

QNetworkRequest TeveclubService::createPostRequest(const QString& path)
{
    auto&& request = createRequest(path);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    return request;
}
