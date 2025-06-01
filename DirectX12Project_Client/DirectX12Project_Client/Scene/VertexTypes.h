#pragma once
#include <DirectXMath.h>

struct VectexPositionNormalTangentBinormalTexture
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMFLOAT2 uv;
};