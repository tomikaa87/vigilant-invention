#include "TeachingPage.h"

TeachingPage::TeachingPage(QString content)
    : m_content{ std::move(content) }
{
}

bool TeachingPage::hasTeachingForm() const
{
    auto&& pattern =
        QRegularExpression::escape(R"(<form action="" method="post" name="tanitb">)") +
        ".*" +
        QRegularExpression::escape("</form>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    auto&& match = re.match(m_content);

    return match.hasMatch();
}

bool TeachingPage::hasTeachingFormWithSelector()
{
    return !findTeachingFormWithSelector().isNull();
}

QStringView TeachingPage::findTeachingFormWithSelector()
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
