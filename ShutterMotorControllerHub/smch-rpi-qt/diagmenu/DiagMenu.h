#pragma once

#include <QObject>
#include <stack>
#include <vector>

#include "AbstractDiagMenuBackend.h"
#include "DiagMenuItem.h"

class QTextStream;

class DiagMenu : public QObject
{
    Q_OBJECT

public:
    DiagMenu(AbstractDiagMenuBackend* backend, QObject *parent = nullptr);

    uint32_t addItem(DiagMenuItem&& item);
    uint32_t addSubItem(uint32_t parentId, DiagMenuItem&& item);

    void setHeader(const QString& text);
    void setPrompt(const QString& text);

    void pause();
    void resume();

    void reset();

    void printMessage(const QString& message);

    void show();

private:
    AbstractDiagMenuBackend* m_backend = nullptr;
    uint32_t m_nextId = 1;
    QString m_header = "Diagnostic menu";
    QString m_prompt = "> ";
    bool m_paused = false;

    struct ItemTreeElement
    {
        uint32_t id;
        uint32_t parentId;
        DiagMenuItem item;

        ItemTreeElement(const uint32_t id, DiagMenuItem&& item, const uint32_t parentId)
            : id(id), parentId(parentId), item(std::move(item))
        {}

        ItemTreeElement(const ItemTreeElement&) = delete;
        ItemTreeElement(ItemTreeElement&&) = default;
        ItemTreeElement& operator=(const ItemTreeElement&) = delete;
        ItemTreeElement& operator=(ItemTreeElement&&) = default;
    };

    std::vector<ItemTreeElement> m_items;
    std::vector<uint32_t> m_path;

    using TreeIterator = decltype(m_items)::const_iterator;

    TreeIterator itemById(uint32_t id) const;
    std::vector<TreeIterator> itemsOf(uint32_t parentId) const;

    void onReceived(const char c);

    void makeSelection(const char c);
    void printHeader(QTextStream& s);
    void printPrompt(QTextStream& s);
    void repaint();
};
