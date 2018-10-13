#pragma once

#include <QDebug>

#include "radio/Command.h"
#include "radio/Result.h"
#include "hub/Command.h"
#include "hub/DeviceIndex.h"
#include "hub/Task.h"

QDebug& operator<<(QDebug& dbg, const hub::Task& task);
QDebug& operator<<(QDebug& dbg, const hub::Command& command);
QDebug& operator<<(QDebug& dbg, const hub::DeviceIndex& index);
QDebug& operator<<(QDebug& dbg, radio::Command command);
QDebug& operator<<(QDebug& dbg, radio::Result result);

template <typename Alloc>
inline QDebug operator<<(QDebug dbg, const std::vector<hub::DeviceIndex, Alloc>& indices)
{
    dbg.nospace();

    bool first = true;
    for (auto i : indices)
    {
        if (!first)
            dbg << ", ";
        dbg << i;
        first = false;
    }

    dbg.maybeSpace();

    return dbg;
}
