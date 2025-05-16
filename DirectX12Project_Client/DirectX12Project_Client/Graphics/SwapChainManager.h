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
    DXGI_MODE_DESC GetOutputModeDesc();
    const unsigned int GetBackBufferCount();
    unsigned int GetCurrentBackBufferIndex();
    IDXGISwapChain4* GetSwapChain();

private:
    void CreateOutput(HWND hwnd);
    void ChangeExclusiveFullscreen(HWND hwnd);
    void CreateSwapChain(HWND hwnd);

private:
    DXGI_MODE_DESC output_mode_desc_;
    Microsoft::WRL::ComPtr<IDXGIOutput> output_;
    
    const unsigned int kBackBufferCount = 2;
   // unsigned int current_back_buffer_index_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swap_chain_;
};

