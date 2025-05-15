#pragma once

class RenderTargetManager
{
public:
    static RenderTargetManager& GetInstance();

    RenderTargetManager(const RenderTargetManager&) = delete;
    RenderTargetManager& operator=(const RenderTargetManager&) = delete;
    RenderTargetManager(RenderTargetManager&&) = delete;
    RenderTargetManager& operator=(RenderTargetManager&&) = delete;

private:
    RenderTargetManager() = default;
    ~RenderTargetManager() = default;

public:
    void Initialize();

private:
    void CreateRenderTargetViewDescriptorHeap();
    void CreateDepthStencilViewDescriptorHeap();
    void CreateRenderTargetViews();
    void CreateDepthStencilView();

    void CreateBackBufferRenderTargetViews(D3D12_CPU_DESCRIPTOR_HANDLE& cpu_descriptor_handle);

    void SetResourceByKey(std::string key, Microsoft::WRL::ComPtr<ID3D12Resource> value);
    void SetCpuDescriptorHandleByKey(std::string key, D3D12_CPU_DESCRIPTOR_HANDLE value);

private:
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> resources_;
    std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> cpu_descriptor_handles_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> render_target_view_descriptor_heap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depth_stencil_view_descriptor_heap_;
};
