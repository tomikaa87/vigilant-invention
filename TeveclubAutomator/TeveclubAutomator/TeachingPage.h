#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringView>

class TeachingPage
{
public:
    explicit TeachingPage(QString content);

    bool hasTeachingForm() const;
    bool hasTeachingFormWithSelector();

    QStringView findTeachingFormWithSelector();

private:
    const QString m_content;

    QStringView m_teachingFormWithSelector;

    static QRegularExpression::PatternOptions defaultPatternOptions();
};

