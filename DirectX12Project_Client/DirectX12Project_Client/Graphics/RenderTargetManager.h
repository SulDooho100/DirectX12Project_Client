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
    D3D12_VIEWPORT* GetViewport() const;
    D3D12_RECT* GetScissorRect() const;
    ID3D12Resource* GetResource(const std::string& key);
    D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuDescriptorHandle(const std::string& key);

private:
    void CreateViewPort();
    void CreateScissorRect();
    void CreateRenderTargetViewDescriptorHeap();
    void CreateDepthStencilViewDescriptorHeap();
    void CreateRenderTargetViews();
    void CreateDepthStencilView();

    void CreateBackBufferRenderTargetViews(D3D12_CPU_DESCRIPTOR_HANDLE& cpu_descriptor_handle);

    void SetResource(const std::string& key, const Microsoft::WRL::ComPtr<ID3D12Resource> resource);
    void SetCpuDescriptorHandle(const std::string& key, const D3D12_CPU_DESCRIPTOR_HANDLE handle);

private:
    std::unique_ptr<D3D12_VIEWPORT> viewport_;
    std::unique_ptr<D3D12_RECT> scissor_rect_;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> resources_;
    std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> cpu_descriptor_handles_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> render_target_view_descriptor_heap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depth_stencil_view_descriptor_heap_;
};
