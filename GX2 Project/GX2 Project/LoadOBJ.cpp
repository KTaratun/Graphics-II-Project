#include "LoadOBJ.h"

void CreateVBuffer(ID3D11Device* device, OBJ_STRUCT& obj)
{
	D3D11_BUFFER_DESC VBDes;
	VBDes.Usage = D3D11_USAGE_IMMUTABLE;
	VBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDes.CPUAccessFlags = NULL;
	VBDes.ByteWidth = sizeof(OBJ_TO_VRAM) * obj.indexCount;
	VBDes.StructureByteStride = sizeof(OBJ_TO_VRAM);
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

	IndexData.pSysMem = obj.indicies;
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

bool LoadOBJ(const char* path, const wchar_t* texture,
	OBJ_STRUCT* obj, ID3D11Device** device)
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
			XMFLOAT3 tempF3;
			XMFLOAT2 tempF2;

			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0)
			{
				int res = fscanf_s(file, "%f %f %f\n", &tempF3.x, &tempF3.y, &tempF3.z);
				temp_verticies.push_back(tempF3);
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
				obj->indexCount += 3;
			}
		}

		for (unsigned int i = 0; i < vertexIndicies.size(); i++)
		{
			unsigned int vertexIndex = vertexIndicies[i];
			XMFLOAT3 vertex = temp_verticies[vertexIndex - 1];
			obj->verticies.push_back(vertex);
		}

		for (unsigned int i = 0; i < normalIndicies.size(); i++)
		{
			unsigned int normalIndex = normalIndicies[i];
			XMFLOAT3 normal = temp_normals[normalIndex - 1];
			obj->normals.push_back(normal);
		}

		for (unsigned int i = 0; i < uvIndicies.size(); i++)
		{
			unsigned int uvIndex = uvIndicies[i];
			XMFLOAT2 uv = temp_uvs[uvIndex - 1];
			obj->uvs.push_back(uv);
		}

		OBJ_TO_VRAM* tempData = new OBJ_TO_VRAM[obj->indexCount];
		unsigned int* tempIndicies = new unsigned int[obj->indexCount];

		for (size_t i = 0; i < obj->indexCount; i++)
		{
			tempData[i].xyz = obj->verticies[i];
			tempIndicies[i] = i;
			tempData[i].normals = obj->normals[i];
			tempData[i].uvs = obj->uvs[i];
		}

		obj->data = tempData;
		obj->indicies = tempIndicies;
		//delete[] tempData;

		CreateVBuffer((*device), *obj);
		CreateIBuffer((*device), *obj);
								 
		XMMATRIX obWo = XMLoadFloat4x4(&obj->worldMatrix);
		obWo = XMMatrixIdentity();
		XMStoreFloat4x4(&obj->worldMatrix, obWo);

		CreateDDSTextureFromFile((*device), texture, NULL, &obj->SRView);
		return true;
	}
}

bool LoadOBJNoNormal(const char* path, const wchar_t* texture,
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
			XMFLOAT3 tempF3;
			XMFLOAT2 tempF2;

			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0)
			{
				int res = fscanf_s(file, "%f %f %f\n", &tempF3.x, &tempF3.y, &tempF3.z);
				temp_verticies.push_back(tempF3);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				int res = fscanf_s(file, "%f %f\n", &tempF2.x, &tempF2.y);
				temp_uvs.push_back(tempF2);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				unsigned int vertexIndex[2], uvIndex[2];
				int matches = fscanf_s(file, "%d/%d %d/%d %d/%d\n",
					&vertexIndex[0], &uvIndex[0],
					&vertexIndex[1], &uvIndex[1],
					&vertexIndex[2], &uvIndex[2]);
				if (matches != 6)
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
				obj.indexCount += 3;
			}
		}

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

		OBJ_TO_VRAM* tempData = new OBJ_TO_VRAM[obj.indexCount];
		unsigned int* tempIndicies = new unsigned int[obj.indexCount];

		for (size_t i = 0; i < obj.indexCount; i++)
		{
			tempData[i].xyz = obj.verticies[i];
			tempIndicies[i] = i;
			tempData[i].uvs = obj.uvs[i];
		}

		obj.data = tempData;
		obj.indicies = tempIndicies;
		//delete[] tempData;

		CreateVBuffer(device, obj);
		CreateIBuffer(device, obj);

		XMMATRIX obWo = XMLoadFloat4x4(&obj.worldMatrix);
		obWo = XMMatrixIdentity();
		XMStoreFloat4x4(&obj.worldMatrix, obWo);

		CreateDDSTextureFromFile(device, texture, NULL, &obj.SRView);
		return true;
	}
}