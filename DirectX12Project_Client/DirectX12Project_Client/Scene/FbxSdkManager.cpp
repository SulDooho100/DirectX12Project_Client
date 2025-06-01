#include "stdafx.h"
#include "FbxSdkManager.h"

FbxSdkManager& FbxSdkManager::GetInstance()
{
	static FbxSdkManager instance;
	return instance;
}

FbxSdkManager::~FbxSdkManager()
{
    if (fbx_manager_)
    {
        fbx_manager_->Destroy();
    }
}

void FbxSdkManager::Initialize()
{
    CreateFbxSdkObjects();

    std::string folder_path = "GameAssets";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".fbx")
        {
            std::filesystem::path fbx_path = entry.path();
            std::filesystem::path game_asset_path = fbx_path;
            game_asset_path.replace_extension(".gameAsset");

            if (std::filesystem::exists(game_asset_path) == false)
            {
                std::string fbx_file_path = entry.path().string();
                SaveFbxToGameAsset(fbx_file_path);
            }
        }
    }
}

void FbxSdkManager::CreateFbxSdkObjects()
{
    fbx_manager_ = FbxManager::Create();
    settings_ = FbxIOSettings::Create(fbx_manager_, IOSROOT);
    fbx_manager_->SetIOSettings(settings_);
} 

void FbxSdkManager::SaveFbxToGameAsset(const std::string& file_path)
{
    FbxImporter* importer = FbxImporter::Create(fbx_manager_, "");
    if (importer->Initialize(file_path.c_str(), -1, fbx_manager_->GetIOSettings()) == false) 
    {
        importer->Destroy();
        THROW_IF_FAILED(E_FAIL);
    }

    FbxScene* scene = FbxScene::Create(fbx_manager_, "scene");
    if (importer->Import(scene) == false) 
    {
        importer->Destroy();
        scene->Destroy();
        THROW_IF_FAILED(E_FAIL);
    }
    importer->Destroy();

    FbxNode* root_node = scene->GetRootNode();
    if (root_node == nullptr) 
    {
        scene->Destroy();
        THROW_IF_FAILED(E_FAIL);
    }

    std::string output_path = file_path;
    output_path.replace(output_path.find(".fbx"), 4, ".gameasset");
    for (int i = 0; i < root_node->GetChildCount(); ++i) 
    {
        TraverseNode(output_path, root_node->GetChild(i));
    }
    
    scene->Destroy();
}

void FbxSdkManager::TraverseNode(const std::string& output_path, FbxNode* node)
{
    FbxMesh* mesh = node->GetMesh();

    if (mesh) 
    {
        ExtractMesh(output_path, mesh);
    }

    for (int i = 0; i < node->GetChildCount(); ++i) 
    {
        TraverseNode(output_path, node->GetChild(i));
    }
}

void FbxSdkManager::ExtractMesh(const std::string& output_path, FbxMesh* mesh)
{ 
    FbxGeometryConverter converter(mesh->GetScene()->GetFbxManager());
    converter.Triangulate(mesh, true);

    const FbxVector4* control_points = mesh->GetControlPoints();
    const FbxGeometryElementNormal* normals = mesh->GetElementNormal();
    const FbxGeometryElementTangent* tangents = mesh->GetElementTangent();
    const FbxGeometryElementBinormal* binormals = mesh->GetElementBinormal();
    const FbxGeometryElementUV* uvs = mesh->GetElementUV();

    unsigned int index_counter = 0;

    std::ofstream out(output_path, std::ios::binary);
    if (out.is_open() == false)
    {
        THROW_IF_FAILED(E_FAIL);
        return;
    } 

    for (int polygon_index = 0; polygon_index < mesh->GetPolygonCount(); ++polygon_index)
    {
        int polygon_size = mesh->GetPolygonSize(polygon_index);
        for (unsigned int vertex_index = 0; vertex_index < polygon_size; ++vertex_index)
        {
            FbxVector4 position;
            int control_point_index = mesh->GetPolygonVertex(polygon_index, vertex_index);
            position = control_points[control_point_index];
            char tag = 'p';
            out.write(&tag, sizeof(char));
            float out_position[3] = { static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2]) };
            out.write(reinterpret_cast<char*>(out_position), sizeof(out_position));
    
            unsigned int by_polygon_vertex_index = polygon_index * polygon_size + vertex_index;
    
            if (normals)
            {
                FbxVector4 normal;
                if (normals->GetReferenceMode() == FbxGeometryElement::eDirect)
                {
                    normal = normals->GetDirectArray().GetAt(by_polygon_vertex_index);
                }
                else if (normals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                {
                    normal = normals->GetDirectArray().GetAt(normals->GetIndexArray().GetAt(by_polygon_vertex_index));
                }
                tag = 'n';
                out.write(&tag, sizeof(char));
                float out_normal[3] = { static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2]) };
                out.write(reinterpret_cast<char*>(out_normal), sizeof(out_normal));
            }
    
            if (tangents)
            {
                FbxVector4 tangent;
                if (tangents->GetReferenceMode() == FbxGeometryElement::eDirect)
                {
                    tangent = tangents->GetDirectArray().GetAt(by_polygon_vertex_index);
                }
                else if (tangents->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                {
                    tangent = tangents->GetDirectArray().GetAt(tangents->GetIndexArray().GetAt(by_polygon_vertex_index));
                }
                tag = 't';
                out.write(&tag, sizeof(char));
                float out_tangent[3] = { static_cast<float>(tangent[0]), static_cast<float>(tangent[1]), static_cast<float>(tangent[2]) };
                out.write(reinterpret_cast<char*>(out_tangent), sizeof(out_tangent));
            }
    
            if (binormals)
            {
                FbxVector4 binormal;
                if (binormals->GetReferenceMode() == FbxGeometryElement::eDirect)
                {
                    binormal = binormals->GetDirectArray().GetAt(by_polygon_vertex_index);
                }
                else if (binormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                {
                    binormal = binormals->GetDirectArray().GetAt(binormals->GetIndexArray().GetAt(by_polygon_vertex_index));
                }
                tag = 'b';
                out.write(&tag, sizeof(char));
                float out_binormal[3] = { static_cast<float>(binormal[0]), static_cast<float>(binormal[1]), static_cast<float>(binormal[2]) };
                out.write(reinterpret_cast<char*>(out_binormal), sizeof(out_binormal));
            }
    
            if (uvs)
            {
                FbxVector2 uv;
                bool unmapped;
                mesh->GetPolygonVertexUV(polygon_index, vertex_index, uvs->GetName(), uv, unmapped);
    
                if (unmapped)
                {
                    uv = FbxVector2(0.0, 0.0);
                }
                tag = 'u';
                out.write(&tag, sizeof(char));
                float out_texture[2] = { static_cast<float>(uv[0]), static_cast<float>(uv[1]) };
                out.write(reinterpret_cast<char*>(out_texture), sizeof(out_texture));
            }
    
            tag = 'i';
            out.write(&tag, sizeof(char));
            out.write(reinterpret_cast<char*>(&index_counter), sizeof(unsigned int));
            ++index_counter;
        }
    }
     
    out.close();
}