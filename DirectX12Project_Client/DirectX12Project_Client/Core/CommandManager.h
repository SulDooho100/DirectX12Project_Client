#pragma once

// temp : for single thread
class CommandManager
{
public:
    static CommandManager& GetInstance();

    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&&) = delete;
    CommandManager& operator=(CommandManager&&) = delete;

private:
    CommandManager() = default;
    ~CommandManager() = default;

public:
    void Initialize(); 
    ID3D12CommandQueue* GetQueue();
    ID3D12CommandAllocator* GetAllocator();
    ID3D12GraphicsCommandList* GetCommandList();

private:
    void CreateQueue();
    void CreateAllocator();
    void CreateCommandList();

private: 
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list_;
};

