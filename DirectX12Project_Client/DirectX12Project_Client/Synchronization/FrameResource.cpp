#include "stdafx.h"
#include "FrameResource.h"
#include "../Core/DeviceManager.h"

void FrameResource::Initialize()
{
	CreateAllAllocators();
	CreateAllCommandLists();
	CreatePassConstantBuffer();
	CreateObjectConstantBuffers();
}

unsigned long long FrameResource::GetFenceValue() const
{
    return fence_value_;
}

void FrameResource::SetFenceValue(const unsigned long long fence_value)
{
    fence_value_ = fence_value;
}

std::vector<ID3D12GraphicsCommandList*> FrameResource::GetAllCommandLists() const
{
    std::vector<ID3D12GraphicsCommandList*> all_command_lists;

    all_command_lists.push_back(main_thread_command_list_.Get());

    for (const auto& command_list : worker_thread_command_lists_)
    {
        all_command_lists.push_back(command_list.Get());
    }

    return all_command_lists;
}

ID3D12GraphicsCommandList* FrameResource::GetMainThreadCommandList() const
{
    return main_thread_command_list_.Get();
}

std::vector<ID3D12GraphicsCommandList*> FrameResource::GetAllWorkerThreadCommandLists() const
{
    std::vector<ID3D12GraphicsCommandList*> all_worker_thread_command_lists;

    for (const auto& command_list : worker_thread_command_lists_)
    {
        all_worker_thread_command_lists.push_back(command_list.Get());
    }

    return all_worker_thread_command_lists;
}

void FrameResource::ResetAllAllocators()
{
    THROW_IF_FAILED(main_thread_allocator_->Reset());

    for (const auto& allocator : worker_thread_allocators_)
    {
        THROW_IF_FAILED(allocator->Reset());
    }
}

void FrameResource::ResetAllCommandLists()
{
    THROW_IF_FAILED(main_thread_command_list_->Reset(main_thread_allocator_.Get(), nullptr)); // temp : nullptr

    for (unsigned int i = 0; i < worker_thread_command_lists_.size(); ++i)
    {
        THROW_IF_FAILED(worker_thread_command_lists_[i]->Reset(worker_thread_allocators_[i].Get(), nullptr)); // temp : nullptr
    }
}

void FrameResource::CloseAllCommandLists()
{
    THROW_IF_FAILED(main_thread_command_list_->Close());

    for (const auto& command_list : worker_thread_command_lists_)
    {
        THROW_IF_FAILED(command_list->Close());
    }
}

void FrameResource::CreateAllAllocators()
{
    THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(main_thread_allocator_.GetAddressOf())));

    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
        THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))); 

        worker_thread_allocators_.push_back(allocator);
    }
}

void FrameResource::CreateAllCommandLists()
{
    THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, main_thread_allocator_.Get(), nullptr, IID_PPV_ARGS(main_thread_command_list_.GetAddressOf())));
    THROW_IF_FAILED(main_thread_command_list_->Close());

    for (unsigned int i = 0; i < worker_thread_allocators_.size(); ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list;
		THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, worker_thread_allocators_[i].Get(), nullptr, IID_PPV_ARGS(command_list.GetAddressOf())));
        THROW_IF_FAILED(command_list->Close());

        worker_thread_command_lists_.push_back(command_list);
    }
}

void FrameResource::CreatePassConstantBuffer()
{
}

void FrameResource::CreateObjectConstantBuffers()
{
    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
       //UINT buffer_size = object_count_per_thread * sizeof(ObjectConstants);
       //buffer_size = (buffer_size + 255) & ~255;  // 256 alignment
       //
       //CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
       //CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
       //
       //THROW_IF_FAILED(device->CreateCommittedResource(
       //    &heapProps,
       //    D3D12_HEAP_FLAG_NONE,
       //    &bufferDesc,
       //    D3D12_RESOURCE_STATE_GENERIC_READ,
       //    nullptr,
       //    IID_PPV_ARGS(&object_constant_buffers_[i])
       //));
    }
}
