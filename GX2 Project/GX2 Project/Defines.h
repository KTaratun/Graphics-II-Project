#pragma once

#include "SharedDefines.h"
#include <DirectXMath.h>
#include <vector>
#include <fstream>

using namespace DirectX;
using namespace std;

struct SIMPLE_VERTEX
{
	XMFLOAT3 xyz, rgb;
};

//bool LoadOBJ(const char* path,
//	vector<XMFLOAT3> & out_verticies,
//	vector<XMFLOAT2> & out_uvs,
//	vector<XMFLOAT3> & out_normals)
//{
//	vector<unsigned int> vertexIndicies, uvIndicies, nomralIndicies;
//	vector<XMFLOAT3> temp_verticies;
//	vector<XMFLOAT2> temp_uvs;
//	vector<XMFLOAT3> temp_normals;
//
//	fstream file;
//	file.open(path, ios_base::binary | ios_base::in);
//
//	if (!file)
//	{
//		printf("Impossible to open the file!\n");
//		return false;
//	}
//	else
//	{
//		char* meshName;
//		meshName = new char[mNLen];
//		//name of the mesh
//		file.read(meshName, mNLen);
//		delete[]meshName;
//
//		while (strcmp()
//	}
//}

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