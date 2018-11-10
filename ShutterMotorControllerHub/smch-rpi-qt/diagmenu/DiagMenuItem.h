#pragma once

#include <functional>
#include <QString>

class DiagMenuItem
{
public:
    DiagMenuItem(const DiagMenuItem&) = delete;
    DiagMenuItem(DiagMenuItem&&) = default;

    DiagMenuItem& operator=(const DiagMenuItem&) = delete;
    DiagMenuItem& operator=(DiagMenuItem&&) = default;

    enum class Type
    {
        Action,
        SubMenu,
        BackNavigator,
        Separator
    };

    using Action = std::function<void()>;

    static DiagMenuItem createAction(const char key, const QString& text, Action&& action);
    static DiagMenuItem createSubMenu(const char key, const QString& text);
    static DiagMenuItem createBackNavigator(const char key, const QString& text);
    static DiagMenuItem createSeparator(const QString& text = "---------------");

    char key() const;
    const QString& text() const;
    Type type() const;
    void trigger() const;

private:
    DiagMenuItem(const char key, const QString& text, const Type type, Action&& action = {});

    const char m_key;
    QString m_text;
    const Type m_type;
    Action m_action;
};
