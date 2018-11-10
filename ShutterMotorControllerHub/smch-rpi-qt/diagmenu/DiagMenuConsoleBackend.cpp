#include "DiagMenuConsoleBackend.h"

#include <QDebug>
#include <thread>

DiagMenuConsoleBackend::DiagMenuConsoleBackend()
    : m_thread([this] { threadProc(); })
{}

void DiagMenuConsoleBackend::send(const QByteArray& data)
{
    fprintf(stdout, "%s", data.constData());
    fflush(stdout);
}

void DiagMenuConsoleBackend::threadProc()
{
    while (true)
    {
        char c = static_cast<char>(getchar());
        QMetaObject::invokeMethod(this, "received", Qt::QueuedConnection, Q_ARG(char, c));
    }
}
