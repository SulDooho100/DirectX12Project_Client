#include "stdafx.h"
#include "ThreadPoolManager.h"

ThreadPoolManager& ThreadPoolManager::GetInstance()
{
	static ThreadPoolManager instance;
	return instance;
}

void ThreadPoolManager::Initialize()
{
    CreateWorkers();
}

void ThreadPoolManager::Submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        tasks_.push(std::move(task));
    }

    condition_.notify_one();
}

void ThreadPoolManager::WorkerLoop(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [&]() {
              return !tasks_.empty() || stoken.stop_requested();
            });

            if (tasks_.empty() && stoken.stop_requested())
            {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        task();
    }
}

void ThreadPoolManager::CreateWorkers()
{ 
    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        workers_.emplace_back([this](std::stop_token stoken) {
          WorkerLoop(stoken);
        });
    }
}
