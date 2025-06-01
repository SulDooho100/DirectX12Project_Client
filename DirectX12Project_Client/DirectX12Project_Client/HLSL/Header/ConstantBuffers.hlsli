cbuffer ConstantBuffer_SceneTransform : register(b0)
{
    float4x4 view;
    float4x4 projection;
}

cbuffer ConstantBuffer_ObjectTransform : register(b1)
{
    float4x4 world;
    float3x3 normal_world;
}