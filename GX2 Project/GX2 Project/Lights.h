#pragma once

#include "LoadOBJ.h"
#include "XTime.h"

using namespace DirectX;

struct DIRECTIONAL_LIGHT
{
	XMFLOAT4 direct, color;
};

struct POINT_LIGHT
{
	XMFLOAT4 color, pos;
};

struct SPOT_LIGHT
{
	XMFLOAT4 conDirect, color, pos;
};

void CreateDirectionalLight(ID3D11Device** device);

void PointLightMovement(POINT_LIGHT &camera, XTime &time, double &xMove,
	double &yMove, double &zMove, XMMATRIX& obj);

//struct DL_TO_VRAM
//{
//	XMFLOAT4 direct, color;
//};
//
//struct DIRECTIONAL_LIGHT
//{
//	ID3D11Buffer *cBuffer;
//	DL_TO_VRAM toVram;
//};
//
//struct PL_TO_VRAM
//{
//	XMFLOAT4 color, pos;
//};
//
//struct POINT_LIGHT
//{
//	ID3D11Buffer *cBuffer;
//	PL_TO_VRAM toVram;
//};
//
//struct SL_TO_VRAM
//{
//	XMFLOAT4 conDirect, color, pos;
//};
//
//struct SPOT_LIGHT
//{
//	ID3D11Buffer *cBuffer;
//	PL_TO_VRAM toVram;
//};
//
//void CreateDirectionalLight(ID3D11Device** device);