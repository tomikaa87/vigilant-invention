#include "GamePage.h"

GamePage::GamePage(QString content)
    : m_content{ std::move(content) }
{
}

bool GamePage::hasForm()
{
    return !findForm().isEmpty();
}

QString GamePage::findForm()
{
    if (!m_form.isEmpty())
        return m_form;

    const auto pattern =
        QRegularExpression::escape(R"(<form name="egyszam" action="" method="POST">)") +
        "(.*)" +
        QRegularExpression::escape("</form>");

    QRegularExpression re{ pattern, defaultPatternOptions() };

    const auto match = re.match(m_content);

    if (!match.hasMatch())
        return{};

    m_form = match.captured(1);

    return m_form;
}

QRegularExpression::PatternOptions GamePage::defaultPatternOptions()
{
    return QRegularExpression::PatternOptions{
        QRegularExpression::CaseInsensitiveOption,
        QRegularExpression::DotMatchesEverythingOption,
        QRegularExpression::InvertedGreedinessOption
    };
}
