#include "stdafx.h"
#include "CommandManager.h"
#include "DeviceManager.h"

CommandManager& CommandManager::GetInstance()
{
	static CommandManager instance;
	return instance;
}

void CommandManager::Initialize()
{
	CreateQueue();
	CreateAllocator();
	CreateCommandList(); 
}

ID3D12CommandQueue* CommandManager::GetQueue()
{
	return queue_.Get();
}

ID3D12CommandAllocator* CommandManager::GetAllocator()
{
	return allocator_.Get();
}

ID3D12GraphicsCommandList* CommandManager::GetCommandList()
{
	return command_list_.Get();
}

void CommandManager::CreateQueue()
{
	D3D12_COMMAND_QUEUE_DESC command_queue_desc;
	::ZeroMemory(&command_queue_desc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; 
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(queue_.GetAddressOf()))); 
}

void CommandManager::CreateAllocator()
{
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator_.GetAddressOf()))); 
}

void CommandManager::CreateCommandList()
{
	THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator_.Get(), nullptr, IID_PPV_ARGS(command_list_.GetAddressOf())));
	command_list_->Close();
}
