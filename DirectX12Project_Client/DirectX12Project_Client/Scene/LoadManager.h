#pragma once

class LoadManager
{
public:
    static LoadManager& GetInstance();

    LoadManager(const LoadManager&) = delete;
    LoadManager& operator=(const LoadManager&) = delete;
    LoadManager(LoadManager&&) = delete;
    LoadManager& operator=(LoadManager&&) = delete;

private:
    LoadManager() = default;
    ~LoadManager();

public:
    void Initialize();
     
private:
    void CreateFbxSdkObjects();

    void SaveFbxToGameAsset(const std::string& file_path);
    void TraverseNode(const std::string& output_path, FbxNode* node);
    void ExtractMesh(const std::string& output_path, FbxMesh* mesh);

private:
    FbxManager* fbx_manager_;
    FbxIOSettings* settings_;
};
