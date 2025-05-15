#include "stdafx.h"
#include "FenceManager.h"
#include "../Core/DeviceManager.h"
#include "../Core/CommandManager.h"

FenceManager& FenceManager::GetInstance()
{
	static FenceManager instance;
	return instance;
}

void FenceManager::Initialize()
{
	CreateFence();
	CreateFenceEvent();
}

void FenceManager::WaitForGPU()
{
	if (fence_->GetCompletedValue() < fence_value_)
	{
		THROW_IF_FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_));
		WaitForSingleObject(fence_event_, INFINITE);
	}
}

void FenceManager::Signal()
{
	++fence_value_;
	THROW_IF_FAILED(CommandManager::GetInstance().GetQueue()->Signal(fence_.Get(), fence_value_));
}

void FenceManager::CreateFence()
{
	fence_value_ = 0;

	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateFence(fence_value_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));
}

void FenceManager::CreateFenceEvent()
{
	fence_event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (fence_event_ == nullptr)
	{
		THROW_IF_FAILED(E_FAIL);
	}
}
