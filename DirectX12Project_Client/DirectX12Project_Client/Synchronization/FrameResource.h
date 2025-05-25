#pragma once

class FrameResource
{
public:
    FrameResource() = default;
    FrameResource(const FrameResource&) = delete;
    FrameResource& operator=(const FrameResource&) = delete;
    FrameResource(FrameResource&&) = delete;
    FrameResource& operator=(FrameResource&&) = delete;

    ~FrameResource() = default;

public:
    void Initialize();

    unsigned long long GetFenceValue() const;
    void SetFenceValue(const unsigned long long fence_value);
    std::vector<ID3D12GraphicsCommandList*> GetAllCommandLists() const;
    ID3D12GraphicsCommandList* GetMainThreadCommandList() const;
    std::vector<ID3D12GraphicsCommandList*> GetAllWorkerThreadCommandLists() const;

    void ResetAllAllocators();
    void ResetAllCommandLists();
    void CloseAllCommandLists();

private:
    void CreateAllAllocators();
    void CreateAllCommandLists();
    void CreatePassConstantBuffer();
    void CreateObjectConstantBuffers();

private:
    unsigned long long fence_value_;

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> main_thread_allocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> main_thread_command_list_;

    std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> worker_thread_allocators_;
    std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> worker_thread_command_lists_;

    Microsoft::WRL::ComPtr<ID3D12Resource> pass_constant_buffer_;
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> object_constant_buffers_;
};