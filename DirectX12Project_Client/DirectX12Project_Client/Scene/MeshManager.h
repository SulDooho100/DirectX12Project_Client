#pragma once
#include "Mesh.h"

class MeshManager 
{
public:
    static MeshManager& GetInstance();

    MeshManager(const MeshManager&) = delete;
    MeshManager& operator=(const MeshManager&) = delete;
    MeshManager(MeshManager&&) = delete;
    MeshManager& operator=(MeshManager&&) = delete;

private:
    MeshManager() = default;
    ~MeshManager() = default;

public:
    void Initialize();

    void SetMesh(const std::string& mesh_name, Mesh* mesh);
    Mesh* GetMesh(const std::string& mesh_name);
    
private:

private:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes_;
};