#include "stdafx.h"
#include "LoadManager.h"

LoadManager& LoadManager::GetInstance()
{
	static LoadManager instance;
	return instance;
}

void LoadManager::Initialize()
{
    FbxManager* manager = FbxManager::Create();
    if (manager) {
        std::cout << "FBX SDK Initialized!" << std::endl;
    }
    else {
        std::cerr << "Failed to initialize FBX SDK!" << std::endl; 
    }
    manager->Destroy(); 
}
