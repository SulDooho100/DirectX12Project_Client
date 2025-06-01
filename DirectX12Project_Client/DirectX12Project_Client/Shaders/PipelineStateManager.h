#pragma once

class PipelineStateManager
{
public:
    static PipelineStateManager& GetInstance();

    PipelineStateManager(const PipelineStateManager&) = delete;
    PipelineStateManager& operator=(const PipelineStateManager&) = delete;
    PipelineStateManager(PipelineStateManager&&) = delete;
    PipelineStateManager& operator=(PipelineStateManager&&) = delete;

private:
    PipelineStateManager() = default;
    ~PipelineStateManager() = default;

public:
    void Initialize(); 

private:

private:
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipeline_states_;
};