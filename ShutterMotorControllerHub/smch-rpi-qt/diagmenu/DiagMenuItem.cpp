#include "DiagMenuItem.h"

DiagMenuItem::DiagMenuItem(const char shortcut, QString&& text, const Type type, Action&& action)
    : m_key(shortcut)
    , m_text(std::move(text))
    , m_type(type)
    , m_action(std::move(action))
{
}

DiagMenuItem DiagMenuItem::createAction(const char key, QString&& text, DiagMenuItem::Action&& action)
{
    return DiagMenuItem(key, std::move(text), DiagMenuItem::Type::Action, std::move(action));
}

DiagMenuItem DiagMenuItem::createSubMenu(const char key, QString&& text)
{
    return DiagMenuItem(key, std::move(text), DiagMenuItem::Type::SubMenu);
}

DiagMenuItem DiagMenuItem::createBackNavigator(const char key, QString&& text)
{
    return DiagMenuItem(key, std::move(text), DiagMenuItem::Type::BackNavigator);
}

DiagMenuItem DiagMenuItem::createSeparator(QString&& text)
{
    return DiagMenuItem(0, std::move(text), DiagMenuItem::Type::Separator);
}

char DiagMenuItem::key() const
{
    return m_key;
}

const QString& DiagMenuItem::text() const
{
    return m_text;
}

DiagMenuItem::Type DiagMenuItem::type() const
{
    return m_type;
}

void DiagMenuItem::trigger() const
{
    if (m_action)
        m_action();
}
