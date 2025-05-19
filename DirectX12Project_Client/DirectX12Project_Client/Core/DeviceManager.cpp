#include "stdafx.h"
#include "DeviceManager.h"

DeviceManager& DeviceManager::GetInstance()
{
	static DeviceManager instance;
	return instance;
}

void DeviceManager::Initialize()
{
    CreateFactory();
    CreateAdapter();
    CreateDevice(); 
}

ID3D12Device* DeviceManager::GetDevice() const
{
	return device_.Get();
}

IDXGIFactory6* DeviceManager::GetFactory() const
{
    return factory_.Get();
}

IDXGIAdapter4* DeviceManager::GetAdapter() const
{
    return adapter_.Get();
}

void DeviceManager::CreateFactory()
{
#if defined(DEBUG) || defined(_DEBUG)
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
        THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf())));
        debug_controller->EnableDebugLayer();

        Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedDataSettings> device_removed_extended_data_settings;
        THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&device_removed_extended_data_settings)));
        device_removed_extended_data_settings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        device_removed_extended_data_settings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

        Microsoft::WRL::ComPtr<IDXGIInfoQueue> info_queue;
        THROW_IF_FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&info_queue)));
        THROW_IF_FAILED(info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true));
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

            return;
        }
    }

    if (adapter_ == nullptr)
    {
        THROW_IF_FAILED(DXGI_ERROR_NOT_FOUND);
    }
}

void DeviceManager::CreateDevice()
{ 
    THROW_IF_FAILED(D3D12CreateDevice(adapter_.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device_.GetAddressOf())));
}
