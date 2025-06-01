#include "stdafx.h"
#include "PipelineStateManager.h"

PipelineStateManager& PipelineStateManager::GetInstance()
{
	static PipelineStateManager instance;
	return instance;
}

void PipelineStateManager::Initialize()
{
}
