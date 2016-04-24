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

void CreateGround(unsigned int* inBuffer, ID3D11Device** device, ID3D11Buffer **groundVBuffer, 
	ID3D11Buffer **groundIBuffer, ID3D11ShaderResourceView **groundSRView)
{
	OBJ_TO_VRAM *Verts = new OBJ_TO_VRAM[4];

	Verts[0].xyz = float3(-5, 0, 5);
	Verts[0].uvs = float2(0, 1);
	Verts[0].normals = float3(0, 1, 0);
	Verts[1].xyz = float3(5, 0, 5);
	Verts[1].uvs = float2(1, 1);
	Verts[1].normals = float3(0, 1, 0);
	Verts[2].xyz = float3(-5, 0, -5);
	Verts[2].uvs = float2(1, 0);
	Verts[2].normals = float3(0, 1, 0);
	Verts[3].xyz = float3(5, 0, -5);
	Verts[3].uvs = float2(0, 0);
	Verts[3].normals = float3(0, 1, 0);

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

	CreateDDSTextureFromFile((*device), L"GXIIfloor.dds", NULL, groundSRView);

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
	rasD.CullMode = D3D11_CULL_FRONT;
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

//void CameraMovement(SCENE_TO_VRAM &camera, SCENE_TO_VRAM &cameraTwo, XTime &time,
//	double &xMove, double &yMove, double &zMove,
//	double &xMoveTwo, double &yMoveTwo, double &zMoveTwo, bool &camOne)
//{
//	double moveX, moveY, moveZ;
//
//	if (GetAsyncKeyState('X'))
//	{
//		if (camOne)
//			camOne = false;
//		else if (!camOne)
//			camOne = true;
//	}
//
//	if (camOne)
//	{
//		moveX = xMove;
//		moveY = yMove;
//		moveZ = zMove;
//	}
//	else
//	{
//		moveX = xMoveTwo;
//		moveY = yMoveTwo;
//		moveZ = zMoveTwo;
//	}
//
//	if (GetAsyncKeyState('W'))
//		moveZ += 2 * time.Delta();
//	else if (GetAsyncKeyState('S'))
//		moveZ -= 2 * time.Delta();
//	if (GetAsyncKeyState('Q'))
//		moveY -= 2 * time.Delta();
//	else if (GetAsyncKeyState('E'))
//		moveY += 2 * time.Delta();
//	if (GetAsyncKeyState('A'))
//		moveX -= 2 * time.Delta();
//	else if (GetAsyncKeyState('D'))
//		moveX += 2 * time.Delta();
//
//	POINT mouse;
//	LPPOINT LPM = &mouse;
//	GetCursorPos(LPM);
//	float camRotSpd = .005f;
//
//	XMMATRIX cam = XMMatrixIdentity();
//	XMMATRIX trans;
//
//	cam = cam * XMMatrixRotationY(mouse.x * camRotSpd);
//	cam = XMMatrixRotationX(-mouse.y * camRotSpd) * cam;
//
//	trans = XMMatrixTranslation((float)moveX, (float)moveY, (float)moveZ);
//
//	cam.r[3] = trans.r[3];
//
//	if (camOne)
//		camera.viewMatrix = XMMatrixInverse(nullptr, cam);
//	else
//		cameraTwo.viewMatrix = XMMatrixInverse(nullptr, cam);
//}

void CreateCube(unsigned int* inBuffer, ID3D11Device** device, ID3D11Buffer** cubeVBuffer,
	ID3D11Buffer** iBuffer, ID3D11ShaderResourceView **boxSRView)
{
	OBJ_TO_VRAM *tri = new OBJ_TO_VRAM[24];

	//front top right
	tri[0].xyz.x = -.25f;
	tri[0].xyz.y = 0.25;
	tri[0].xyz.z = .25;
	tri[0].uvs.x = 1;
	tri[0].uvs.y = 0;

	//front bottom right
	tri[1].xyz.x = -0.25f;
	tri[1].xyz.y = -0.25;
	tri[1].xyz.z = .25;
	tri[1].uvs.x = 1;
	tri[1].uvs.y = 1;

	//front bottom left
	tri[2].xyz.x = .25f;
	tri[2].xyz.y = -0.25;
	tri[2].xyz.z = .25;
	tri[2].uvs.x = 0;
	tri[2].uvs.y = 1;

	//front top left
	tri[3].xyz.x = .25f;
	tri[3].xyz.y = 0.25;
	tri[3].xyz.z = .25;
	tri[3].uvs.x = 0;
	tri[3].uvs.y = 0;

	//left top right
	tri[4].xyz.x = .25f;
	tri[4].xyz.y = 0.25;
	tri[4].xyz.z = .25;
	tri[4].uvs.x = 1;
	tri[4].uvs.y = 0;

	//left bottom right
	tri[5].xyz.x = .25f;
	tri[5].xyz.y = -0.25;
	tri[5].xyz.z = .25;
	tri[5].uvs.x = 1;
	tri[5].uvs.y = 1;

	//left bottom left
	tri[6].xyz.x = .25f;
	tri[6].xyz.y = -0.25;
	tri[6].xyz.z = -.25;
	tri[6].uvs.x = 0;
	tri[6].uvs.y = 1;

	//left top left
	tri[7].xyz.x = .25f;
	tri[7].xyz.y = 0.25;
	tri[7].xyz.z = -.25;
	tri[7].uvs.x = 0;
	tri[7].uvs.y = 0;

	//right top right
	tri[8].xyz.x = -.25f;
	tri[8].xyz.y = 0.25;
	tri[8].xyz.z = .25;
	tri[8].uvs.x = 0;
	tri[8].uvs.y = 0;

	//right bottom right
	tri[9].xyz.x = -0.25f;
	tri[9].xyz.y = -0.25;
	tri[9].xyz.z = .25;
	tri[9].uvs.x = 1;
	tri[9].uvs.y = 0;

	//right bottom left
	tri[10].xyz.x = -0.25f;
	tri[10].xyz.y = -0.25;
	tri[10].xyz.z = -.25;
	tri[10].uvs.x = 1;
	tri[10].uvs.y = 1;

	//right top left
	tri[11].xyz.x = -0.25;
	tri[11].xyz.y = 0.25;
	tri[11].xyz.z = -.25;
	tri[11].uvs.x = 0;
	tri[11].uvs.y = 1;

	//back top right
	tri[12].xyz.x = -0.25;
	tri[12].xyz.y = 0.25;
	tri[12].xyz.z = -.25;
	tri[12].uvs.x = 0;
	tri[12].uvs.y = 0;

	//back bottom right
	tri[13].xyz.x = -0.25;
	tri[13].xyz.y = -0.25;
	tri[13].xyz.z = -.25;
	tri[13].uvs.x = 0;
	tri[13].uvs.y = 1;

	//back bottom left
	tri[14].xyz.x = 0.25;
	tri[14].xyz.y = -0.25;
	tri[14].xyz.z = -.25;
	tri[14].uvs.x = 1;
	tri[14].uvs.y = 1;

	//back top left
	tri[15].xyz.x = 0.25;
	tri[15].xyz.y = 0.25;
	tri[15].xyz.z = -.25;
	tri[15].uvs.x = 1;
	tri[15].uvs.y = 0;

	//top top right
	tri[16].xyz.x = -0.25;
	tri[16].xyz.y = 0.25;
	tri[16].xyz.z = -.25;
	tri[16].uvs.x = 1;
	tri[16].uvs.y = 1;

	//top bottom right
	tri[17].xyz.x = 0.25;
	tri[17].xyz.y = 0.25;
	tri[17].xyz.z = -.25;
	tri[17].uvs.x = 1;
	tri[17].uvs.y = 0;

	//top bottom left
	tri[18].xyz.x = -0.25;
	tri[18].xyz.y = 0.25;
	tri[18].xyz.z = .25;
	tri[18].uvs.x = 0;
	tri[18].uvs.y = 1;

	//top top left
	tri[19].xyz.x = 0.25;
	tri[19].xyz.y = 0.25;
	tri[19].xyz.z = .25;
	tri[19].uvs.x = 0;
	tri[19].uvs.y = 0;

	//bottom top right
	tri[20].xyz.x = -0.25;
	tri[20].xyz.y = -0.25;
	tri[20].xyz.z = .25;
	tri[20].uvs.x = 0;
	tri[20].uvs.y = 0;

	//bottom bottom right
	tri[21].xyz.x = 0.25;
	tri[21].xyz.y = -0.25;
	tri[21].xyz.z = .25;
	tri[21].uvs.x = 1;
	tri[21].uvs.y = 0;

	//bottom bottom left
	tri[22].xyz.x = -0.25;
	tri[22].xyz.y = -0.25;
	tri[22].xyz.z = -.25;
	tri[22].uvs.x = 0;
	tri[22].uvs.y = 1;

	//bottom top left
	tri[23].xyz.x = 0.25;
	tri[23].xyz.y = -0.25;
	tri[23].xyz.z = -.25;
	tri[23].uvs.x = 1;
	tri[23].uvs.y = 1;

	D3D11_BUFFER_DESC oBjVBDes;
	oBjVBDes.Usage = D3D11_USAGE_IMMUTABLE;
	oBjVBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	oBjVBDes.CPUAccessFlags = NULL;
	oBjVBDes.ByteWidth = sizeof(OBJ_TO_VRAM) * 24;
	oBjVBDes.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA cubeInitData;
	cubeInitData.pSysMem = tri;
	cubeInitData.SysMemPitch = 0;
	cubeInitData.SysMemSlicePitch = 0;

	(*device)->CreateBuffer(&oBjVBDes, &cubeInitData, cubeVBuffer);

	inBuffer[0] = 3;
	inBuffer[1] = 2;
	inBuffer[2] = 1;

	inBuffer[3] = 3;
	inBuffer[4] = 1;
	inBuffer[5] = 0;

	inBuffer[6] = 7;
	inBuffer[7] = 6;
	inBuffer[8] = 5;

	inBuffer[9] = 7;
	inBuffer[10] = 5;
	inBuffer[11] = 4;

	inBuffer[12] = 8;
	inBuffer[13] = 9;
	inBuffer[14] = 10;

	inBuffer[15] = 8;
	inBuffer[16] = 10;
	inBuffer[17] = 11;

	inBuffer[18] = 12;
	inBuffer[19] = 13;
	inBuffer[20] = 14;

	inBuffer[21] = 12;
	inBuffer[22] = 14;
	inBuffer[23] = 15;

	inBuffer[24] = 16;
	inBuffer[25] = 17;
	inBuffer[26] = 19;

	inBuffer[27] = 16;
	inBuffer[28] = 19;
	inBuffer[29] = 18;

	inBuffer[30] = 20;
	inBuffer[31] = 23;
	inBuffer[32] = 22;

	inBuffer[33] = 20;
	inBuffer[34] = 21;
	inBuffer[35] = 23;

	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = inBuffer;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vRDIndex;
	vRDIndex.Usage = D3D11_USAGE_IMMUTABLE;
	vRDIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vRDIndex.CPUAccessFlags = NULL;
	vRDIndex.ByteWidth = sizeof(unsigned int) * 36;
	vRDIndex.StructureByteStride = sizeof(unsigned int);
	vRDIndex.MiscFlags = 0;

	(*device)->CreateBuffer(&vRDIndex, &IndexData, iBuffer);

	CreateDDSTextureFromFile((*device), L"SkyboxOcean.dds", NULL, boxSRView);
}