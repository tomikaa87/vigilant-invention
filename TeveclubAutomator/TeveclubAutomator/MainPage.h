#pragma once

#include <QRegularExpression>
#include <QString>

class MainPage
{
public:
    MainPage(QString content);

    bool hasFeedingForm();
    QStringView findFeedingForm();
    int countEmptyFoodSlots();
    int countEmptyDrinkSlots();

private:
    const QString m_content;

    QStringView m_feedingForm;

    int countEmptyFeedingSlots(const QString& formName);

    static QRegularExpression::PatternOptions defaultPatternOptions();
    static int countSelectOptions(QStringView s);
};

