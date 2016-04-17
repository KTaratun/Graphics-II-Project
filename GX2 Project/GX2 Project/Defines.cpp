#include "Defines.h"

void CreateStar(unsigned int* inBuffer, ID3D11Device** device, ID3D11Buffer** starVBuffer,
	ID3D11Buffer** iBuffer)
{
	int numIn = 60;
	int objVCount = 12;
	SIMPLE_VERTEX *Geometry = new SIMPLE_VERTEX[objVCount];

	for (int i = 1; i < objVCount - 1; i++)
	{
		if (i % 2)
		{
			Geometry[i].xyz.x = (float)sin((i * 36)* 3.141592653589793 / 180.0) / 2;
			Geometry[i].xyz.y = (float)cos((i * 36)* 3.141592653589793 / 180.0) / 2;
		}
		else
		{
			Geometry[i].xyz.x = (float)sin((i * 36)* 3.141592653589793 / 180.0);
			Geometry[i].xyz.y = (float)cos((i * 36)* 3.141592653589793 / 180.0);
		}
		Geometry[i].xyz.z = 0;
		Geometry[i].rgb.x = 1;
		Geometry[i].rgb.y = 0;
		Geometry[i].rgb.z = 0;
	}
	Geometry[0].xyz.x = 0;
	Geometry[0].xyz.y = 0;
	Geometry[0].xyz.z = 0.3f;
	Geometry[0].rgb.x = 1;
	Geometry[0].rgb.y = 1;
	Geometry[0].rgb.z = 0;

	Geometry[11].xyz.x = 0;
	Geometry[11].xyz.y = 0;
	Geometry[11].xyz.z = -0.3f;
	Geometry[11].rgb.x = 1;
	Geometry[11].rgb.y = 1;
	Geometry[11].rgb.z = 0;

	D3D11_BUFFER_DESC oBjVBDes;
	oBjVBDes.Usage = D3D11_USAGE_IMMUTABLE;
	oBjVBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	oBjVBDes.CPUAccessFlags = NULL;
	oBjVBDes.ByteWidth = sizeof(SIMPLE_VERTEX) * objVCount;
	oBjVBDes.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA starInitData;
	starInitData.pSysMem = Geometry;
	starInitData.SysMemPitch = 0;
	starInitData.SysMemSlicePitch = 0;

	(*device)->CreateBuffer(&oBjVBDes, &starInitData, starVBuffer);

	int j = 1;
	int k = 1;
	for (int i = 0; i < 60; i++)
	{
		if (i < 30)
		{
			if (i % 3 == 0)
			{
				if (j == 10)
					inBuffer[i] = 1;
				else
					inBuffer[i] = j + 1;
			}
			else if (i % 3 == 1)
				inBuffer[i] = j;
			else
			{
				inBuffer[i] = 0;
				j++;
			}
		}
		else
		{
			if (i % 3 == 0)
				inBuffer[i] = k;
			else if (i % 3 == 1)
			{
				if (k == 10)
					inBuffer[i] = 1;
				else
					inBuffer[i] = k + 1;
			}

			else
			{
				inBuffer[i] = 11;
				k++;
			}
		}
	}

	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = inBuffer;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vRDIndex;
	vRDIndex.Usage = D3D11_USAGE_IMMUTABLE;
	vRDIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vRDIndex.CPUAccessFlags = NULL;
	vRDIndex.ByteWidth = sizeof(unsigned int) * numIn;
	vRDIndex.StructureByteStride = sizeof(unsigned int);
	vRDIndex.MiscFlags = 0;

	(*device)->CreateBuffer(&vRDIndex, &IndexData, iBuffer);
}

//void CreateGrid()
//{
//	groundVCount = 84;
//	SIMPLE_VERTEX *Lines = new SIMPLE_VERTEX[groundVCount];
//
//	//ground STUFF
//
//	int j = 0;
//	for (size_t i = 0; i < groundVCount; i++)
//	{
//		if (i < 42)
//			if (i % 2)
//				Lines[i].xyz.x = -2;
//			else
//				Lines[i].xyz.x = 2;
//		else
//			Lines[i].xyz.x = (j - 30.5) * 0.2f - .1f;
//
//		if (i < 42)
//			Lines[i].xyz.z = (j - 9.5) * 0.2f - .1f;
//		else
//			if (i % 2)
//				Lines[i].xyz.z = -2;
//			else
//				Lines[i].xyz.z = 2;
//
//		Lines[i].xyz.y = -.9;
//		Lines[i].rgb.x = 0;
//		Lines[i].rgb.y = 1;
//		Lines[i].rgb.z = 0;
//
//		if (i % 2)
//			j++;
//
//		groundVBDes.Usage = D3D11_USAGE_IMMUTABLE;
//		groundVBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		groundVBDes.CPUAccessFlags = NULL;
//		groundVBDes.ByteWidth = sizeof(SIMPLE_VERTEX) * groundVCount;
//		groundVBDes.MiscFlags = 0;
//
//		// TODO: PART 2 STEP 3c
//
//		D3D11_SUBRESOURCE_DATA groundInitData;
//
//		groundInitData.pSysMem = Lines;
//		groundInitData.SysMemPitch = 0;
//		groundInitData.SysMemSlicePitch = 0;
//
//		device->CreateBuffer(&groundVBDes, &groundInitData, &groundVBuffer);
//		delete[] Lines;
//	}
//}

void CreateGround(unsigned int* inBuffer, ID3D11Device** device, ID3D11Buffer **groundVBuffer, ID3D11Buffer **groundIBuffer)
{
	OBJ_TO_VRAM *Verts = new OBJ_TO_VRAM[4];

	Verts[0].xyz.x = -5;
	Verts[0].xyz.y = 0;
	Verts[0].xyz.z = 5;
	Verts[0].uvs.x = 0;
	Verts[0].uvs.y = 1;
	Verts[1].xyz.x = 5;
	Verts[1].xyz.y = 0;
	Verts[1].xyz.z = 5;
	Verts[1].uvs.x = 1;
	Verts[1].uvs.y = 1;
	Verts[2].xyz.x = -5;
	Verts[2].xyz.y = 0;
	Verts[2].xyz.z = -5;
	Verts[2].uvs.x = 1;
	Verts[2].uvs.y = 0;
	Verts[3].xyz.x = 5;
	Verts[3].xyz.y = 0;
	Verts[3].xyz.z = -5;
	Verts[3].uvs.x = 0;
	Verts[3].uvs.y = 0;

	CD3D11_BUFFER_DESC groundVBDes;
	groundVBDes.Usage = D3D11_USAGE_IMMUTABLE;
	groundVBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	groundVBDes.CPUAccessFlags = NULL;
	groundVBDes.ByteWidth = sizeof(OBJ_TO_VRAM) * 4;
	groundVBDes.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA groundInitData;

	groundInitData.pSysMem = Verts;
	groundInitData.SysMemPitch = 0;
	groundInitData.SysMemSlicePitch = 0;

	(*device)->CreateBuffer(&groundVBDes, &groundInitData, groundVBuffer);
	delete[] Verts;

	inBuffer[0] = 0;
	inBuffer[1] = 1;
	inBuffer[2] = 2;
	inBuffer[3] = 2;
	inBuffer[4] = 1;
	inBuffer[5] = 3;

	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = inBuffer;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vRDIndex;
	vRDIndex.Usage = D3D11_USAGE_IMMUTABLE;
	vRDIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vRDIndex.CPUAccessFlags = NULL;
	vRDIndex.ByteWidth = sizeof(unsigned int) * 6;
	vRDIndex.StructureByteStride = sizeof(unsigned int);
	vRDIndex.MiscFlags = 0;

	ID3D11ShaderResourceView *SRView;

	CreateDDSTextureFromFile((*device), L"GXIIfloor.dds", NULL, &SRView);

	(*device)->CreateBuffer(&vRDIndex, &IndexData, groundIBuffer);
}

void CreateDepthBuffer(ID3D11Device** device, ID3D11DeviceContext** dContext, ID3D11RenderTargetView** rTView,
	ID3D11DepthStencilView** stenView, ID3D11RasterizerState** rasState, ID3D11Texture2D** texTwoD,
	IDXGISwapChain** swapChain)
{
	D3D11_TEXTURE2D_DESC texDes;

	texDes.Width = BACKBUFFER_WIDTH;
	texDes.Height = BACKBUFFER_HEIGHT;

	texDes.MipLevels = 1;
	texDes.ArraySize = 1;
	texDes.Format = DXGI_FORMAT_D32_FLOAT;
	texDes.SampleDesc.Count = 1;
	texDes.SampleDesc.Quality = 0;
	texDes.Usage = D3D11_USAGE_DEFAULT;
	texDes.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDes.CPUAccessFlags = 0;
	texDes.MiscFlags = 0;

	(*device)->CreateTexture2D(&texDes, NULL, texTwoD);

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	(*device)->CreateDepthStencilState(&dsDesc, &pDSState);

	// Bind depth stencil state
	(*dContext)->OMSetDepthStencilState(pDSState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	// CREATE DEPTH STENCIL VIEW
	(*device)->CreateDepthStencilView((*texTwoD), // Depth stencil texture
		&descDSV, // Depth stencil desc
		stenView);  // [out] Depth stencil view

	// CREATE RENDER TARGET VIEW
	ID3D11Resource *p_RT;
	(*swapChain)->GetBuffer(0, __uuidof(p_RT), reinterpret_cast<void**>(&p_RT));
	(*device)->CreateRenderTargetView(p_RT, nullptr, rTView);

	p_RT->Release();

	// BIND STENCIL VIEW
	(*dContext)->OMSetRenderTargets(1,          // One rendertarget view
		rTView,      // Render target view, created earlier
		(*stenView));     // Depth stencil view for the render target

	// CREATE RASTERIZER STATE
	D3D11_RASTERIZER_DESC rasD;
	rasD.FillMode = D3D11_FILL_SOLID;
	//rasD.CullMode = D3D11_CULL_FRONT;
	rasD.AntialiasedLineEnable = true;

	(*device)->CreateRasterizerState(&rasD, rasState);
}

void CameraMovement(SCENE_TO_VRAM &camera, XTime &time, double &xMove,
double &yMove, double &zMove)
{
	if (GetAsyncKeyState('W'))
		zMove += 2 * time.Delta();
	else if (GetAsyncKeyState('S'))
		zMove -= 2 * time.Delta();
	if (GetAsyncKeyState('Q'))
		yMove -= 2 * time.Delta();
	else if (GetAsyncKeyState('E'))
		yMove += 2 * time.Delta();
	if (GetAsyncKeyState('A'))
		xMove -= 2 * time.Delta();
	else if (GetAsyncKeyState('D'))
		xMove += 2 * time.Delta();

	POINT mouse;
	LPPOINT LPM = &mouse;
	GetCursorPos(LPM);
	float camRotSpd = .005f;

	XMMATRIX cam = XMMatrixIdentity();
	XMMATRIX trans;

	cam = cam * XMMatrixRotationY(mouse.x * camRotSpd);
	cam = XMMatrixRotationX(-mouse.y * camRotSpd) * cam;

	trans = XMMatrixTranslation((float)xMove, (float)yMove, (float)zMove);

	cam.r[3] = trans.r[3];

	camera.viewMatrix = XMMatrixInverse(nullptr, cam);
}