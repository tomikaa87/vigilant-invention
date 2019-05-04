#include "TeveclubService.h"

#include <QBuffer>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include "MainPage.h"

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
            qCWarning(TeveclubServiceLog) << "network error:" << reply->errorString();
            callback(LoginResult::NetworkError);
            return;
        }

        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode != 302)
        {
            qCWarning(TeveclubServiceLog) << "invalid response status code:" << statusCode;
            callback(LoginResult::NetworkError);
            return;
        }

        auto&& location = reply->header(QNetworkRequest::LocationHeader).toUrl();

        if (location.path() == "/error.pet")
        {
            qCWarning(TeveclubServiceLog) << "invalid credentials";
            callback(LoginResult::InvalidCredentials);
            return;
        }

        qCDebug(TeveclubServiceLog) << "logged in successfully";

        callback(LoginResult::Ok);
    });
}

void TeveclubService::feed(std::function<void(FeedResult)>&& callback)
{
    getMainPage([this, callback{ std::move(callback) }](bool succeeded, QByteArray&& content) {
        if (!succeeded)
        {
            qCWarning(TeveclubServiceLog) << "feeding failed, main page couldn't be loaded";
            callback(FeedResult::NetworkError);
            return;
        }

        MainPage mainPage{ content };

        if (!mainPage.hasFeedingForm())
        {
            qCWarning(TeveclubServiceLog) << "feeding is unnecessary";
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
            qCWarning(TeveclubServiceLog) << "feeding is unnecessary";
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
                qCWarning(TeveclubServiceLog) << "network error:" << reply->errorString();
                callback(FeedResult::NetworkError);
                return;
            }

            auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            auto content = reply->readAll();

            callback(FeedResult::Ok);
        });
    });
}

void TeveclubService::teach(std::function<void(TeachResult)>&& callback)
{
    auto&& request = createPostRequest();

    /*
     * form name = tanitb
     * formdoit = tanit
     * input submit name learn
     */

    QUrlQuery query;

    auto reply = m_network->post(request, query.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }]{

    });
}

void TeveclubService::getMainPage(std::function<void(bool succeeded, QByteArray&& content)>&& callback)
{
    auto&& request = createRequest("/myteve.pet");

    auto reply = m_network->get(request);

    connect(reply, &QNetworkReply::finished, [reply, callback{ std::move(callback) }] {
        if (reply->error() != QNetworkReply::NoError)
        {
            qCWarning(TeveclubServiceLog) << "network error:" << reply->errorString();
            callback(false, {});
            return;
        }

        qCDebug(TeveclubServiceLog) << "main page loaded";

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
