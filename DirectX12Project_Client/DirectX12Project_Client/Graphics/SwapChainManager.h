#pragma once

class SwapChainManager
{
public:
    static SwapChainManager& GetInstance();

    SwapChainManager(const SwapChainManager&) = delete;
    SwapChainManager& operator=(const SwapChainManager&) = delete;
    SwapChainManager(SwapChainManager&&) = delete;
    SwapChainManager& operator=(SwapChainManager&&) = delete;

private:
    SwapChainManager() = default;
    ~SwapChainManager() = default;

public:
    void Initialize(HWND hwnd);

    DXGI_MODE_DESC* GetOutputModeDesc() const;
    const unsigned int GetBackBufferCount() const;
    unsigned int GetCurrentBackBufferIndex() const;
    IDXGISwapChain4* GetSwapChain() const;

    void Present() const;

private:
    void GetBestRateAndResolution();
    void ChangeExclusiveFullscreen(HWND hwnd);
    void CreateSwapChain(HWND hwnd);

private:
    std::unique_ptr<DXGI_MODE_DESC> output_mode_desc_;

    const unsigned int kBackBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swap_chain_;
};

