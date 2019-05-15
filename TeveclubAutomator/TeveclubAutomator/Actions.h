#pragma once

#include <functional>
#include <initializer_list>
#include <map>

class Actions
{
public:
    enum class Kind
    {
        Feed,
        Teach,
        Play
    };

    Actions() = default;
    Actions(std::initializer_list<Kind> l);

    void require(Kind kind);
    void complete(Kind kind);

    bool isRequired(Kind kind) const;
    bool isEmpty() const;
    bool isAllCompleted() const;

    void setCompletionCallback(std::function<void()>&& callback);

private:
    std::map<Kind, bool> m_completed;
    std::function<void()> m_completionCallback;
};
