#include "TeachingPage.h"

TeachingPage::TeachingPage(QString content)
    : m_content{ std::move(content) }
{
}

bool TeachingPage::hasForm()
{
    return !findForm().isEmpty();
}

bool TeachingPage::hasFormWithSkillSelector()
{
    return !findFormWithSkillSelector().isEmpty();
}

QString TeachingPage::findForm()
{
    if (!m_teachingForm.isEmpty())
        return m_teachingForm;

    const auto pattern =
        QRegularExpression::escape(R"(<form action="" method="post" name="tanitb">)") +
        "(.*)" +
        QRegularExpression::escape("</form>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto match = re.match(m_content);

    if (!match.hasMatch())
    {
        return{};
    }

    m_teachingForm = match.captured(1);

    return m_teachingForm;
}

QString TeachingPage::findFormWithSkillSelector()
{
    if (!m_teachingFormWithSelector.isEmpty())
        return m_teachingFormWithSelector;

    if (!hasForm())
        return{};

    auto&& pattern =
        QRegularExpression::escape(R"(<select name="tudomany">)") +
        "(.*)" +
        QRegularExpression::escape("</select>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(findForm());

    if (!match.hasMatch())
    {
        return {};
    }

    m_teachingFormWithSelector = match.captured(1);

    return m_teachingFormWithSelector;
}

std::pair<QString, QString> TeachingPage::selectRandomSkill()
{
    if (findFormWithSkillSelector().isEmpty())
        return{};

    auto&& pattern =
        QRegularExpression::escape(R"(<option value=")") +
        R"((\d+))" +
        QRegularExpression::escape(R"(">)") +
        "(.*)" +
        QRegularExpression::escape("</option>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& optionsMatch = re.globalMatch(m_teachingFormWithSelector);

    std::vector<std::pair<QString, QString>> skills;

    while (optionsMatch.hasNext())
    {
        auto&& option = optionsMatch.next();
        skills.emplace_back(option.captured(1), option.captured(2));
    }

    if (skills.empty())
        return{};

    auto&& skill = skills[qrand() % skills.size()];

    return std::make_pair(skill.first, skill.second);
}

QRegularExpression::PatternOptions TeachingPage::defaultPatternOptions()
{
    return QRegularExpression::PatternOptions{
        QRegularExpression::CaseInsensitiveOption,
        QRegularExpression::DotMatchesEverythingOption,
        QRegularExpression::InvertedGreedinessOption
    };
}
