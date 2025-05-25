#include "stdafx.h"
#include "FrameResourceManager.h"
#include "FenceManager.h"
#include "../Core/DeviceManager.h"
#include "../Graphics/RenderTargetManager.h"
#include "../Graphics/SwapChainManager.h"

FrameResourceManager& FrameResourceManager::GetInstance()
{
	static FrameResourceManager instance;
	return instance;
}

void FrameResourceManager::Initialize()
{
	CreateFrameResources();
}

std::vector<ID3D12GraphicsCommandList*> FrameResourceManager::GetCurrentFrameResourceAllCommandLists() const
{
	return frame_resources_[current_frame_index_]->GetAllCommandLists();
}

void FrameResourceManager::SetCurrentFrameResourceFenceValue(const unsigned long long fence_value)
{
	frame_resources_[current_frame_index_]->SetFenceValue(fence_value);
}

unsigned long long FrameResourceManager::GetCurrentFrameResourceFenceValue() const
{
	return frame_resources_[current_frame_index_]->GetFenceValue();
}

void FrameResourceManager::ResetCurrentFrameResource()
{
	frame_resources_[current_frame_index_]->ResetAllAllocators();
	frame_resources_[current_frame_index_]->ResetAllCommandLists();
}

void FrameResourceManager::ChangeBackBufferPresentToRenderTarget()
{
	D3D12_RESOURCE_BARRIER resource_barrier;
	::ZeroMemory(&resource_barrier, sizeof(D3D12_RESOURCE_BARRIER));
	resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	std::string key = "BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex());
	resource_barrier.Transition.pResource = RenderTargetManager::GetInstance().GetResourceByKey(key);
	resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->ResourceBarrier(1, &resource_barrier);
}

void FrameResourceManager::ClearBackBufferRenderTargetView()
{
	std::string key = "BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex());
	float clear_color[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->ClearRenderTargetView(RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey(key), clear_color, 0, nullptr);
}

void FrameResourceManager::ClearDepthStencilView()
{
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->ClearDepthStencilView(RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("DepthStencil"), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void FrameResourceManager::SetViewport()
{
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->RSSetViewports(1, RenderTargetManager::GetInstance().GetViewport());
}

void FrameResourceManager::SetScissorRect()
{
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->RSSetScissorRects(1, RenderTargetManager::GetInstance().GetScissorRect());
}

void FrameResourceManager::SetRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE& render_target_descriptor = RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex()));
	D3D12_CPU_DESCRIPTOR_HANDLE& depth_stencil_descriptor = RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("DepthStencil");
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->OMSetRenderTargets(1, &render_target_descriptor, true, &depth_stencil_descriptor);
}

void FrameResourceManager::ChangeBackBufferRenderTargetToPresent()
{
	D3D12_RESOURCE_BARRIER resource_barrier;
	::ZeroMemory(&resource_barrier, sizeof(D3D12_RESOURCE_BARRIER));
	resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	std::string key = "BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex());
	resource_barrier.Transition.pResource = RenderTargetManager::GetInstance().GetResourceByKey(key); 
	resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	frame_resources_[current_frame_index_]->GetMainThreadCommandList()->ResourceBarrier(1, &resource_barrier);
}

void FrameResourceManager::CloseCurrentFrameResource()
{
	frame_resources_[current_frame_index_]->CloseAllCommandLists();
}

void FrameResourceManager::AdvanceFrame()
{
	current_frame_index_ = (current_frame_index_ + 1) % kFrameResourceCount;
}

void FrameResourceManager::CreateFrameResources()
{
	current_frame_index_ = 0;

	for (unsigned int i = 0; i < kFrameResourceCount; ++i)
	{
		std::unique_ptr<FrameResource> frame_resource = std::make_unique<FrameResource>();
		frame_resource->Initialize();
		frame_resources_.emplace_back(std::move(frame_resource));
	}
}
