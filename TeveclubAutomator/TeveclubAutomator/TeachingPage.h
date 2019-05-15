#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringView>

class TeachingPage
{
public:
    explicit TeachingPage(QString content);

    bool hasForm();
    bool hasFormWithSkillSelector();

    QString findForm();
    QString findFormWithSkillSelector();

    std::pair<QString, QString> selectRandomSkill();

private:
    const QString m_content;

    QString m_teachingForm;
    QString m_teachingFormWithSelector;

    static QRegularExpression::PatternOptions defaultPatternOptions();
};

