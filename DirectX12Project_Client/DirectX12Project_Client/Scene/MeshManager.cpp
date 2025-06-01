#include "stdafx.h"
#include "MeshManager.h"

MeshManager& MeshManager::GetInstance()
{
	static MeshManager instance;
	return instance;
}

void MeshManager::Initialize()
{
	std::string folder_path = "GameAssets";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".gameasset")
		{  
			Mesh* mesh = new Mesh();
			mesh->Initialize(entry.path().string());
			std::string mesh_name = entry.path().stem().string();
			SetMesh(mesh_name, mesh);
		} 
	}
}

void MeshManager::SetMesh(const std::string& mesh_name, Mesh* mesh)
{ 
	auto iter = meshes_.find(mesh_name);
	
	if (iter == meshes_.end())
	{
		meshes_[mesh_name] = std::unique_ptr<Mesh>(mesh);
	
		return;
	}
	
	THROW_IF_FAILED(E_FAIL); 
}

Mesh* MeshManager::GetMesh(const std::string& mesh_name)
{
	auto iter = meshes_.find(mesh_name);

	if (iter == meshes_.end())
	{
		THROW_IF_FAILED(E_FAIL);
	}

	return meshes_[mesh_name].get();
}
