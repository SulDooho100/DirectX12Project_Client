#include "../Header/VertexTypes.hlsli"
#include "../Header/ConstantBuffers.hlsli"

VectexPositionNormalTangentBinormalTexture_Output main(VectexPositionNormalTangentBinormalTexture input)
{
    VectexPositionNormalTangentBinormalTexture_Output output;
    
    float4 world_position = mul(float4(input.position, 1.0f), world);
    output.world_position = world_position.xyz;
    
    output.position = mul(world_position, view);
    output.position = mul(output.position, projection);
  
    output.normal = mul(input.normal, normal_world);
    output.tangent = mul(input.tangent, normal_world);
    output.binormal = mul(input.binormal, normal_world);
    
    output.uv = input.uv;
    
    return output;
}