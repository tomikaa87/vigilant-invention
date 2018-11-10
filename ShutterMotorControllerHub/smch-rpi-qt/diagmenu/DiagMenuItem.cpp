#include "DiagMenuItem.h"

DiagMenuItem::DiagMenuItem(const char shortcut, const QString& text, const Type type, Action&& action)
    : m_key(shortcut)
    , m_text(text)
    , m_type(type)
    , m_action(std::move(action))
{
}

DiagMenuItem DiagMenuItem::createAction(const char key, const QString& text, DiagMenuItem::Action&& action)
{
    return DiagMenuItem(key, text, DiagMenuItem::Type::Action, std::move(action));
}

DiagMenuItem DiagMenuItem::createSubMenu(const char key, const QString& text)
{
    return DiagMenuItem(key, text, DiagMenuItem::Type::SubMenu);
}

DiagMenuItem DiagMenuItem::createBackNavigator(const char key, const QString& text)
{
    return DiagMenuItem(key, text, DiagMenuItem::Type::BackNavigator);
}

DiagMenuItem DiagMenuItem::createSeparator(const QString& text)
{
    return DiagMenuItem(0, text, DiagMenuItem::Type::Separator);
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
