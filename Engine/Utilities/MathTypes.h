#pragma once
#include "CommonHeaders.h"

namespace primal::math
{
	using namespace DirectX::SimpleMath;
	constexpr f32 pi{ DirectX::XM_PI };
	constexpr f32 half_pi{ pi * 0.5f };
	constexpr f32 two_pi{ DirectX::XM_2PI };
	constexpr f32 epsilon{ 1e-5f };
#if defined(_WIN64)
	//DirectX::SimpleMath
	using DirectX::SimpleMath::Vector2;
	using DirectX::SimpleMath::Vector3;
	using DirectX::SimpleMath::Vector4;
	using DirectX::SimpleMath::Quaternion;
	//XMFLOAT
	using v2 = DirectX::XMFLOAT2;
	using v2a = DirectX::XMFLOAT2A;
	using v3 = DirectX::XMFLOAT3;
	using v3a = DirectX::XMFLOAT3A;
	using v4 = DirectX::XMFLOAT4;
	using v4a = DirectX::XMFLOAT4A;
	using u32v2 = DirectX::XMUINT2;
	using u32v3 = DirectX::XMUINT3;
	using u32v4 = DirectX::XMUINT4;
	using s32v2 = DirectX::XMINT2;
	using s32v3 = DirectX::XMINT3;
	using s32v4 = DirectX::XMINT4;
	using m3x3 = DirectX::XMFLOAT3X3; //NOTE: DirectXMath doesn't have aligned 3x3 matrices
	using m4x4 = DirectX::XMFLOAT4X4;
	using m4x4a = DirectX::XMFLOAT4X4A;
#endif
}