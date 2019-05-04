#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringView>

class MainPage
{
public:
    explicit MainPage(QString content);

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

