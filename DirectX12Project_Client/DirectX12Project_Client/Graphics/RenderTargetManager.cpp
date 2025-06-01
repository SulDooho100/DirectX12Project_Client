#include "stdafx.h"
#include "RenderTargetManager.h"
#include "SwapChainManager.h"
#include "../Core/DeviceManager.h"

RenderTargetManager& RenderTargetManager::GetInstance()
{
	static RenderTargetManager instance;
	return instance;
}

void RenderTargetManager::Initialize()
{
	CreateViewPort();
	CreateScissorRect();
	CreateRenderTargetViewDescriptorHeap();
	CreateDepthStencilViewDescriptorHeap();
	CreateRenderTargetViews();
	CreateDepthStencilView();
}

D3D12_VIEWPORT* RenderTargetManager::GetViewport() const
{
	return viewport_.get();
}

D3D12_RECT* RenderTargetManager::GetScissorRect() const
{
	return scissor_rect_.get();
}

ID3D12Resource* RenderTargetManager::GetResource(const std::string& key)
{
	auto iter = resources_.find(key);

	if (iter == resources_.end())
	{
		THROW_IF_FAILED(E_FAIL);
	}

	return resources_[key].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE& RenderTargetManager::GetCpuDescriptorHandle(const std::string& key)
{
	auto iter = cpu_descriptor_handles_.find(key);

	if (iter == cpu_descriptor_handles_.end())
	{
		THROW_IF_FAILED(E_FAIL);
	}

	return cpu_descriptor_handles_[key];
}

void RenderTargetManager::CreateViewPort()
{
	viewport_ = std::make_unique<D3D12_VIEWPORT>();
	viewport_->TopLeftX = 0.0f;
	viewport_->TopLeftY = 0.0f;
	viewport_->Width = static_cast<float>(SwapChainManager::GetInstance().GetOutputModeDesc()->Width);
	viewport_->Height = static_cast<float>(SwapChainManager::GetInstance().GetOutputModeDesc()->Height);
	viewport_->MinDepth = 0.0f;
	viewport_->MaxDepth = 1.0f;
}

void RenderTargetManager::CreateScissorRect()
{
	scissor_rect_ = std::make_unique<D3D12_RECT>();
	scissor_rect_->left = 0;
	scissor_rect_->top = 0;
	scissor_rect_->right = static_cast<long>(SwapChainManager::GetInstance().GetOutputModeDesc()->Width);
	scissor_rect_->bottom = static_cast<long>(SwapChainManager::GetInstance().GetOutputModeDesc()->Height);
}

void RenderTargetManager::CreateRenderTargetViewDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	::ZeroMemory(&descriptor_heap_desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptor_heap_desc.NumDescriptors = SwapChainManager::GetInstance().GetBackBufferCount();
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(render_target_view_descriptor_heap_.GetAddressOf())));
}

void RenderTargetManager::CreateDepthStencilViewDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	::ZeroMemory(&descriptor_heap_desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptor_heap_desc.NumDescriptors = 1;
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(depth_stencil_view_descriptor_heap_.GetAddressOf())));
}

void RenderTargetManager::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle = render_target_view_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();

	CreateBackBufferRenderTargetViews(cpu_descriptor_handle);
}

void RenderTargetManager::CreateBackBufferRenderTargetViews(D3D12_CPU_DESCRIPTOR_HANDLE& cpu_descriptor_handle)
{ 
	for (unsigned int i = 0; i < SwapChainManager::GetInstance().GetBackBufferCount(); ++i)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> back_buffer;
		THROW_IF_FAILED(SwapChainManager::GetInstance().GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(back_buffer.GetAddressOf())));
		DeviceManager::GetInstance().GetDevice()->CreateRenderTargetView(back_buffer.Get(), nullptr, cpu_descriptor_handle);

		std::string key = "BackBuffer" + std::to_string(i);
		SetResource(key, back_buffer);
		SetCpuDescriptorHandle(key, cpu_descriptor_handle);

		cpu_descriptor_handle.ptr += DeviceManager::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

void RenderTargetManager::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC resource_desc;
	::ZeroMemory(&resource_desc, sizeof(D3D12_RESOURCE_DESC));
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resource_desc.Alignment = 0;
	resource_desc.Width = SwapChainManager::GetInstance().GetOutputModeDesc()->Width;
	resource_desc.Height = SwapChainManager::GetInstance().GetOutputModeDesc()->Height;
	resource_desc.DepthOrArraySize = 1;
	resource_desc.MipLevels = 1;
	resource_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resource_desc.SampleDesc.Count = 1;
	resource_desc.SampleDesc.Quality = 0;
	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heap_properties;
	::ZeroMemory(&heap_properties, sizeof(D3D12_HEAP_PROPERTIES));
	heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE clear_value;
	::ZeroMemory(&clear_value, sizeof(D3D12_CLEAR_VALUE));
	clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clear_value.DepthStencil.Depth = 1.0f;
	clear_value.DepthStencil.Stencil = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> depth_stencil_buffer;
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value, IID_PPV_ARGS(depth_stencil_buffer.GetAddressOf())));
	
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle = depth_stencil_view_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
	DeviceManager::GetInstance().GetDevice()->CreateDepthStencilView(depth_stencil_buffer.Get(), nullptr, cpu_descriptor_handle);

	std::string key = "DepthStencil";
	SetResource(key, depth_stencil_buffer);
	SetCpuDescriptorHandle(key, cpu_descriptor_handle);
}

void RenderTargetManager::SetResource(const std::string& key, const Microsoft::WRL::ComPtr<ID3D12Resource> resource)
{
	auto iter = resources_.find(key);

	if (iter == resources_.end())
	{
		resources_[key] = resource;

		return;
	}

	THROW_IF_FAILED(E_FAIL);
}

void RenderTargetManager::SetCpuDescriptorHandle(const std::string& key, const D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	auto iter = cpu_descriptor_handles_.find(key);

	if (iter == cpu_descriptor_handles_.end())
	{
		cpu_descriptor_handles_[key] = handle;

		return;
	}

	THROW_IF_FAILED(E_FAIL);
}