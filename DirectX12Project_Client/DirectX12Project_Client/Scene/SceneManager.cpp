#include "stdafx.h"
#include "SceneManager.h"
#include "../Core/CommandManager.h"
#include "../Graphics/SwapChainManager.h"
#include "../Graphics/RenderTargetManager.h"
#include "../Synchronization/FenceManager.h"

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
}

void SceneManager::Initialize()
{
}

void SceneManager::Draw()
{
	THROW_IF_FAILED(CommandManager::GetInstance().GetAllocator()->Reset());
	THROW_IF_FAILED(CommandManager::GetInstance().GetCommandList()->Reset(CommandManager::GetInstance().GetAllocator(), nullptr));

	D3D12_RESOURCE_BARRIER resource_barrier;
	::ZeroMemory(&resource_barrier, sizeof(D3D12_RESOURCE_BARRIER));
	resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; 
	resource_barrier.Transition.pResource = RenderTargetManager::GetInstance().GetResourceByKey("BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex()));
	resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandManager::GetInstance().GetCommandList()->ResourceBarrier(1, &resource_barrier); 

	 
	float clear_color[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	CommandManager::GetInstance().GetCommandList()->ClearRenderTargetView(RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex())), clear_color, 0, nullptr);
	CommandManager::GetInstance().GetCommandList()->ClearDepthStencilView(RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("DepthStencil"), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	CommandManager::GetInstance().GetCommandList()->RSSetViewports(1, RenderTargetManager::GetInstance().GetViewport());
	CommandManager::GetInstance().GetCommandList()->RSSetScissorRects(1, RenderTargetManager::GetInstance().GetScissorRect());

	auto& rtv = RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("BackBuffer" + std::to_string(SwapChainManager::GetInstance().GetCurrentBackBufferIndex()));
	auto& dsv = RenderTargetManager::GetInstance().GetCpuDescriptorHandleByKey("DepthStencil");
	CommandManager::GetInstance().GetCommandList()->OMSetRenderTargets(1, &rtv, TRUE, &dsv);
	 
	resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandManager::GetInstance().GetCommandList()->ResourceBarrier(1, &resource_barrier);
 
	THROW_IF_FAILED(CommandManager::GetInstance().GetCommandList()->Close());

	ID3D12CommandList* command_lists[] = { CommandManager::GetInstance().GetCommandList() };
	CommandManager::GetInstance().GetQueue()->ExecuteCommandLists(1, command_lists);

	FenceManager::GetInstance().Signal();
	FenceManager::GetInstance().WaitForGPU(); 

	DXGI_PRESENT_PARAMETERS present_parameters;
	::ZeroMemory(&present_parameters, sizeof(DXGI_PRESENT_PARAMETERS));
	present_parameters.DirtyRectsCount = 0;
	present_parameters.pDirtyRects = nullptr;
	present_parameters.pScrollRect = nullptr;
	present_parameters.pScrollOffset = nullptr;
	THROW_IF_FAILED(SwapChainManager::GetInstance().GetSwapChain()->Present1(1, 0, &present_parameters));
}
