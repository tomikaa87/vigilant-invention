#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class OperationQueue
{
public:
    OperationQueue(OperationQueue&&) = default;
    OperationQueue& operator=(OperationQueue&&) = default;

    OperationQueue(const OperationQueue&) = delete;
    OperationQueue& operator=(const OperationQueue&) = delete;

    OperationQueue()
        : m_thread{ std::thread{ [this] { threadProc(); }} }
    {}

    ~OperationQueue()
    {
        abortSync();
    }

    void enqueue(std::function<void()>&& fn)
    {
        if (m_stopped || m_aborted)
            return;

        {
            std::lock_guard<std::mutex> l{ m_mutex };
            m_queue.emplace(std::move(fn));
        }

        m_cv.notify_one();
    }

    void flushAndStopSync()
    {
        m_stopped = true;
        if (m_thread.joinable())
            m_thread.join();
    }

    void abortSync()
    {
        m_aborted = true;
        if (m_thread.joinable())
            m_thread.join();
    }

private:
    struct QueueElement
    {
        QueueElement() = default;
        QueueElement(QueueElement&&) = default;
        QueueElement& operator=(QueueElement&&) = default;

        QueueElement(const QueueElement&) = delete;
        QueueElement& operator=(const QueueElement&) = delete;

        QueueElement(std::function<void()>&& fn)
            : fn{ std::move(fn) }
        {}

        std::function<void()> fn;
    };

    std::queue<QueueElement> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic_bool m_stopped = { false };
    std::atomic_bool m_aborted = { false };
    std::thread m_thread;

    void threadProc()
    {
        while (true)
        {
            QueueElement e;

            {
                std::unique_lock<std::mutex> l{ m_mutex };
                m_cv.wait(l, [this] { return !m_queue.empty() || m_stopped || m_aborted; });
                if (m_queue.empty() || m_aborted)
                    break;
                e = std::move(m_queue.front());
                m_queue.pop();
            }

            e.fn();
        }
    }
};
