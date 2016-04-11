#pragma once

#include "SharedDefines.h"
#include "LoadOBJ.h"


struct SIMPLE_VERTEX
{
	XMFLOAT3 xyz, rgb;
};

struct OBJECT_TO_VRAM
{
	XMMATRIX worldMatrix;
};

struct SCENE_TO_VRAM
{
	XMMATRIX viewMatrix, projectionMatrix;
};

XMMATRIX worldMatrix = XMMatrixIdentity();
XMMATRIX viewMatrix = XMMatrixIdentity();
XMMATRIX projectionMatrix;