#include "stdafx.h"
#include "QueueManager.h"
#include "DeviceManager.h"
#include "../Synchronization/FrameResourceManager.h"

QueueManager& QueueManager::GetInstance()
{
	static QueueManager instance;
	return instance;
}

void QueueManager::Initialize()
{
	CreateQueue();
}

ID3D12CommandQueue* QueueManager::GetQueue() const
{
	return queue_.Get();
}

void QueueManager::ExecuteCommandLists() const
{
	std::vector<ID3D12CommandList*> command_lists_to_execute;
	std::vector<ID3D12GraphicsCommandList*> current_frame_resource_all_command_lists = FrameResourceManager::GetInstance().GetCurrentFrameResourceAllCommandLists();
	for (const auto& current_frame_resource_command_list : current_frame_resource_all_command_lists)
	{
		command_lists_to_execute.push_back(current_frame_resource_command_list);
	}

	QueueManager::GetInstance().GetQueue()->ExecuteCommandLists(static_cast<unsigned int>(command_lists_to_execute.size()), command_lists_to_execute.data());
}

void QueueManager::CreateQueue()
{
	D3D12_COMMAND_QUEUE_DESC command_queue_desc;
	::ZeroMemory(&command_queue_desc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; 
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(queue_.GetAddressOf()))); 
}
