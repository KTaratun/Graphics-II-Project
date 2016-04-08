//includes

#include <iostream>
#include <assert.h>
#include <windows.h>
#include <ctime>
#include "Defines.h"
#include "XTime.h"
#include "teapot.h"
#include "DDSTextureLoader.h"

#include "Trivial_PSCOLOR.csh"
#include "Trivial_VSCOLOR.csh"

using namespace std;

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HRESULT							hr;
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	ID3D11Device					*device;
	ID3D11DeviceContext				*dContext;
	ID3D11RenderTargetView			*rTView;
	IDXGISwapChain					*swapChain;
	D3D11_VIEWPORT					vPort;

	ID3D11Buffer *groundVBuffer;
	D3D11_BUFFER_DESC groundVBDes;
	ID3D11Buffer *oBjVBuffer;
	ID3D11Buffer *oBj2VBuffer;
	D3D11_BUFFER_DESC oBjVBDes;
	D3D11_BUFFER_DESC oBj2VBDes;
	unsigned int groundVCount;
	unsigned int objVCount;
	unsigned int obj2VCount = 1641;
	ID3D11InputLayout *ILayout;
	ID3D11Texture2D *texTwoD;
	ID3D11DepthStencilView *stenView;
	ID3D11RasterizerState *rasStateThree;
	ID3D11RasterizerState *rasStateFour;

	ID3D11VertexShader *VShader;
	ID3D11PixelShader *PShader;

	ID3D11Buffer *groundCBuffer;
	ID3D11Buffer *objCBuffer;
	ID3D11Buffer *obj2CBuffer;
	ID3D11Buffer *cBufferView;
	ID3D11Buffer *iBuffer;
	ID3D11Buffer *iBuffer2;
	XTime time;

public:
	
	OBJECT_TO_VRAM ground;
	OBJECT_TO_VRAM obj;
	OBJECT_TO_VRAM obj2;
	SCENE_TO_VRAM camera;
	unsigned int numIn = 60;
	unsigned int inBuffer[60];
	float xMove = 0;
	float yMove = 0;
	float zMove = 0;

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);



	//MY STUFF
	unsigned int flag = 0;
#if _DEBUG
	flag = D3D11_CREATE_DEVICE_DEBUG;
	//until sdk 10 works
	//flag = 0;
#elif TRUE
	flag = 0;
#endif

	DXGI_SWAP_CHAIN_DESC sCDes;
	ZeroMemory(&sCDes, sizeof(DXGI_SWAP_CHAIN_DESC));
	sCDes.BufferCount = 1;
	sCDes.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sCDes.BufferDesc.Height = BACKBUFFER_HEIGHT;
	sCDes.BufferDesc.Width = BACKBUFFER_WIDTH;
	sCDes.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sCDes.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sCDes.OutputWindow = window;
	sCDes.SampleDesc.Count = 1;
	sCDes.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sCDes.Windowed = true;

	assert(hr == S_OK);

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//pAdapter
		D3D_DRIVER_TYPE_HARDWARE,	//driverType
		NULL,						//hModule
		flag,						//flags
		nullptr,					//featureLevel
		0,							//featureLevels
		D3D11_SDK_VERSION,			//SDKVersion
		&sCDes,
		&swapChain,					//swapChain
		&device,					//device
		nullptr,					//featureLevel??
		&dContext);					//deviceContext

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

	device->CreateTexture2D(&texDes, NULL, &texTwoD);

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
	device->CreateDepthStencilState(&dsDesc, &pDSState);

	// Bind depth stencil state
	dContext->OMSetDepthStencilState(pDSState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	// Create the depth stencil view
	device->CreateDepthStencilView(texTwoD, // Depth stencil texture
		&descDSV, // Depth stencil desc
		&stenView);  // [out] Depth stencil view


	ID3D11Resource *p_RT;

	swapChain->GetBuffer(0, __uuidof(p_RT), reinterpret_cast<void**>(&p_RT));
	device->CreateRenderTargetView(p_RT, nullptr, &rTView);

	p_RT->Release();

	// Bind the depth stencil view
	dContext->OMSetRenderTargets(1,          // One rendertarget view
		&rTView,      // Render target view, created earlier
		stenView);     // Depth stencil view for the render target

	D3D11_RASTERIZER_DESC rasDThree;

	rasDThree.FillMode = D3D11_FILL_SOLID;
	rasDThree.CullMode = D3D11_CULL_NONE;
	rasDThree.AntialiasedLineEnable = true;

	device->CreateRasterizerState(&rasDThree, &rasStateThree);

	// TODO: PART 1 STEP 5

	vPort.Height = BACKBUFFER_HEIGHT;
	vPort.MaxDepth = 1;
	vPort.MinDepth = 0;
	vPort.Width = BACKBUFFER_WIDTH;
	//swapChain->GetDesc(&sCDes.BufferDesc.Width);

	// TODO: PART 2 STEP 3a


	groundVCount = 84;
	SIMPLE_VERTEX *Lines = new SIMPLE_VERTEX[groundVCount];

	//ground STUFF

	int j = 0;
	for (size_t i = 0; i < groundVCount; i++)
	{
		if (i < 42)
			if (i % 2)
				Lines[i].xyz.x = -2;
			else
				Lines[i].xyz.x = 2;
		else
			Lines[i].xyz.x = (j - 30.5) * 0.2f - .1f;

		if (i < 42)
			Lines[i].xyz.z = (j - 9.5) * 0.2f - .1f;
		else
			if (i % 2)
				Lines[i].xyz.z = -2;
			else
				Lines[i].xyz.z = 2;

		Lines[i].xyz.y = -.9;
		Lines[i].rgb.x = 0;
		Lines[i].rgb.y = 1;
		Lines[i].rgb.z = 0;

		if (i % 2)
			j++;
	}

	//ID3D11Texture2D *tex;
	//ID3D11ShaderResourceView *texView;
	//CreateDDSTextureFromFile(device, "GXIIfloor.dds", &tex, &texView, 0);

	//STAR
	//////////////////////////////
	//////////////////////////////
	objVCount = 12;
	SIMPLE_VERTEX *Geometry = new SIMPLE_VERTEX[objVCount];

	for (size_t i = 1; i < objVCount - 1; i++)
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
	Geometry[0].xyz.z = 0.3;
	Geometry[0].rgb.x = 1;
	Geometry[0].rgb.y = 1;
	Geometry[0].rgb.z = 0;

	Geometry[11].xyz.x = 0;
	Geometry[11].xyz.y = 0;
	Geometry[11].xyz.z = -0.3;
	Geometry[11].rgb.x = 1;
	Geometry[11].rgb.y = 1;
	Geometry[11].rgb.z = 0;

	// BEGIN PART 4
	// TODO: PART 4 STEP 1

	// TODO: PART 2 STEP 3b

	groundVBDes.Usage = D3D11_USAGE_IMMUTABLE;
	groundVBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	groundVBDes.CPUAccessFlags = NULL;
	groundVBDes.ByteWidth = sizeof(SIMPLE_VERTEX) * groundVCount;
	groundVBDes.MiscFlags = 0;

	oBjVBDes.Usage = D3D11_USAGE_IMMUTABLE;
	oBjVBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	oBjVBDes.CPUAccessFlags = NULL;
	oBjVBDes.ByteWidth = sizeof(SIMPLE_VERTEX) * objVCount;
	oBjVBDes.MiscFlags = 0;

	oBj2VBDes.Usage = D3D11_USAGE_IMMUTABLE;
	oBj2VBDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	oBj2VBDes.CPUAccessFlags = NULL;
	oBj2VBDes.ByteWidth = sizeof(OBJ_VERT) * obj2VCount;
	oBj2VBDes.MiscFlags = 0;

	// TODO: PART 2 STEP 3c

	D3D11_SUBRESOURCE_DATA groundInitData;

	groundInitData.pSysMem = Lines;
	groundInitData.SysMemPitch = 0;
	groundInitData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA oBjInitData;

	oBjInitData.pSysMem = Geometry;
	oBjInitData.SysMemPitch = 0;
	oBjInitData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA oBj2InitData;

	oBj2InitData.pSysMem = teapot_data;
	oBj2InitData.SysMemPitch = 0;
	oBj2InitData.SysMemSlicePitch = 0;

	// TODO: PART 2 STEP 3d

	device->CreateBuffer(&groundVBDes, &groundInitData, &groundVBuffer);
	device->CreateBuffer(&oBjVBDes, &oBjInitData, &oBjVBuffer);
	device->CreateBuffer(&oBj2VBDes, &oBj2InitData, &oBj2VBuffer);
	delete[] Lines;

	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), nullptr, &VShader);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &PShader);

	// TODO: PART 2 STEP 8a

	D3D11_INPUT_ELEMENT_DESC vLayoutCOLOR[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// TODO: PART 2 STEP 8b

	device->CreateInputLayout(vLayoutCOLOR, 2, Trivial_VS, sizeof(Trivial_VS), &ILayout);

	// TODO: PART 3 STEP 3


	D3D11_BUFFER_DESC groundVRD;
	groundVRD.Usage = D3D11_USAGE_DYNAMIC;
	groundVRD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	groundVRD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	groundVRD.ByteWidth = sizeof(OBJECT_TO_VRAM);
	groundVRD.StructureByteStride = sizeof(float);
	groundVRD.MiscFlags = 0;

	device->CreateBuffer(&groundVRD, nullptr, &groundCBuffer);

	D3D11_BUFFER_DESC objVRD;
	objVRD.Usage = D3D11_USAGE_DYNAMIC;
	objVRD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objVRD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objVRD.ByteWidth = sizeof(OBJECT_TO_VRAM);
	objVRD.StructureByteStride = sizeof(float);
	objVRD.MiscFlags = 0;

	device->CreateBuffer(&objVRD, nullptr, &objCBuffer);

	D3D11_BUFFER_DESC obj2VRD;
	obj2VRD.Usage = D3D11_USAGE_DYNAMIC;
	obj2VRD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	obj2VRD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	obj2VRD.ByteWidth = sizeof(OBJECT_TO_VRAM);
	obj2VRD.StructureByteStride = sizeof(float);
	obj2VRD.MiscFlags = 0;

	device->CreateBuffer(&obj2VRD, nullptr, &obj2CBuffer);

	D3D11_BUFFER_DESC vRDView;
	vRDView.Usage = D3D11_USAGE_DYNAMIC;
	vRDView.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vRDView.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vRDView.ByteWidth = sizeof(SCENE_TO_VRAM);
	vRDView.StructureByteStride = sizeof(float);
	vRDView.MiscFlags = 0;

	device->CreateBuffer(&vRDView, nullptr, &cBufferView);

	//for star
	j = 1;
	int k = 1;
	for (size_t i = 0; i < numIn; i++)
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
	//vRDIndex.StructureByteStride = sizeof(unsigned int);
	vRDIndex.MiscFlags = 0;

	device->CreateBuffer(&vRDIndex, &IndexData, &iBuffer);

	D3D11_SUBRESOURCE_DATA IndexData2;

	IndexData2.pSysMem = teapot_indicies;
	IndexData2.SysMemPitch = 0;
	IndexData2.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vRDIndex2;
	vRDIndex2.Usage = D3D11_USAGE_IMMUTABLE;
	vRDIndex2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vRDIndex2.CPUAccessFlags = NULL;
	vRDIndex2.ByteWidth = sizeof(unsigned int) * 4632;
	vRDIndex2.StructureByteStride = sizeof(unsigned int);
	vRDIndex2.MiscFlags = 0;

	device->CreateBuffer(&vRDIndex2, &IndexData2, &iBuffer2);

	// TODO: PART 3 STEP 4b

	float nearPlane = 0.1, farPlane = 100, fieldOfView = 30, AspectRatio = BACKBUFFER_HEIGHT / BACKBUFFER_WIDTH;
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, AspectRatio, nearPlane, farPlane);
	worldMatrix = XMMatrixTranslation(0, 0, 5) * worldMatrix;

	ground.worldMatrix = worldMatrix;
	obj.worldMatrix = worldMatrix;
	obj2.worldMatrix = worldMatrix;
	camera.projectionMatrix = projectionMatrix;
	camera.viewMatrix = viewMatrix;
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// TODO: PART 4 STEP 2
	time.Signal();
	obj.worldMatrix = XMMatrixRotationY(time.Delta()) * obj.worldMatrix;

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
	float camRotSpd = .005;

	XMMATRIX cam = XMMatrixIdentity();
	XMMATRIX trans;
	
	cam = cam * XMMatrixRotationY(mouse.x * camRotSpd);
	cam = XMMatrixRotationX(mouse.y * camRotSpd) * cam;

	trans = XMMatrixTranslation((float)xMove, (float)yMove, (float)zMove);

	cam.r[3] = trans.r[3];

	camera.viewMatrix = XMMatrixInverse(nullptr, cam);

	//////

	dContext->OMSetRenderTargets(1, &rTView, stenView);

	// TODO: PART 1 STEP 7b

	dContext->RSSetViewports(1, &vPort);

	// TODO: PART 1 STEP 7c

	float color[4];
	color[2] = 1;
	dContext->ClearRenderTargetView(rTView, color);
	dContext->ClearDepthStencilView(stenView, D3D11_CLEAR_DEPTH, 1, 0);

	// TODO: PART 5 STEP 4

	// TODO: PART 5 STEP 5

	// TODO: PART 5 STEP 6

	// TODO: PART 5 STEP 7

	// END PART 5

	// TODO: PART 3 STEP 5

	D3D11_MAPPED_SUBRESOURCE map;

	dContext->Map(groundCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	//((SEND_TO_VRAM*)map.pData)->constantColor[0] = toShader.constantColor[0];
	memcpy(map.pData, &ground, sizeof(ground));
	dContext->Unmap(groundCBuffer, 0);

	dContext->Map(cBufferView, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	//((SEND_TO_VRAM*)map.pData)->constantColor[0] = toShader.constantColor[0];
	memcpy(map.pData, &camera, sizeof(camera));
	dContext->Unmap(cBufferView, 0);

	// TODO: PART 3 STEP 6
	dContext->VSSetConstantBuffers(0, 1, &groundCBuffer);
	dContext->VSSetConstantBuffers(1, 1, &cBufferView);
	dContext->RSSetState(rasStateThree);
	//set pixel shader

	// TODO: PART 2 STEP 9a

	unsigned int stride = sizeof(SIMPLE_VERTEX), offSet = 0;
	dContext->IASetVertexBuffers(0, 1, &groundVBuffer, &stride, &offSet);
	dContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, offSet);

	// TODO: PART 2 STEP 9b

	dContext->VSSetShader(VShader, NULL, 0);
	dContext->PSSetShader(PShader, NULL, 0);

	// TODO: PART 2 STEP 9c

	dContext->IASetInputLayout(ILayout);

	// TODO: PART 2 STEP 9d

	//dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// TODO: PART 2 STEP 10

	//dContext->DrawIndexed(numIn, 0, 0);
	dContext->Draw(groundVCount, 0);

	dContext->Map(objCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	//((SEND_TO_VRAM*)map.pData)->constantColor[0] = toShader.constantColor[0];
	memcpy(map.pData, &obj, sizeof(obj));
	dContext->Unmap(objCBuffer, 0);

	dContext->VSSetConstantBuffers(0, 1, &objCBuffer);
	dContext->IASetVertexBuffers(0, 1, &oBjVBuffer, &stride, &offSet);

	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dContext->DrawIndexed(numIn, 0, 0);


	//OBJECT

	dContext->IASetIndexBuffer(iBuffer2, DXGI_FORMAT_R32_UINT, offSet);
	dContext->Map(obj2CBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	//((SEND_TO_VRAM*)map.pData)->constantColor[0] = toShader.constantColor[0];
	memcpy(map.pData, &obj2, sizeof(obj2));
	dContext->Unmap(obj2CBuffer, 0);

	stride = sizeof(OBJ_VERT);
	dContext->VSSetConstantBuffers(0, 1, &obj2CBuffer);
	dContext->IASetVertexBuffers(0, 1, &oBj2VBuffer, &stride, &offSet);

	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dContext->DrawIndexed(4632, 0, 0);

	// END PART 2

	// TODO: PART 1 STEP 8
	swapChain->Present(0, 0);
	// END OF PART 1
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6

	dContext->ClearState();

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//