#pragma once

#include "WidgetTimeInput.h"

class ScheduleDetails
{
public:
    ScheduleDetails() = default;
    ScheduleDetails(ScheduleDetails&&) = default;
    ScheduleDetails& operator=(ScheduleDetails&&) = default;

    ScheduleDetails(const ScheduleDetails&) = delete;
    ScheduleDetails& operator=(const ScheduleDetails&) = delete;

    explicit ScheduleDetails(const TimeInputParam& p)
        : m_startHour(p.getStartHour() & 0xff)
        , m_startMinute(p.getStartMinute() & 0xff)
        , m_stopHour(p.getStopHour() & 0xff)
        , m_stopMinute(p.getStopMinute() & 0xff)
    {
        for (uint8_t i = 0; i < 7; ++i)
            m_weekdays[i] = p.isWeekdaySelected(i + 1);
    }

    uint8_t startHour() const { return m_startHour; }
    uint8_t startMinute() const { return m_startMinute; }
    uint8_t stopHour() const { return m_stopHour; }
    uint8_t stopMinute() const { return m_stopMinute; }
    bool isWeekdaySelected(uint8_t wd) const { return m_weekdays[wd & 0x07]; }

private:
    uint8_t m_startHour = 0;
    uint8_t m_startMinute = 0;
    uint8_t m_stopHour = 0;
    uint8_t m_stopMinute = 0;
    bool m_weekdays[7] = { 0 };
};
