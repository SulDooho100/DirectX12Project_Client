#pragma once
#include "FrameResource.h"

class FrameResourceManager
{
public:
    static FrameResourceManager& GetInstance();

    FrameResourceManager(const FrameResourceManager&) = delete;
    FrameResourceManager& operator=(const FrameResourceManager&) = delete;
    FrameResourceManager(FrameResourceManager&&) = delete;
    FrameResourceManager& operator=(FrameResourceManager&&) = delete;

private:
    FrameResourceManager() = default;
    ~FrameResourceManager() = default;

public:
    void Initialize(); 

    std::vector<ID3D12GraphicsCommandList*> GetCurrentFrameResourceAllCommandLists() const;
    void SetCurrentFrameResourceFenceValue(const unsigned long long fence_value);
    unsigned long long GetCurrentFrameResourceFenceValue() const;

    void ResetCurrentFrameResource();
    void ChangeBackBufferPresentToRenderTarget();
    void ClearBackBufferRenderTargetView();
    void ClearDepthStencilView();
    void SetViewport();
    void SetScissorRect();
    void SetRenderTarget();
    void ChangeBackBufferRenderTargetToPresent();
    void CloseCurrentFrameResource();
    void AdvanceFrame();

private:
    void CreateFrameResources();

private: 
    const unsigned int kFrameResourceCount = 3;
    unsigned int current_frame_index_;
    std::vector<std::unique_ptr<FrameResource>> frame_resources_;
};

