#pragma once

class QueueManager
{
public:
    static QueueManager& GetInstance();

    QueueManager(const QueueManager&) = delete;
    QueueManager& operator=(const QueueManager&) = delete;
    QueueManager(QueueManager&&) = delete;
    QueueManager& operator=(QueueManager&&) = delete;

private:
    QueueManager() = default;
    ~QueueManager() = default;

public:
    void Initialize(); 

    ID3D12CommandQueue* GetQueue() const;

    void ExecuteCommandLists() const;

private:
    void CreateQueue(); 

private: 
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue_;
};

