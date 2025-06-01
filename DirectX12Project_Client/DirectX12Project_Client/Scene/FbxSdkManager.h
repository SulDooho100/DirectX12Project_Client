#pragma once

class FbxSdkManager
{
public:
    static FbxSdkManager& GetInstance();

    FbxSdkManager(const FbxSdkManager&) = delete;
    FbxSdkManager& operator=(const FbxSdkManager&) = delete;
    FbxSdkManager(FbxSdkManager&&) = delete;
    FbxSdkManager& operator=(FbxSdkManager&&) = delete;

private:
    FbxSdkManager() = default;
    ~FbxSdkManager();

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
