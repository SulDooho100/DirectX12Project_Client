#pragma once

class ThreadPoolManager
{
public:
    static ThreadPoolManager& GetInstance();

    ThreadPoolManager(const ThreadPoolManager&) = delete;
    ThreadPoolManager& operator=(const ThreadPoolManager&) = delete;
    ThreadPoolManager(ThreadPoolManager&&) = delete;
    ThreadPoolManager& operator=(ThreadPoolManager&&) = delete;

private:
    ThreadPoolManager() = default;
    ~ThreadPoolManager() = default;

public:
    void Initialize();
    void Submit(std::function<void()> task);

private:
    void CreateWorkers();

    void WorkerLoop(std::stop_token stoken);

private:
    std::vector<std::jthread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
};