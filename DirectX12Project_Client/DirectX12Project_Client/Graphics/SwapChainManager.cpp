#include "stdafx.h"
#include "SwapChainManager.h"
#include "../Core/DeviceManager.h"
#include "../Core/QueueManager.h"

SwapChainManager& SwapChainManager::GetInstance()
{
	static SwapChainManager instance;
	return instance;
} 

void SwapChainManager::Initialize(HWND hwnd)
{
	CreateOutput(hwnd);
	GetBestRateAndResolution();
	CreateSwapChain(hwnd);
	SetFullscreen(hwnd);
}

DXGI_MODE_DESC* SwapChainManager::GetOutputModeDesc() const
{
	return output_mode_desc_.get();
}

const unsigned int SwapChainManager::GetBackBufferCount() const
{
	return kBackBufferCount;
}

unsigned int SwapChainManager::GetCurrentBackBufferIndex() const
{
	return  swap_chain_->GetCurrentBackBufferIndex();
}

IDXGISwapChain4* SwapChainManager::GetSwapChain() const
{
	return swap_chain_.Get();
}

void SwapChainManager::Present() const
{
	DXGI_PRESENT_PARAMETERS present_parameters;
	::ZeroMemory(&present_parameters, sizeof(DXGI_PRESENT_PARAMETERS));
	THROW_IF_FAILED(SwapChainManager::GetInstance().GetSwapChain()->Present1(1, 0, &present_parameters));
}

void SwapChainManager::CreateOutput(HWND hwnd)
{
	HMONITOR current_monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFOEX monitor_info;
	::ZeroMemory(&monitor_info, sizeof(MONITORINFOEX));
	monitor_info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(current_monitor, &monitor_info);

	Microsoft::WRL::ComPtr<IDXGIOutput> output;
	for (unsigned int i = 0; DeviceManager::GetInstance().GetAdapter()->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_OUTPUT_DESC output_desc;
		output->GetDesc(&output_desc);

		if (wcscmp(monitor_info.szDevice, output_desc.DeviceName) == 0)
		{
			output_ = output;

			return;
		}
	}
}

void SwapChainManager::GetBestRateAndResolution()
{
	output_mode_desc_ = std::make_unique<DXGI_MODE_DESC>();
	::ZeroMemory(output_mode_desc_.get(), sizeof(DXGI_MODE_DESC));
	output_mode_desc_->RefreshRate.Denominator = 30;
	output_mode_desc_->RefreshRate.Numerator = 1;
	 
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (unsigned int i = 0; DeviceManager::GetInstance().GetFactory()->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i)
	{ 
		Microsoft::WRL::ComPtr<IDXGIOutput> output;
		for (unsigned int j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j)
		{
			DXGI_OUTPUT_DESC output_desc;
			output->GetDesc(&output_desc);
			  
			unsigned int mode_count = 0;
			output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &mode_count, nullptr);

			std::vector<DXGI_MODE_DESC> mode_list(mode_count);
			output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &mode_count, mode_list.data());
			 
			for (const DXGI_MODE_DESC& mode : mode_list)
			{
				float current_rate = static_cast<float>(mode.RefreshRate.Numerator) / static_cast<float>(mode.RefreshRate.Denominator);
				float best_rate = static_cast<float>(output_mode_desc_->RefreshRate.Numerator) / static_cast<float>(output_mode_desc_->RefreshRate.Denominator);
				 
				if (best_rate <= current_rate && output_mode_desc_->Width * output_mode_desc_->Height <= mode.Width * mode.Height)
				{
					*output_mode_desc_ = mode;
				}
			} 
		}
	} 
} 
 
void SwapChainManager::CreateSwapChain(HWND hwnd)
{ 
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
	::ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swap_chain_desc.Width = output_mode_desc_->Width;
	swap_chain_desc.Height = output_mode_desc_->Height;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = kBackBufferCount;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc;
	::ZeroMemory(&swap_chain_fullscreen_desc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	swap_chain_fullscreen_desc.RefreshRate = output_mode_desc_->RefreshRate;
	swap_chain_fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_fullscreen_desc.Scaling = DXGI_MODE_SCALING_CENTERED;
	swap_chain_fullscreen_desc.Windowed = false;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
	if (output_)
	{
		THROW_IF_FAILED(DeviceManager::GetInstance().GetFactory()->CreateSwapChainForHwnd(QueueManager::GetInstance().GetQueue(), hwnd, &swap_chain_desc, &swap_chain_fullscreen_desc, nullptr, swap_chain1.GetAddressOf()));
		THROW_IF_FAILED(swap_chain1.As(&swap_chain_));
		THROW_IF_FAILED(swap_chain_->SetFullscreenState(true, nullptr));
	}
	else
	{
		THROW_IF_FAILED(DeviceManager::GetInstance().GetFactory()->CreateSwapChainForHwnd(QueueManager::GetInstance().GetQueue(), hwnd, &swap_chain_desc, nullptr, nullptr, swap_chain1.GetAddressOf()));
		THROW_IF_FAILED(swap_chain1.As(&swap_chain_));
	}
}

void SwapChainManager::SetFullscreen(HWND hwnd)
{
	DeviceManager::GetInstance().GetFactory()->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
	SetMenu(hwnd, nullptr);
	if (output_)
	{
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, output_mode_desc_->Width, output_mode_desc_->Height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	else
	{ 
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}