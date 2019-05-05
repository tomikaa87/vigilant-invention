#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringView>

class TeachingPage
{
public:
    explicit TeachingPage(QString content);

    bool hasForm() const;
    bool hasFormWithSelector();

    QStringView findFormWithSkillSelector();

    int selectRandomSkill();

private:
    const QString m_content;

    QStringView m_teachingFormWithSelector;

    static QRegularExpression::PatternOptions defaultPatternOptions();
};

