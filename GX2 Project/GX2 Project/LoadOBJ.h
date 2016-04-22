#pragma once

#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#include <vector>
#include <fstream>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

using namespace DirectX;
using namespace std;

struct OBJ_TO_VRAM
{
	XMFLOAT3 xyz;
	XMFLOAT2 uvs;
	XMFLOAT3 normals;
};

struct OBJ_STRUCT
{
	ID3D11ShaderResourceView *SRView;
	XMFLOAT4X4 worldMatrix;
	vector<XMFLOAT3> verticies, normals;
	vector<XMFLOAT2> uvs;
	ID3D11Buffer *VBuffer, *IBuffer;
	OBJ_TO_VRAM* data;
	unsigned int indexCount = 0, *indicies;
};

void CreateVBuffer(ID3D11Device* device, OBJ_STRUCT& obj);
void CreateIBuffer(ID3D11Device* device, OBJ_STRUCT& obj);
bool LoadOBJ(const char* path, const wchar_t* texture,
	OBJ_STRUCT* obj, ID3D11Device** device);
bool LoadOBJNoNormal(const char* path, const wchar_t* texture,
	OBJ_STRUCT& obj, ID3D11Device* device);
