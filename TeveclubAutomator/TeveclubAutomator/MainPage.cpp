#include "MainPage.h"

#include <algorithm>
#include <QRegularExpression>

MainPage::MainPage(QString content)
    : m_content{ std::move(content) }
{
}

bool MainPage::hasFeedingForm()
{
    return !findFeedingForm().isNull();
}

QStringView MainPage::findFeedingForm()
{
    if (!m_feedingForm.isNull())
        return m_feedingForm;

    auto&& pattern =
        QRegularExpression::escape(R"(<form action="" method="post" name="etet">)") +
        "(.*)" +
        QRegularExpression::escape("</form>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(m_content);

    if (!match.hasMatch())
    {
        return{};
    }

    m_feedingForm = match.capturedView(1);

    return m_feedingForm;
}

int MainPage::countEmptyFoodSlots()
{
    return countEmptyFeedingSlots("kaja");
}

int MainPage::countEmptyDrinkSlots()
{
    return countEmptyFeedingSlots("pia");
}

int MainPage::countEmptyFeedingSlots(const QString& formName)
{
    if (!hasFeedingForm())
        return{};

    auto&& form = findFeedingForm();

    auto&& pattern =
        QRegularExpression::escape(QString{ R"(<select name="%1">)" }.arg(formName)) +
        "(.*)" +
        QRegularExpression::escape("</select>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(form.toString());

    if (!match.hasMatch())
    {
        return{};
    }

    return countSelectOptions(match.capturedView(1));
}

QRegularExpression::PatternOptions MainPage::defaultPatternOptions()
{
    return QRegularExpression::PatternOptions{
        QRegularExpression::CaseInsensitiveOption,
        QRegularExpression::DotMatchesEverythingOption,
        QRegularExpression::InvertedGreedinessOption
    };
}

int MainPage::countSelectOptions(QStringView s)
{
    if (s.isNull())
        return{};

    const QString option{ "<option" };

    auto&& it = std::cbegin(s);
    auto count = 0;

    while (true)
    {
        it = std::search(it, std::cend(s), std::cbegin(option), std::cend(option));
        if (it == std::cend(s))
            break;
        ++it;
        ++count;
    }

    return count;
}

