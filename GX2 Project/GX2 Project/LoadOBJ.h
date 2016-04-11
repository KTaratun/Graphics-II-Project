#pragma once

#include <DirectXMath.h>
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
	XMFLOAT4X4 worldMatrix;
	vector<XMFLOAT3> verticies, normals;
	vector<XMFLOAT2> uvs;
	ID3D11Buffer *VBuffer, *IBuffer, *CBuffer;
	OBJ_TO_VRAM* data;
	unsigned int uniqVert = 0, indexCount = 0;
};

void CreateVBuffer(ID3D11Device* device, OBJ_STRUCT& obj)
{
	D3D11_BUFFER_DESC VBDes;

	VBDes.Usage = D3D11_USAGE_IMMUTABLE;
	VBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDes.CPUAccessFlags = NULL;
	VBDes.ByteWidth = sizeof(OBJ_TO_VRAM) * obj.uniqVert;
	VBDes.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;

	InitData.pSysMem = obj.data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&VBDes, &InitData, &obj.VBuffer);
}

void CreateIBuffer(ID3D11Device* device, OBJ_STRUCT& obj)
{
	D3D11_SUBRESOURCE_DATA IndexData;

	IndexData.pSysMem = &obj.indexCount;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vRDIndex;
	vRDIndex.Usage = D3D11_USAGE_IMMUTABLE;
	vRDIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vRDIndex.CPUAccessFlags = NULL;
	vRDIndex.ByteWidth = sizeof(unsigned int) * obj.indexCount;
	vRDIndex.StructureByteStride = sizeof(unsigned int);
	vRDIndex.MiscFlags = 0;

	device->CreateBuffer(&vRDIndex, &IndexData, &obj.IBuffer);
}

void CreateCBuffer(ID3D11Device* device, OBJ_STRUCT& obj)
{
	D3D11_BUFFER_DESC objVRD;
	objVRD.Usage = D3D11_USAGE_DYNAMIC;
	objVRD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objVRD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objVRD.ByteWidth = sizeof(obj.worldMatrix);
	objVRD.StructureByteStride = sizeof(float);
	objVRD.MiscFlags = 0;

	device->CreateBuffer(&objVRD, nullptr, &obj.CBuffer);
}

bool LoadOBJ(const char* path,
	OBJ_STRUCT& obj, ID3D11Device* device)
{
	vector<unsigned int> vertexIndicies, uvIndicies, normalIndicies;
	vector<XMFLOAT3> temp_verticies;
	vector<XMFLOAT2> temp_uvs;
	vector<XMFLOAT3> temp_normals;

	FILE* file;
	errno_t err;
	err = fopen_s(&file, path, "r");

	if (err)
	{
		printf("Impossible to open the file!\n");
		return false;
	}
	else
	{
		while (1)
		{
			char lineHeader[128];
			//char temp = NULL;
			XMFLOAT3 tempF3;
			XMFLOAT2 tempF2;
			//int count = 0, spot = 0, i = 0;
			//do
			//{
			//	file.get(temp);
			//	count++;
			//} while (temp);

			//file.seekg(0);
			//file.read(meshName, count);
			//spot += count;

			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0)
			{
				int res = fscanf_s(file, "%f %f %f\n", &tempF3.x, &tempF3.y, &tempF3.z);
				temp_verticies.push_back(tempF3);
				obj.uniqVert += 1;
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				int res = fscanf_s(file, "%f %f\n", &tempF2.x, &tempF2.y);
				temp_uvs.push_back(tempF2);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				int res = fscanf_s(file, "%f %f %f\n", &tempF3.x, &tempF3.y, &tempF3.z);
				temp_normals.push_back(tempF3);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9)
				{
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return false;
				}
				vertexIndicies.push_back(vertexIndex[0]);
				vertexIndicies.push_back(vertexIndex[1]);
				vertexIndicies.push_back(vertexIndex[2]);
				uvIndicies.push_back(uvIndex[0]);
				uvIndicies.push_back(uvIndex[1]);
				uvIndicies.push_back(uvIndex[2]);
				normalIndicies.push_back(normalIndex[0]);
				normalIndicies.push_back(normalIndex[1]);
				normalIndicies.push_back(normalIndex[2]);

				obj.indexCount += 1;
			}
		}

		/*obj.verticies.push_back(temp_verticies);
		obj.uvs.push_back(temp_uvs);
		obj.normals.push_back(temp_normals);*/

		OBJ_TO_VRAM* tempData = new OBJ_TO_VRAM[obj.uniqVert];
		obj.data = tempData;
		delete[] tempData;

		for (unsigned int i = 0; i < vertexIndicies.size(); i++)
		{
			unsigned int vertexIndex = vertexIndicies[i];
			XMFLOAT3 vertex = temp_verticies[vertexIndex - 1];
			obj.verticies.push_back(vertex);
		}
		
		for (unsigned int i = 0; i < uvIndicies.size(); i++)
		{
			unsigned int uvIndex = uvIndicies[i];
			XMFLOAT2 uv = temp_uvs[uvIndex - 1];
			obj.uvs.push_back(uv);
		}
		
		for (unsigned int i = 0; i < normalIndicies.size(); i++)
		{
			unsigned int normalIndex = normalIndicies[i];
			XMFLOAT3 normal = temp_normals[normalIndex - 1];
			obj.normals.push_back(normal);
		}
		CreateVBuffer(device, obj);
		CreateIBuffer(device, obj);
		CreateCBuffer(device, obj);
		return true;
	}
}