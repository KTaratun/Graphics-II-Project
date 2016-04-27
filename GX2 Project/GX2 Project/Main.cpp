//includes

#include <iostream>
#include <assert.h>
#include <windows.h>
#include <ctime>
#include "Defines.h"
#include "teapot.h"
#include <thread>

#include "Trivial_PSCOLOR.csh"
#include "Trivial_VSCOLOR.csh"
#include "VSInstance.csh"
#include "PSUV.csh"
#include "VSUV.csh"
#include "VSSkyBox.csh"
#include "PSSkyBox.csh"
// That ain't how you abbreviate process...
#include "VSPostPros.csh"
#include "PSPostPros.csh"

using namespace std;

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
	D3D11_VIEWPORT					vPort2;
	D3D11_VIEWPORT					vPort3;

	// CONSTANT BUFFERS
	ID3D11Buffer *objCBuffer;
	ID3D11Buffer *loadCBuffer;
	ID3D11Buffer *cBufferView;
	ID3D11Buffer *cBufferDirectional;
	ID3D11Buffer *cBufferPoint;
	ID3D11Buffer *cBufferSpot;
	ID3D11Buffer *cBufferInstance;

	// LAYOUTS
	ID3D11InputLayout *ILayoutCOLOR;
	ID3D11InputLayout *ILayoutUV;

	// SHADERS
	ID3D11VertexShader *VShaderCOLOR;
	ID3D11PixelShader *PShaderCOLOR;
	ID3D11VertexShader *VShaderUV;
	ID3D11PixelShader *PShaderUV;
	ID3D11VertexShader *VShaderSkyBox;
	ID3D11PixelShader *PShaderSkyBox;
	ID3D11VertexShader *VSShaderInstance;
	ID3D11VertexShader *VSShaderPostPros;
	ID3D11PixelShader *PSShaderPostPros;

	// TEXTURE AND VIEWS
	ID3D11Texture2D *tex2;
	ID3D11Texture2D *tex2DEPTH;
	ID3D11ShaderResourceView *sRVTex2;
	ID3D11ShaderResourceView *sRV;
	ID3D11ShaderResourceView *sRV2;
	ID3D11RenderTargetView *rTView2;
	ID3D11DepthStencilView* stenViewTex2;
	ID3D11Texture2D *texTwoD;
	ID3D11DepthStencilView *stenView;
	ID3D11RasterizerState *rasState;
	D3D11_MAPPED_SUBRESOURCE map;

	// OTHER
	XTime time;
	vector<thread> threads;

	// VIEW
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix;

public:
	
	// OBJECTS
	DIRECTIONAL_LIGHT dL;
	POINT_LIGHT pL;
	SPOT_LIGHT sL;
	OBJ_STRUCT Zack;
	OBJ_STRUCT Behemoth;
	STAR star;
	QUAD ground;
	QUAD screen;
	SCENE_TO_VRAM camera;
	SCENE_TO_VRAM cameraTwo;
	BOX box;
	XMMATRIX instances[3];
	bool camOne;

	bool flipped = false;
	double xMove = 0;
	double yMove = 0;
	double zMove = 0;
	double xMoveCTwo = 0;
	double yMoveCTwo = 0;
	double zMoveCTwo = 0;
	double xMovePL = 0;
	double yMovePL = 0;
	double zMovePL = 0;
	double xMoveSL = 0;
	double yMoveSL = 0;
	double zMoveSL = 0;

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

	// SWAPCHAIN CREATION
	DXGI_SWAP_CHAIN_DESC sCDes;
	ZeroMemory(&sCDes, sizeof(DXGI_SWAP_CHAIN_DESC));
	sCDes.BufferCount = 1;
	sCDes.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sCDes.BufferDesc.Height = BACKBUFFER_HEIGHT;
	sCDes.BufferDesc.Width = BACKBUFFER_WIDTH;
	sCDes.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sCDes.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sCDes.OutputWindow = window;
	sCDes.SampleDesc.Count = 4;
	sCDes.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sCDes.Windowed = true;

	assert(hr == S_OK);

	D3D_FEATURE_LEVEL FeLevel[1] = { D3D_FEATURE_LEVEL_11_1 };// 0xb100;

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//pAdapter
		D3D_DRIVER_TYPE_HARDWARE,	//driverType
		NULL,						//hModule
		flag,						//flags
		FeLevel,					//featureLevel
		1,							//featureLevels
		D3D11_SDK_VERSION,			//SDKVersion
		&sCDes,
		&swapChain,					//swapChain
		&device,					//device
		nullptr,					//featureLevel??
		&dContext);					//deviceContext


	CreateDepthBuffer(&device, &dContext, &rTView, &stenView, &rasState, &texTwoD, &swapChain);

	// VIEWPORT SETUP
	vPort.Height = BACKBUFFER_HEIGHT;
	vPort.MaxDepth = 1;
	vPort.MinDepth = 0;
	vPort.Width = BACKBUFFER_WIDTH / 2;

	vPort2.Height = BACKBUFFER_HEIGHT;
	vPort2.MaxDepth = 1;
	vPort2.MinDepth = 0;
	vPort2.Width = BACKBUFFER_WIDTH / 2;
	vPort2.TopLeftX = BACKBUFFER_WIDTH / 2;

	// original view
	vPort3.Height = BACKBUFFER_HEIGHT;
	vPort3.MaxDepth = 1;
	vPort3.MinDepth = 0;
	vPort3.Width = BACKBUFFER_WIDTH;

	// OBJ CREATION
	CreateStar(star, &device);
	CreateQuadFlat(ground, &device);
	CreateCube(box, &device);
	CreateQuadScreen(screen, &device);

	// OBJ LOADING
	
	//LoadOBJ("Zack Fair.obj", L"Zack Fair.dds", Zack, device);
	//LoadOBJNoNormal("Cloud.OBJ", L"GXIIfloor.dds", Behemoth, device);

	threads.push_back(thread(LoadOBJ, "Zack Fair.obj", L"Zack Fair.dds", &Zack, &device));

	// SHADER CREATION
	device->CreateVertexShader(Trivial_VSCOLOR, sizeof(Trivial_VSCOLOR), nullptr, &VShaderCOLOR);
	device->CreatePixelShader(Trivial_PSCOLOR, sizeof(Trivial_PSCOLOR), nullptr, &PShaderCOLOR);

	device->CreateVertexShader(VSUV, sizeof(VSUV), nullptr, &VShaderUV);
	device->CreatePixelShader(PSUV, sizeof(PSUV), nullptr, &PShaderUV);

	device->CreateVertexShader(VSSkyBox, sizeof(VSSkyBox), nullptr, &VShaderSkyBox);
	device->CreatePixelShader(PSSkyBox, sizeof(PSSkyBox), nullptr, &PShaderSkyBox);

	device->CreateVertexShader(VSInstance, sizeof(VSInstance), nullptr, &VSShaderInstance);

	device->CreateVertexShader(VSPostPros, sizeof(VSPostPros), nullptr, &VSShaderPostPros);
	device->CreatePixelShader(PSPostPros, sizeof(PSPostPros), nullptr, &PSShaderPostPros);

	// LAYOUTS
	D3D11_INPUT_ELEMENT_DESC vLayoutCOLOR[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	D3D11_INPUT_ELEMENT_DESC vLayoutUV[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// LAYOUT CREATION
	device->CreateInputLayout(vLayoutCOLOR, 2, Trivial_VSCOLOR, sizeof(Trivial_VSCOLOR), &ILayoutCOLOR);
	device->CreateInputLayout(vLayoutUV, 3, VSUV, sizeof(VSUV), &ILayoutUV);

	// CONSTANT BUFFER CREATION
	D3D11_BUFFER_DESC objVRD;
	objVRD.Usage = D3D11_USAGE_DYNAMIC;
	objVRD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objVRD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objVRD.ByteWidth = sizeof(OBJECT_TO_VRAM);
	objVRD.StructureByteStride = sizeof(float);
	objVRD.MiscFlags = 0;

	device->CreateBuffer(&objVRD, nullptr, &objCBuffer);

	D3D11_BUFFER_DESC loadVRD;
	loadVRD.Usage = D3D11_USAGE_DYNAMIC;
	loadVRD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	loadVRD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	loadVRD.ByteWidth = sizeof(XMFLOAT4X4);
	loadVRD.StructureByteStride = sizeof(float);
	loadVRD.MiscFlags = 0;

	device->CreateBuffer(&loadVRD, nullptr, &loadCBuffer);

	D3D11_BUFFER_DESC vRDView;
	vRDView.Usage = D3D11_USAGE_DYNAMIC;
	vRDView.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vRDView.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vRDView.ByteWidth = sizeof(SCENE_TO_VRAM);
	vRDView.StructureByteStride = sizeof(float);
	vRDView.MiscFlags = 0;

	device->CreateBuffer(&vRDView, nullptr, &cBufferView);

	D3D11_BUFFER_DESC vRDDIRECTION;
	vRDDIRECTION.Usage = D3D11_USAGE_DYNAMIC;
	vRDDIRECTION.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vRDDIRECTION.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vRDDIRECTION.ByteWidth = sizeof(DIRECTIONAL_LIGHT);
	vRDDIRECTION.StructureByteStride = sizeof(float);
	vRDDIRECTION.MiscFlags = 0;

	device->CreateBuffer(&vRDDIRECTION, nullptr, &cBufferDirectional);

	D3D11_BUFFER_DESC vRDPOINT;
	vRDPOINT.Usage = D3D11_USAGE_DYNAMIC;
	vRDPOINT.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vRDPOINT.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vRDPOINT.ByteWidth = sizeof(POINT_LIGHT);
	vRDPOINT.StructureByteStride = sizeof(float);
	vRDPOINT.MiscFlags = 0;

	device->CreateBuffer(&vRDPOINT, nullptr, &cBufferPoint);

	D3D11_BUFFER_DESC vRDSPOT;
	vRDSPOT.Usage = D3D11_USAGE_DYNAMIC;
	vRDSPOT.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vRDSPOT.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vRDSPOT.ByteWidth = sizeof(SPOT_LIGHT);
	vRDSPOT.StructureByteStride = sizeof(float);
	vRDSPOT.MiscFlags = 0;

	device->CreateBuffer(&vRDSPOT, nullptr, &cBufferSpot);

	D3D11_BUFFER_DESC vRDInstance;
	vRDInstance.Usage = D3D11_USAGE_DYNAMIC;
	vRDInstance.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vRDInstance.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vRDInstance.ByteWidth = sizeof(instances);
	vRDInstance.StructureByteStride = sizeof(XMMATRIX);
	vRDInstance.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA instData;

	instData.pSysMem = &instances;
	instData.SysMemPitch = 0;
	instData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vRDInstance, &instData, &cBufferInstance);

	// RENDER TO TEXTURE
	D3D11_TEXTURE2D_DESC Tex2DDesc;
	ZeroMemory(&Tex2DDesc, sizeof(Tex2DDesc));
	Tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Tex2DDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	Tex2DDesc.SampleDesc.Count = 1;
	Tex2DDesc.Width = 500;
	Tex2DDesc.Height = 500;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.ArraySize = 1;

	device->CreateTexture2D(&Tex2DDesc, NULL, &tex2);
	
	D3D11_RENDER_TARGET_VIEW_DESC rTVDTex2;
	rTVDTex2.Format = Tex2DDesc.Format;
	rTVDTex2.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rTVDTex2.Texture2D.MipSlice = 0;
	
	device->CreateRenderTargetView(tex2, &rTVDTex2, &rTView2);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC sRVDescTex2;
	sRVDescTex2.Format = Tex2DDesc.Format;
	sRVDescTex2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sRVDescTex2.Texture2D.MostDetailedMip = 0;
	sRVDescTex2.Texture2D.MipLevels = 1;
	
	device->CreateShaderResourceView(tex2, &sRVDescTex2, &sRVTex2);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	D3D11_TEXTURE2D_DESC Tex2DDescDEPTH;
	ZeroMemory(&Tex2DDescDEPTH, sizeof(Tex2DDescDEPTH));
	Tex2DDescDEPTH.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	Tex2DDescDEPTH.Format = DXGI_FORMAT_D32_FLOAT;
	Tex2DDescDEPTH.MiscFlags = 0;
	Tex2DDescDEPTH.SampleDesc.Count = 1;
	Tex2DDescDEPTH.Width = 500;
	Tex2DDescDEPTH.Height = 500;
	Tex2DDescDEPTH.MipLevels = 1;
	Tex2DDescDEPTH.ArraySize = 1;

	device->CreateTexture2D(&Tex2DDescDEPTH, NULL, &tex2DEPTH);

	// CREATE DEPTH STENCIL VIEW
	device->CreateDepthStencilView(tex2DEPTH, &descDSV, &stenViewTex2);

	// POST PROCESS
	device->CreateShaderResourceView(tex2, &sRVDescTex2, &sRV);
	device->CreateShaderResourceView(tex2, &sRVDescTex2, &sRV2);


	// CAMERA SETUP
	float nearPlane = 0.1f, farPlane = 100, fieldOfView = 30, AspectRatio = BACKBUFFER_HEIGHT / BACKBUFFER_WIDTH;
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, AspectRatio, nearPlane, farPlane);
	camera.projectionMatrix = projectionMatrix;
	camera.viewMatrix = viewMatrix;
	cameraTwo.projectionMatrix = projectionMatrix;
	cameraTwo.viewMatrix = viewMatrix;

	// LIGHT SETUP
	dL.color = float4(1, 1, 1, 1);
	dL.direct = float4(1, -2, 0, 0);

	pL.color = float4(1, .2, .2, 1);
	pL.pos = float4(0, 1, 1, 0);

	sL.color = float4(.2, .2, 1, 1);
	sL.pos = float4(0, 2, 5, 0);
	sL.conDirect = float4(0, -1, 0, 0);
	
	for (size_t i = 0; i < threads.size(); i++)
		threads[i].join();

	// WORLD MATRICIES SETUP
	star.TO_VRAM.worldMatrix = XMMatrixIdentity();
	star.TO_VRAM.worldMatrix = XMMatrixTranslation(pL.pos.x, pL.pos.y, pL.pos.z) * star.TO_VRAM.worldMatrix;

	ground.TO_VRAM.worldMatrix = XMMatrixIdentity();
	ground.TO_VRAM.worldMatrix = XMMatrixTranslation(0, -1, 5) * ground.TO_VRAM.worldMatrix;

	box.TO_VRAM.worldMatrix = XMMatrixIdentity();

	screen.TO_VRAM.worldMatrix = XMMatrixIdentity();
	screen.TO_VRAM.worldMatrix = XMMatrixScaling(20, 20, 20) * screen.TO_VRAM.worldMatrix;

	// INSTANCING
	XMMATRIX ZackWorld = XMLoadFloat4x4(&Zack.worldMatrix);
	ZackWorld = XMMatrixRotationY(-4.8) * ZackWorld;
	ZackWorld = XMMatrixRotationX(29.84f) * ZackWorld;
	ZackWorld = XMMatrixTranslation(-5, 0, -1) * ZackWorld;
	ZackWorld = XMMatrixScaling(.01, .01, .01) * ZackWorld;
	XMStoreFloat4x4(&Zack.worldMatrix, ZackWorld);

	instances[0] = XMMatrixIdentity();
	instances[0] = XMMatrixRotationY(-4.8) * instances[0];
	instances[0] = XMMatrixRotationX(29.84f) * instances[0];
	instances[0] = XMMatrixTranslation(-5, 0, -1) * instances[0];
	instances[0] = XMMatrixScaling(.01, .01, .01) * instances[0];

	instances[1] = XMMatrixIdentity();
	instances[1] = XMMatrixRotationY(-4.8) * instances[1];
	instances[1] = XMMatrixRotationX(29.84f) * instances[1];
	instances[1] = XMMatrixTranslation(-8, 0, -1) * instances[1];
	instances[1] = XMMatrixScaling(.01, .01, .01) * instances[1];

	instances[2] = XMMatrixIdentity();
	instances[2] = XMMatrixRotationY(-4.8) * instances[2];
	instances[2] = XMMatrixRotationX(29.84f) * instances[2];
	instances[2] = XMMatrixTranslation(-3, 0, -1) * instances[2];
	instances[2] = XMMatrixScaling(.01, .01, .01) * instances[2];

	cameraTwo.viewMatrix = XMMatrixIdentity();
	cameraTwo.viewMatrix = XMMatrixRotationZ(XMConvertToRadians(180)) * cameraTwo.viewMatrix;
	cameraTwo.viewMatrix = XMMatrixTranslation(.3, -.8, -4.5) * cameraTwo.viewMatrix;
	}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// OBJ ROTATION
	time.Signal();
	//XMMATRIX ZackWorld = XMLoadFloat4x4(&Zack.worldMatrix);
	//ZackWorld = XMMatrixRotationY((float)time.Delta()) * ZackWorld;
	//XMStoreFloat4x4(&Zack.worldMatrix, ZackWorld);

	//if (!flipped && dL.direct.x < 20)
		dL.direct.x -= time.Delta() *.2;
	//else if (flipped && dL.direct.x > -20)
	//	dL.direct.x += time.Delta() * 20;

	// MOVEMENT
	CameraMovement(camera, time, xMove, yMove, zMove);
	PointLightMovement(pL, time, xMovePL, yMovePL, zMovePL, star.TO_VRAM.worldMatrix);
	SpotLightMovement(sL, time, xMoveSL, yMoveSL, zMoveSL);

	// VIEW SETUP
	dContext->VSSetConstantBuffers(1, 1, &cBufferView);
	dContext->RSSetState(rasState);

	// INSTANCE SETUP
	dContext->VSSetConstantBuffers(2, 1, &cBufferInstance);
	dContext->Map(cBufferInstance, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &instances, sizeof(instances));
	dContext->Unmap(cBufferInstance, 0);

	// LIGHTS
	dContext->PSSetConstantBuffers(0, 1, &cBufferDirectional);
	dContext->Map(cBufferDirectional, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &dL, sizeof(dL));
	dContext->Unmap(cBufferDirectional, 0);

	dContext->PSSetConstantBuffers(1, 1, &cBufferPoint);
	dContext->Map(cBufferPoint, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &pL, sizeof(pL));
	dContext->Unmap(cBufferPoint, 0);

	dContext->PSSetConstantBuffers(2, 1, &cBufferSpot);
	dContext->Map(cBufferSpot, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &sL, sizeof(sL));
	dContext->Unmap(cBufferSpot, 0);

	// RENDER TO TEXTURE SET UP
	dContext->OMSetRenderTargets(1, &rTView2, stenViewTex2);

	float color[4];
	color[2] = 0;
	dContext->ClearRenderTargetView(rTView2, color);

	// VIEW LOOP
		dContext->Map(cBufferView, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &camera, sizeof(camera));
		dContext->Unmap(cBufferView, 0);
		dContext->RSSetViewports(1, &vPort3);
		box.TO_VRAM.worldMatrix.r[3] = XMMatrixInverse(0, camera.viewMatrix).r[3];

		// SKYBOX
		ID3D11SamplerState *sState;
		D3D11_SAMPLER_DESC samDesc;
		samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samDesc.MaxAnisotropy = 1;
		samDesc.MipLODBias = 0;

		device->CreateSamplerState(&samDesc, &sState);
		dContext->IASetInputLayout(ILayoutCOLOR);
		dContext->PSSetSamplers(0, 1, &sState);
		dContext->VSSetSamplers(0, 1, &sState);
		unsigned int stride = sizeof(OBJ_TO_VRAM), offSet = 0;
		dContext->VSSetConstantBuffers(0, 1, &loadCBuffer);
		dContext->VSSetShader(VShaderSkyBox, NULL, 0);
		dContext->PSSetShader(PShaderSkyBox, NULL, 0);

		dContext->IASetIndexBuffer(box.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &box.TO_VRAM, sizeof(box.TO_VRAM));
		dContext->Unmap(loadCBuffer, 0);

		dContext->IASetVertexBuffers(0, 1, &box.VBuffer, &stride, &offSet);
		dContext->PSSetShaderResources(0, 1, &box.SRView);

		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(36, 0, 0);

		// Clear the depth buffer.
		dContext->ClearDepthStencilView(stenViewTex2, D3D11_CLEAR_DEPTH, 1, 0);

		// COLORED OBJS
		stride = sizeof(SIMPLE_VERTEX), offSet = 0;
		dContext->IASetInputLayout(ILayoutCOLOR);
		dContext->VSSetConstantBuffers(0, 1, &objCBuffer);
		dContext->VSSetShader(VShaderCOLOR, NULL, 0);
		dContext->PSSetShader(PShaderCOLOR, NULL, 0);

		// STAR
		dContext->IASetIndexBuffer(star.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(objCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &star.TO_VRAM, sizeof(star.TO_VRAM));
		dContext->Unmap(objCBuffer, 0);

		dContext->IASetVertexBuffers(0, 1, &star.VBuffer, &stride, &offSet);
		dContext->IASetIndexBuffer(star.IBuffer, DXGI_FORMAT_R32_UINT, offSet);

		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(star.numIn, 0, 0);

		// TEXTURED OBJS
		stride = sizeof(OBJ_TO_VRAM), offSet = 0;
		dContext->IASetInputLayout(ILayoutUV);
		dContext->VSSetShader(VShaderUV, NULL, 0);
		dContext->PSSetShader(PShaderUV, NULL, 0);
		dContext->VSSetConstantBuffers(0, 1, &loadCBuffer);

		// GROUND
		dContext->IASetIndexBuffer(ground.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &ground.TO_VRAM, sizeof(ground.TO_VRAM));
		dContext->Unmap(loadCBuffer, 0);

		dContext->IASetVertexBuffers(0, 1, &ground.VBuffer, &stride, &offSet);
		dContext->PSSetShaderResources(0, 1, &ground.SRView);

		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(6, 0, 0);

		// Zack
		dContext->IASetIndexBuffer(Zack.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &Zack.worldMatrix, sizeof(Zack.worldMatrix));
		dContext->Unmap(loadCBuffer, 0);

		dContext->IASetVertexBuffers(0, 1, &Zack.VBuffer, &stride, &offSet);
		dContext->PSSetShaderResources(0, 1, &Zack.SRView);

		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->VSSetShader(VSShaderInstance, NULL, 0);
		dContext->DrawInstanced(Zack.indexCount, 3, 0, 0);
		//dContext->DrawIndexed(Zack.indexCount, 0, 0);
		
		//// Behemoth
		//dContext->IASetIndexBuffer(Behemoth.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		//dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		//memcpy(map.pData, &Behemoth.worldMatrix, sizeof(Behemoth.worldMatrix));
		//dContext->Unmap(loadCBuffer, 0);
		//
		//dContext->IASetVertexBuffers(0, 1, &Behemoth.VBuffer, &stride, &offSet);
		//dContext->PSSetShaderResources(0, 1, &Behemoth.SRView);
		//
		//dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//dContext->DrawIndexed(Behemoth.indexCount, 0, 0);

		// TO FRONT BUFFER sRVTex2

	dContext->OMSetRenderTargets(1, &rTView, stenView);
	// SCREEN CLEAR
	color[2] = 1;
	dContext->ClearRenderTargetView(rTView, color);
	
	// VIEW LOOP
	for (size_t i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			dContext->Map(cBufferView, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
			memcpy(map.pData, &camera, sizeof(camera));
			dContext->Unmap(cBufferView, 0);
			dContext->RSSetViewports(1, &vPort);
			box.TO_VRAM.worldMatrix.r[3] = XMMatrixInverse(0, camera.viewMatrix).r[3];
		}
		else if (i == 1)
		{
			dContext->Map(cBufferView, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
			memcpy(map.pData, &cameraTwo, sizeof(cameraTwo));
			dContext->Unmap(cBufferView, 0);
			dContext->RSSetViewports(1, &vPort2);
			box.TO_VRAM.worldMatrix.r[3] = XMMatrixInverse(0, cameraTwo.viewMatrix).r[3];
		}
	
		// SKYBOX
		ID3D11SamplerState *sState;
		D3D11_SAMPLER_DESC samDesc;
		samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samDesc.MaxAnisotropy = 1;
		samDesc.MipLODBias = 0;
	
		device->CreateSamplerState(&samDesc, &sState);
		dContext->IASetInputLayout(ILayoutCOLOR);
		dContext->PSSetSamplers(0, 1, &sState);
		dContext->VSSetSamplers(0, 1, &sState);
		unsigned int stride = sizeof(OBJ_TO_VRAM), offSet = 0;
		dContext->VSSetConstantBuffers(0, 1, &loadCBuffer);
		dContext->VSSetShader(VShaderSkyBox, NULL, 0);
		dContext->PSSetShader(PShaderSkyBox, NULL, 0);
	
		dContext->IASetIndexBuffer(box.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &box.TO_VRAM, sizeof(box.TO_VRAM));
		dContext->Unmap(loadCBuffer, 0);
	
		dContext->IASetVertexBuffers(0, 1, &box.VBuffer, &stride, &offSet);
		dContext->PSSetShaderResources(0, 1, &box.SRView);
	
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(36, 0, 0);
	
		dContext->ClearDepthStencilView(stenView, D3D11_CLEAR_DEPTH, 1, 0);
	
		// COLORED OBJS
		stride = sizeof(SIMPLE_VERTEX), offSet = 0;
		dContext->IASetInputLayout(ILayoutCOLOR);
		dContext->VSSetConstantBuffers(0, 1, &objCBuffer);
		dContext->VSSetShader(VShaderCOLOR, NULL, 0);
		dContext->PSSetShader(PShaderCOLOR, NULL, 0);
	
		// STAR
		dContext->IASetIndexBuffer(star.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(objCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &star.TO_VRAM, sizeof(star.TO_VRAM));
		dContext->Unmap(objCBuffer, 0);
	
		dContext->IASetVertexBuffers(0, 1, &star.VBuffer, &stride, &offSet);
		dContext->IASetIndexBuffer(star.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
	
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(star.numIn, 0, 0);
	
		// TEXTURED OBJS
		stride = sizeof(OBJ_TO_VRAM), offSet = 0;
		dContext->IASetInputLayout(ILayoutUV);
		dContext->VSSetShader(VShaderUV, NULL, 0);
		dContext->PSSetShader(PShaderUV, NULL, 0);
		dContext->VSSetConstantBuffers(0, 1, &loadCBuffer);
	
		// GROUND
		dContext->IASetIndexBuffer(ground.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &ground.TO_VRAM, sizeof(ground.TO_VRAM));
		dContext->Unmap(loadCBuffer, 0);
	
		dContext->IASetVertexBuffers(0, 1, &ground.VBuffer, &stride, &offSet);
		dContext->PSSetShaderResources(0, 1, &sRVTex2);
	
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(6, 0, 0);
	
		// Zack
		dContext->IASetIndexBuffer(Zack.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		memcpy(map.pData, &Zack.worldMatrix, sizeof(Zack.worldMatrix));
		dContext->Unmap(loadCBuffer, 0);
	
		dContext->IASetVertexBuffers(0, 1, &Zack.VBuffer, &stride, &offSet);
		dContext->PSSetShaderResources(0, 1, &Zack.SRView);
	
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->VSSetShader(VSShaderInstance, NULL, 0);
		dContext->DrawInstanced(Zack.indexCount, 3, 0, 0);

		// SCREEN
		//dContext->PSSetShader(PSShaderPostPros, NULL, 0);
		//dContext->VSSetShader(VSShaderPostPros, NULL, 0);
		//dContext->IASetIndexBuffer(screen.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
		//dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
		//memcpy(map.pData, &screen.TO_VRAM, sizeof(screen.TO_VRAM));
		//dContext->Unmap(loadCBuffer, 0);
		//
		//dContext->IASetVertexBuffers(0, 1, &screen.VBuffer, &stride, &offSet);
		//
		//if (i == 0)
		//	dContext->PSSetShaderResources(0, 1, &sRV);
		//else
		//	dContext->PSSetShaderResources(0, 1, &sRV2);
		
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dContext->DrawIndexed(6, 0, 0);

		//ID3D11ShaderResourceView *null{};
		//dContext->PSSetShaderResources(0, 1, &null);
		//dContext->VSSetShaderResources(0, 1, &null);

		// TO FRONT BUFFER
	}
	ID3D11DepthStencilView *null{};
	dContext->OMSetRenderTargets(1, &rTView, null);

	swapChain->Present(0, 0);
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
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