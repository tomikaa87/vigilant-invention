#pragma once

#include <future>

class QString;

class ITeveclubService
{
public:
    virtual ~ITeveclubService() = default;

    enum class LoginResult
    {
        Ok,
        InvalidCredentials,
        NetworkError
    };

    enum class FeedResult
    {
        Ok,
        NotLoggedIn,
        NetworkError,
        AlreadyFed
    };

    enum class TeachResult
    {
        Ok,
        NotLoggedIn,
        NetworkError,
        AlreadyTaught,
        SkillSelectionFailed
    };

    enum class PlayResult
    {
        Ok,
        NotLoggedIn,
        NetworkError,
        InvalidConfiguration,
        AlreadyPlayed
    };

    virtual void login(std::function<void(LoginResult)>&& callback) = 0;
    virtual void feed(std::function<void(FeedResult)>&& callback) = 0;
    virtual void teach(std::function<void(TeachResult)>&& callback) = 0;
    virtual void play(std::function<void(PlayResult)>&& callback) = 0;
};
