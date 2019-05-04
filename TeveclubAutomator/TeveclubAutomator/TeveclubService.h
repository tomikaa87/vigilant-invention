#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>

#include "IConfiguration.h"
#include "ITeveclubService.h"

class TeveclubService final : public QObject, public ITeveclubService
{
    Q_OBJECT

public:
    TeveclubService(IConfiguration& configuration, QObject *parent = nullptr);

    void login(std::function<void(LoginResult)>&& callback) override;
    void feed(std::function<void(FeedResult)>&& callback) override;
    void teach(std::function<void(TeachResult)>&& callback) override;

private:
    IConfiguration& m_configuration;
    QNetworkAccessManager* const m_network;

    void getMainPage(std::function<void(bool succeeded, QByteArray&& content)>&& callback);

    static QNetworkRequest createRequest(const QString& path = {});
    static QNetworkRequest createPostRequest();
};
