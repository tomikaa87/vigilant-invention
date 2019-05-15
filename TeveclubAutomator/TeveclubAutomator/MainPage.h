#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringView>

class MainPage
{
public:
    explicit MainPage(QString content);

    bool hasFeedingForm();
    QString findFeedingForm();
    int countEmptyFoodSlots();
    int countEmptyDrinkSlots();

private:
    const QString m_content;

    QString m_feedingForm;

    int countEmptyFeedingSlots(const QString& formName);

    static QRegularExpression::PatternOptions defaultPatternOptions();
    static int countSelectOptions(const QString& s);
};

