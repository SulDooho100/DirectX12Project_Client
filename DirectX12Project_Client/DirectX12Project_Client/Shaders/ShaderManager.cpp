#include "stdafx.h"
#include "ShaderManager.h"

ShaderManager& ShaderManager::GetInstance()
{
	static ShaderManager instance;
	return instance;
}

void ShaderManager::Initialize()
{
	std::string base_path;

#ifdef _DEBUG
	base_path = "../x64/Debug/";
#else
	base_directory = "./Shaders/";
#endif

	std::filesystem::path folder_path = base_path;
	 
	if (!std::filesystem::exists(folder_path) || !std::filesystem::is_directory(folder_path))
	{
		THROW_IF_FAILED(E_FAIL);
		return;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		std::filesystem::path file_path = entry.path();

		if (file_path.extension() == ".cso")
		{
			LoadShader(file_path.string());
		}
	}
}

std::vector<char> ShaderManager::GetShader(const std::string& shader_name, const std::string& target)
{
	auto iter = shaders_.find(shader_name);
	if (iter == shaders_.end())
	{
		THROW_IF_FAILED(E_FAIL);
	}

	return iter->second;
}

void ShaderManager::LoadShader(const std::string& file_path)
{
	std::ifstream file(file_path, std::ios::binary);
	if (!file)
	{
		THROW_IF_FAILED(E_FAIL);
	}

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	file.read(buffer.data(), size);

	std::filesystem::path path_obj(file_path);
	std::string shader_name = path_obj.stem().string();
	SetShader(shader_name, buffer);
}

void ShaderManager::SetShader(const std::string& shader_name, const std::vector<char>& shader)
{ 
	auto iter = shaders_.find(shader_name);
	if (iter == shaders_.end())
	{
		shaders_[shader_name] = shader;

		return;
	}

	THROW_IF_FAILED(E_FAIL);
}
