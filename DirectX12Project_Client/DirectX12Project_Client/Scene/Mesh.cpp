#include "stdafx.h"
#include "Mesh.h"
#include "../Core/DeviceManager.h"
#include "../Synchronization/FrameResourceManager.h"

void Mesh::Initialize(const std::string& file_path)
{
    std::vector<VectexPositionNormalTangentBinormalTexture> vertices;
    std::vector<unsigned int> indices;

    LoadGameAsset(file_path, vertices, indices);
    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);
}

void Mesh::LoadGameAsset(const std::string& file_path, std::vector<VectexPositionNormalTangentBinormalTexture>& out_vertices, std::vector<unsigned int>& out_indices) const
{ 
    std::ifstream in(file_path, std::ios::binary);
    if (in.is_open() == false)
    {
        THROW_IF_FAILED(E_FAIL);
        return;
    }

    VectexPositionNormalTangentBinormalTexture vertex;
    ::ZeroMemory(&vertex, sizeof(VectexPositionNormalTangentBinormalTexture));

    while (in.peek() != EOF)
    {
        char tag;
        in.read(&tag, sizeof(char));

        switch (tag)
        {
        case 'p':
            in.read(reinterpret_cast<char*>(&vertex.position), sizeof(DirectX::XMFLOAT3));
            break;
        case 'n':
            in.read(reinterpret_cast<char*>(&vertex.normal), sizeof(DirectX::XMFLOAT3));
            break;
        case 't':
            in.read(reinterpret_cast<char*>(&vertex.tangent), sizeof(DirectX::XMFLOAT3));
            break;
        case 'b':
            in.read(reinterpret_cast<char*>(&vertex.binormal), sizeof(DirectX::XMFLOAT3));
            break;
        case 'u':
            in.read(reinterpret_cast<char*>(&vertex.uv), sizeof(DirectX::XMFLOAT2));
            break;
        case 'i':
        {
            unsigned int index;
            in.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
            out_vertices.push_back(vertex);
            out_indices.push_back(index);
            break;
        }
        default:
            break;
        }
    }

    in.close();
}

void Mesh::CreateVertexBuffer(const std::vector<VectexPositionNormalTangentBinormalTexture>& vertices)
{
    vertex_count_ = static_cast<unsigned int>(vertices.size());

    const unsigned int buffer_size = static_cast<unsigned int>(sizeof(VectexPositionNormalTangentBinormalTexture) * vertices.size());

    D3D12_HEAP_PROPERTIES default_heap_properties;
    ::ZeroMemory(&default_heap_properties, sizeof(D3D12_HEAP_PROPERTIES));
    default_heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    default_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    default_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    default_heap_properties.CreationNodeMask = 1;
    default_heap_properties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resource_desc;
    ::ZeroMemory(&resource_desc, sizeof(D3D12_RESOURCE_DESC));
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Alignment = 0;
    resource_desc.Width = buffer_size;
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommittedResource(&default_heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(vertex_buffer_.GetAddressOf())));
   
    D3D12_HEAP_PROPERTIES upload_heap_properties;
    ::ZeroMemory(&upload_heap_properties, sizeof(D3D12_HEAP_PROPERTIES));
    upload_heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    upload_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    upload_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    upload_heap_properties.CreationNodeMask = 1;
    upload_heap_properties.VisibleNodeMask = 1;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertex_upload_buffer;
    THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommittedResource(&upload_heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertex_upload_buffer.GetAddressOf())));
  
    void* mapped_data = nullptr;
    D3D12_RANGE read_range;
    ::ZeroMemory(&read_range, sizeof(D3D12_RANGE));
    THROW_IF_FAILED(vertex_upload_buffer->Map(0, &read_range, &mapped_data));
    ::memcpy(mapped_data, vertices.data(), buffer_size);
    vertex_upload_buffer->Unmap(0, nullptr);
     
    FrameResourceManager::GetInstance().GetCurrentFrameResourceMainThreadCommandList()->CopyBufferRegion(vertex_buffer_.Get(), 0, vertex_upload_buffer.Get(), 0, buffer_size);

    D3D12_RESOURCE_BARRIER resource_barrier;
    ::ZeroMemory(&resource_barrier, sizeof(D3D12_RESOURCE_BARRIER));
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resource_barrier.Transition.pResource = vertex_buffer_.Get();
    resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    FrameResourceManager::GetInstance().GetCurrentFrameResourceMainThreadCommandList()->ResourceBarrier(1, &resource_barrier);
    
    vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
    vertex_buffer_view_.SizeInBytes = buffer_size;
    vertex_buffer_view_.StrideInBytes = sizeof(VectexPositionNormalTangentBinormalTexture);
}

void Mesh::CreateIndexBuffer(const std::vector<unsigned int>& indices)
{
    index_count_ = static_cast<unsigned int>(indices.size());

    const unsigned int buffer_size = static_cast<unsigned int>(sizeof(unsigned int) * indices.size());

    D3D12_HEAP_PROPERTIES default_heap_properties;
    ::ZeroMemory(&default_heap_properties, sizeof(D3D12_HEAP_PROPERTIES));
    default_heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    default_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    default_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    default_heap_properties.CreationNodeMask = 1;
    default_heap_properties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resource_desc;
    ::ZeroMemory(&resource_desc, sizeof(D3D12_RESOURCE_DESC));
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Alignment = 0;
    resource_desc.Width = buffer_size;
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommittedResource(&default_heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(index_buffer_.GetAddressOf())));

    D3D12_HEAP_PROPERTIES upload_heap_properties;
    ::ZeroMemory(&upload_heap_properties, sizeof(D3D12_HEAP_PROPERTIES));
    upload_heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    upload_heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    upload_heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    upload_heap_properties.CreationNodeMask = 1;
    upload_heap_properties.VisibleNodeMask = 1;

    Microsoft::WRL::ComPtr<ID3D12Resource> index_upload_buffer;
    THROW_IF_FAILED(DeviceManager::GetInstance().GetDevice()->CreateCommittedResource(&upload_heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(index_upload_buffer.GetAddressOf())));

    void* mapped_data = nullptr;
    D3D12_RANGE read_range;
    ::ZeroMemory(&read_range, sizeof(D3D12_RANGE));
    THROW_IF_FAILED(index_upload_buffer->Map(0, &read_range, &mapped_data));
    ::memcpy(mapped_data, indices.data(), buffer_size);
    index_upload_buffer->Unmap(0, nullptr);

    FrameResourceManager::GetInstance().GetCurrentFrameResourceMainThreadCommandList()->CopyBufferRegion(index_buffer_.Get(), 0, index_upload_buffer.Get(), 0, buffer_size);

    D3D12_RESOURCE_BARRIER resource_barrier;
    ::ZeroMemory(&resource_barrier, sizeof(D3D12_RESOURCE_BARRIER));
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resource_barrier.Transition.pResource = index_buffer_.Get();
    resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
    resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    FrameResourceManager::GetInstance().GetCurrentFrameResourceMainThreadCommandList()->ResourceBarrier(1, &resource_barrier);

    index_buffer_view_.BufferLocation = index_buffer_->GetGPUVirtualAddress();
    index_buffer_view_.SizeInBytes = buffer_size;
    index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
}