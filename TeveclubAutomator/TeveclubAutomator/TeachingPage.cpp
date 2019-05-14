#include "TeachingPage.h"

TeachingPage::TeachingPage(QString content)
    : m_content{ std::move(content) }
{
}

bool TeachingPage::hasForm() const
{
    auto&& pattern =
        QRegularExpression::escape(R"(<form action="" method="post" name="tanitb">)") +
        ".*" +
        QRegularExpression::escape("</form>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(m_content);

    return match.hasMatch();
}

bool TeachingPage::hasFormWithSelector()
{
    return !findFormWithSkillSelector().isNull();
}

QStringView TeachingPage::findFormWithSkillSelector()
{
    if (!m_teachingFormWithSelector.isNull())
        return m_teachingFormWithSelector;

    auto&& pattern =
        QRegularExpression::escape(R"(<form action="" method="post" name="tanitb">)") +
        "(.*)" +
        QRegularExpression::escape("</form>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(m_content);

    if (!match.hasMatch())
    {
        return{};
    }

    m_teachingFormWithSelector = match.capturedView(1);

    return m_teachingFormWithSelector;
}

std::pair<QString, QString> TeachingPage::selectRandomSkill()
{
    if (findFormWithSkillSelector().isEmpty())
        return{};

    auto pattern =
        QRegularExpression::escape(R"(<select name="tudomany">)") +
        "(.*)" +
        QRegularExpression::escape("</select>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(findFormWithSkillSelector().toString());

    if (!match.hasMatch())
    {
        return {};
    }

    const auto options = match.capturedRef(1);

    pattern =
        QRegularExpression::escape(R"(<option value=")") +
        R"((\d+))" +
        QRegularExpression::escape(R"(">)") +
        "(.*)" +
        QRegularExpression::escape("</option>");

    re.setPattern(pattern);

    auto&& optionsMatch = re.globalMatch(options);

    std::vector<std::pair<QStringView, QStringView>> skills;

    while (optionsMatch.hasNext())
    {
        auto&& option = optionsMatch.next();
        skills.emplace_back(option.capturedView(1), option.capturedView(2));
    }

    if (skills.empty())
        return{};

    auto&& skill = skills[qrand() % skills.size()];

    return std::make_pair(skill.first.toString(), skill.second.toString());
}

QRegularExpression::PatternOptions TeachingPage::defaultPatternOptions()
{
    return QRegularExpression::PatternOptions{
        QRegularExpression::CaseInsensitiveOption,
        QRegularExpression::DotMatchesEverythingOption,
        QRegularExpression::InvertedGreedinessOption
    };
}
