#pragma once

#include "AbstractDiagMenuBackend.h"

#include <thread>

class DiagMenuConsoleBackend : public AbstractDiagMenuBackend
{
public:
    DiagMenuConsoleBackend();

    void send(const QByteArray& data) override;

private:
    std::thread m_thread;

    void threadProc();
};
