#include "stdafx.h"
#include "DeviceManager.h"

DeviceManager& DeviceManager::GetInstance()
{
	static DeviceManager instance;
	return instance;
}

void DeviceManager::Initialize()
{
    try
    {
        CreateFactory();
        CreateAdapter();
        CreateDevice(); 
    }
    catch (std::wstring message)
    {
        std::wcout << message << std::endl;
    }
}

ID3D12Device* DeviceManager::GetDevice()
{
	return device_.Get();
}

IDXGIFactory6* DeviceManager::GetFactory()
{
    return factory_.Get();
}

IDXGIAdapter4* DeviceManager::GetAdapter()
{
    return adapter_.Get();
}

void DeviceManager::CreateFactory()
{
#if defined(DEBUG) || defined(_DEBUG)
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
        D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf()));
        debug_controller->EnableDebugLayer();
    }
#endif

    THROW_IF_FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(factory_.GetAddressOf())));
}

void DeviceManager::CreateAdapter()
{
    Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;

    for (unsigned int i = 0; factory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf())) != DXGI_ERROR_NOT_FOUND; ++i)
    { 
        DXGI_ADAPTER_DESC1 adapter_desc;
        adapter->GetDesc1(&adapter_desc);

        if (!(adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) 
        {
            adapter_ = adapter; 

            break;
        }
    }

    if (adapter_ == nullptr)
    {
        THROW_IF_FAILED(DXGI_ERROR_NOT_FOUND);
    }
}

void DeviceManager::CreateDevice()
{ 
    THROW_IF_FAILED(D3D12CreateDevice(adapter_.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device_.GetAddressOf())));
}
