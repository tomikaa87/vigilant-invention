#include "Actions.h"

#include <algorithm>
#include <iterator>

Actions::Actions(std::initializer_list<Kind> l)
{
    for (const auto kind : l)
        m_completed[kind] = false;
}

void Actions::require(const Kind kind)
{
    m_completed[kind] = false;
}

void Actions::complete(const Kind kind)
{
    m_completed[kind] = true;

    if (m_completionCallback && isAllCompleted())
        m_completionCallback();
}

bool Actions::isRequired(const Kind kind) const
{
    return m_completed.find(kind) != std::cend(m_completed);
}

bool Actions::isEmpty() const
{
    return m_completed.empty();
}

bool Actions::isAllCompleted() const
{
    return std::all_of(std::cbegin(m_completed),
                       std::cend(m_completed),
                       [](const auto p) { return p.second; });
}

void Actions::setCompletionCallback(std::function<void()>&& callback)
{
    m_completionCallback = std::move(callback);
}
