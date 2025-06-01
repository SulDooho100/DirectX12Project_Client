#pragma once
#include "VertexTypes.h"

class Mesh
{
public:
    Mesh() = default;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    ~Mesh() = default;

public:
    void Initialize(const std::string& file_path);

private:
    void LoadGameAsset(const std::string& file_path, std::vector<VectexPositionNormalTangentBinormalTexture>& out_vertices, std::vector<unsigned int>& out_indices) const;
    void CreateVertexBuffer(const std::vector<VectexPositionNormalTangentBinormalTexture>& vertices);
    void CreateIndexBuffer(const std::vector<unsigned int>& indices);
     
private:
    unsigned int vertex_count_;
    unsigned int index_count_;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> index_buffer_;

    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view_;
};