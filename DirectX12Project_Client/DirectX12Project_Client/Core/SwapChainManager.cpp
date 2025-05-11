#include "stdafx.h"
#include "SwapChainManager.h"
#include "DeviceManager.h"
#include "CommandManager.h"

SwapChainManager& SwapChainManager::GetInstance()
{
	static SwapChainManager instance;
	return instance;
} 

void SwapChainManager::Initialize(HWND hwnd)
{
	try
	{
		CreateOutput(hwnd); 
		ChangeExclusiveFullscreen(hwnd);
		CreateSwapChain(hwnd);
	}
	catch (std::wstring message)
	{
		std::wcout << message << std::endl;
	}
}

void SwapChainManager::CreateOutput(HWND hwnd)
{
	::ZeroMemory(&output_mode_desc_, sizeof(DXGI_MODE_DESC));
	output_mode_desc_.RefreshRate.Denominator = 30;
	output_mode_desc_.RefreshRate.Numerator = 1;

	HMONITOR current_activated_monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	  
	MONITORINFOEX current_activated_monitor_info;
	::ZeroMemory(&current_activated_monitor_info, sizeof(MONITORINFOEX)); 
	current_activated_monitor_info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(current_activated_monitor, &current_activated_monitor_info);
	 
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (unsigned int i = 0; DeviceManager::GetInstance().GetFactory()->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i)
	{ 
		Microsoft::WRL::ComPtr<IDXGIOutput> output;
		for (unsigned int j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j)
		{
			DXGI_OUTPUT_DESC output_desc;
			output->GetDesc(&output_desc);
			 
			if (wcscmp(current_activated_monitor_info.szDevice, output_desc.DeviceName) == 0)
			{
				unsigned int mode_count = 0;
				output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_SCALING, &mode_count, nullptr);

				std::vector<DXGI_MODE_DESC> mode_list(mode_count);
				output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_SCALING, &mode_count, mode_list.data());
				 
				for (const DXGI_MODE_DESC& mode : mode_list)
				{
					float current_rate = static_cast<float>(mode.RefreshRate.Numerator) / static_cast<float>(mode.RefreshRate.Denominator);
					float best_rate = static_cast<float>(output_mode_desc_.RefreshRate.Numerator) / static_cast<float>(output_mode_desc_.RefreshRate.Denominator);
					 
					if (output_mode_desc_.Width * output_mode_desc_.Height < mode.Width * mode.Height && best_rate < current_rate)
					{
						output_mode_desc_ = mode; 
						DEVMODE devmode;
						::ZeroMemory(&devmode, sizeof(DEVMODE));
						devmode.dmSize = sizeof(DEVMODE);
						EnumDisplaySettings(current_activated_monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
						output_mode_desc_.Width = static_cast<UINT>(devmode.dmPelsWidth);
						output_mode_desc_.Height = static_cast<UINT>(devmode.dmPelsHeight);

						output_ = output;
					}
				}
			}
		}
	} 
}
 
void SwapChainManager::ChangeExclusiveFullscreen(HWND hwnd)
{
	DEVMODE devmode;
	::ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmPelsWidth = output_mode_desc_.Width;
	devmode.dmPelsHeight = output_mode_desc_.Height;
	devmode.dmBitsPerPel = 32;
	devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	devmode.dmDisplayFrequency = static_cast<float>(output_mode_desc_.RefreshRate.Numerator) / static_cast<float>(output_mode_desc_.RefreshRate.Denominator);

	ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
	 
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(hwnd, HWND_TOP, 0, 0, output_mode_desc_.Width, output_mode_desc_.Height, SWP_NOZORDER | SWP_FRAMECHANGED);
}
 
void SwapChainManager::CreateSwapChain(HWND hwnd)
{ 
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
	::ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swap_chain_desc.Width = output_mode_desc_.Width;
	swap_chain_desc.Height = output_mode_desc_.Height;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.Scaling = DXGI_SCALING_NONE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc;
	::ZeroMemory(&swap_chain_fullscreen_desc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	swap_chain_fullscreen_desc.RefreshRate = output_mode_desc_.RefreshRate;
	swap_chain_fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_fullscreen_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_fullscreen_desc.Windowed = false;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
	THROW_IF_FAILED(DeviceManager::GetInstance().GetFactory()->CreateSwapChainForHwnd(CommandManager::GetInstance().GetQueue(), hwnd, &swap_chain_desc, &swap_chain_fullscreen_desc, nullptr, swap_chain1.GetAddressOf()));
	THROW_IF_FAILED(swap_chain1.As(&swap_chain_));

	DeviceManager::GetInstance().GetFactory()->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	swap_chain_->SetFullscreenState(true, output_.Get());

	current_back_buffer_index_ = swap_chain_->GetCurrentBackBufferIndex();
}

