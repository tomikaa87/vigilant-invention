#include "DiagMenu.h"

#include <algorithm>
#include <QDebug>
#include <QLoggingCategory>
#include <QTextStream>

Q_DECLARE_LOGGING_CATEGORY(DiagTerminalLog)
Q_LOGGING_CATEGORY(DiagTerminalLog, "DiagTerminal")

DiagMenu::DiagMenu(AbstractDiagMenuBackend* backend, QObject* parent)
    : QObject(parent)
    , m_backend(backend)
{
    m_path.reserve(5);

    reset();

    connect(m_backend, &AbstractDiagMenuBackend::received,
            this, &DiagMenu::onReceived);
}

uint32_t DiagMenu::addItem(DiagMenuItem&& item)
{
    m_items.emplace_back(m_nextId, std::move(item), 0);
    return m_nextId++;
}

uint32_t DiagMenu::addSubItem(uint32_t parentId, DiagMenuItem&& item)
{
    m_items.emplace_back(m_nextId, std::move(item), parentId);
    return m_nextId++;
}

void DiagMenu::setHeader(const QString& text)
{
    m_header = text;
}

void DiagMenu::setPrompt(const QString& text)
{
    m_prompt = text;
}

void DiagMenu::pause()
{
    m_paused = true;
}

void DiagMenu::resume()
{
    if (m_paused)
    {
        repaint();
        m_paused = false;
    }
}

void DiagMenu::reset()
{
    m_path.clear();
    m_path.push_back(0);

    m_paused = false;

    repaint();
}

void DiagMenu::show()
{
    repaint();
}

DiagMenu::TreeIterator DiagMenu::itemById(uint32_t id) const
{
    return std::find_if(std::cbegin(m_items), std::cend(m_items), [id](const auto& i) { return i.id == id; });
}

std::vector<DiagMenu::TreeIterator> DiagMenu::itemsOf(uint32_t parentId) const
{
    std::vector<DiagMenu::TreeIterator> its;
    its.reserve(10);

    for (auto it = std::cbegin(m_items); it != std::cend(m_items); ++it)
    {
        if (it->parentId == parentId)
            its.push_back(it);
    }

    return its;
}

void DiagMenu::onReceived(const char c)
{
    if (m_paused)
        return;

    if (c == '\r')
        return;

    if (c == '\n')
    {
        repaint();
        return;
    }

    makeSelection(c);
}

void DiagMenu::makeSelection(const char c)
{
    auto it = std::find_if(std::cbegin(m_items), std::cend(m_items), [c, parent{ m_path.back() }](const auto& item) {
        return item.parentId == parent && item.item.key() != 0 && item.item.key() == c;
    });

    if (it == std::cend(m_items))
    {
        printMessage(QString{ "Invalid selection: %1" }.arg(c));
        return;
    }

    switch (it->item.type())
    {
    case DiagMenuItem::Type::Action:
        it->item.trigger();
        break;

    case DiagMenuItem::Type::SubMenu:
        m_path.push_back(it->id);
        break;

    case DiagMenuItem::Type::BackNavigator:
        if (m_path.size() > 1)
            m_path.pop_back();
        break;

    default:
        break;
    }
}

void DiagMenu::printMessage(const QString& message)
{
    QByteArray data;
    QTextStream s(&data);

    s << "> " << message << "\r\n";

    s.seek(0);
    m_backend->send(data);
}

void DiagMenu::printHeader(QTextStream& s)
{
    s << m_header;

    if (m_path.size() > 1)
    {
        std::for_each(std::cbegin(m_path) + 1, std::cend(m_path), [this, &s](const uint32_t id) {
            s << " :: " << itemById(id)->item.text();
        });
    }

    s << "\r\n";
}

void DiagMenu::printPrompt(QTextStream& s)
{
    s << m_prompt;
}

void DiagMenu::repaint()
{
    QByteArray data;
    QTextStream s(&data);

    s << "\r\n";

    printHeader(s);

    for (const auto& item : itemsOf(m_path.back()))
    {
        if (item->item.type() != DiagMenuItem::Type::Separator)
            s << item->item.key() << ": ";
        s << item->item.text() << "\r\n";
    }

    printPrompt(s);

    s.seek(0);
    m_backend->send(s.readAll().toUtf8());
}
