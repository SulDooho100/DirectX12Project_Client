#include "stdafx.h"
#include "SceneManager.h"
#include "../Core/QueueManager.h"
#include "../Graphics/SwapChainManager.h"
#include "../Graphics/RenderTargetManager.h"
#include "../Synchronization/FenceManager.h"
#include "../Synchronization/FrameResourceManager.h"

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
}

void SceneManager::Initialize()
{
}

void SceneManager::Draw() const
{
	FrameResourceManager::GetInstance().AdvanceFrame();
	FenceManager::GetInstance().WaitForGPU();

	FrameResourceManager::GetInstance().ResetCurrentFrameResource();
	FrameResourceManager::GetInstance().ChangeBackBufferPresentToRenderTarget();
	FrameResourceManager::GetInstance().ClearBackBufferRenderTargetView();
	FrameResourceManager::GetInstance().ClearDepthStencilView();

	FrameResourceManager::GetInstance().SetViewport();
	FrameResourceManager::GetInstance().SetScissorRect();
	FrameResourceManager::GetInstance().SetRenderTarget();

	// Multi thread part. some set&draw..

	FrameResourceManager::GetInstance().ChangeBackBufferRenderTargetToPresent();
	FrameResourceManager::GetInstance().CloseCurrentFrameResource();

	QueueManager::GetInstance().ExecuteCommandLists();

	FenceManager::GetInstance().Signal();

	SwapChainManager::GetInstance().Present();
}
