#pragma once

#include <QRegularExpression>
#include <QString>

class GamePage
{
public:
    explicit GamePage(QString content);

    bool hasForm();
    QString findForm();

private:
    const QString m_content;

    QString m_form;

    static QRegularExpression::PatternOptions defaultPatternOptions();
};

