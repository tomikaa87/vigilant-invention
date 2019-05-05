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

QRegularExpression::PatternOptions TeachingPage::defaultPatternOptions()
{
    return QRegularExpression::PatternOptions{
        QRegularExpression::CaseInsensitiveOption,
        QRegularExpression::DotMatchesEverythingOption,
        QRegularExpression::InvertedGreedinessOption
    };
}
