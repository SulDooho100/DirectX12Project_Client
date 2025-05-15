#pragma once

class FenceManager
{
public:
    static FenceManager& GetInstance();

    FenceManager(const FenceManager&) = delete;
    FenceManager& operator=(const FenceManager&) = delete;
    FenceManager(FenceManager&&) = delete;
    FenceManager& operator=(FenceManager&&) = delete;

private:
    FenceManager() = default;
    ~FenceManager() = default;

public:
    void Initialize();  
    void WaitForGPU();
    void Signal();

private:
    void CreateFence();
    void CreateFenceEvent();

private:
    unsigned long long fence_value_;
    HANDLE fence_event_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
};

