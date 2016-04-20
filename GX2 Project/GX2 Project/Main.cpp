//includes

#include <iostream>
#include <assert.h>
#include <windows.h>
#include <ctime>
#include "Defines.h"
#include "teapot.h"

#include "Trivial_PSCOLOR.csh"
#include "Trivial_VSCOLOR.csh"
#include "PSUV.csh"
#include "VSUV.csh"

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

	ID3D11Buffer *groundVBuffer;
	ID3D11Buffer *groundIBuffer;
	ID3D11Buffer *starVBuffer;
	ID3D11Buffer *starIBuffer;
	ID3D11Buffer *objCBuffer;
	ID3D11Buffer *loadCBuffer;
	ID3D11Buffer *cBufferView;
	ID3D11Buffer *cBufferDirectional;
	ID3D11Buffer *cBufferPoint;
	ID3D11Buffer *cBufferSpot;

	ID3D11InputLayout *ILayoutCOLOR;
	ID3D11InputLayout *ILayoutUV;
	ID3D11VertexShader *VShaderCOLOR;
	ID3D11PixelShader *PShaderCOLOR;
	ID3D11VertexShader *VShaderUV;
	ID3D11PixelShader *PShaderUV;

	ID3D11Texture2D *texTwoD;
	ID3D11DepthStencilView *stenView;
	ID3D11RasterizerState *rasState;
	D3D11_MAPPED_SUBRESOURCE map;
	XTime time;

	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix;

public:
	
	DIRECTIONAL_LIGHT dL;
	POINT_LIGHT pL;
	SPOT_LIGHT sL;
	OBJ_STRUCT pyramid;
	OBJECT_TO_VRAM obj;
	OBJECT_TO_VRAM ground;
	SCENE_TO_VRAM camera;
	unsigned int numIn = 60;
	unsigned int inBuffer[60];
	unsigned int groundInBuffer[4];

	double xMove = 0;
	double yMove = 0;
	double zMove = 0;
	double xMovePL = 0;
	double yMovePL = 0;
	double zMovePL = 0;

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


	CreateDepthBuffer(&device, &dContext, &rTView, &stenView, &rasState, &texTwoD, &swapChain);

	// VIEWPORT SETUP
	vPort.Height = BACKBUFFER_HEIGHT;
	vPort.MaxDepth = 1;
	vPort.MinDepth = 0;
	vPort.Width = BACKBUFFER_WIDTH;

	// OBJ CREATION
	CreateStar(inBuffer, &device, &starVBuffer, &starIBuffer);
	CreateGround(groundInBuffer, &device, &groundVBuffer, &groundIBuffer);

	// OBJ LOADING
	LoadOBJ("test pyramid.obj", L"GXIIfloor.dds", pyramid, device);

	// SHADER CREATION
	device->CreateVertexShader(Trivial_VSCOLOR, sizeof(Trivial_VSCOLOR), nullptr, &VShaderCOLOR);
	device->CreatePixelShader(Trivial_PSCOLOR, sizeof(Trivial_PSCOLOR), nullptr, &PShaderCOLOR);

	device->CreateVertexShader(VSUV, sizeof(VSUV), nullptr, &VShaderUV);
	device->CreatePixelShader(PSUV, sizeof(PSUV), nullptr, &PShaderUV);

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
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
	vRDSPOT.ByteWidth = sizeof(POINT_LIGHT);
	vRDSPOT.StructureByteStride = sizeof(float);
	vRDSPOT.MiscFlags = 0;

	device->CreateBuffer(&vRDSPOT, nullptr, &cBufferSpot);

	// CAMERA SETUP
	float nearPlane = 0.1f, farPlane = 100, fieldOfView = 30, AspectRatio = BACKBUFFER_HEIGHT / BACKBUFFER_WIDTH;
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, AspectRatio, nearPlane, farPlane);
	camera.projectionMatrix = projectionMatrix;
	camera.viewMatrix = viewMatrix;

	// LIGHT SETUP
	dL.color = float4(1, 1, 1, 1);
	dL.direct = float4(0, 0, 1, 0);

	pL.color = float4(1, 1, 1, 1);
	pL.pos = float4(0, 1, 1, 0);

	sL.color = float4(1, 1, 1, 1);
	sL.pos = float4(0, 1, 1, 0);
	sL.conDirect = float4(1, 1, 1, 1);
	
	// WORLD MATRICIES SETUP
	obj.worldMatrix = XMMatrixIdentity();
	obj.worldMatrix = XMMatrixTranslation(pL.pos.x, pL.pos.y, pL.pos.z) * obj.worldMatrix;

	ground.worldMatrix = XMMatrixIdentity();
	ground.worldMatrix = XMMatrixTranslation(0, -1, 5) * ground.worldMatrix;

	XMMATRIX PyWo = XMLoadFloat4x4(&pyramid.worldMatrix);
	PyWo = XMMatrixTranslation(0, 0, 5) * PyWo;
	PyWo = XMMatrixScaling(2, 2, 2) * PyWo;
	XMStoreFloat4x4(&pyramid.worldMatrix, PyWo);
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// OBJ ROTATION
	time.Signal();
	XMMATRIX PyWo = XMLoadFloat4x4(&pyramid.worldMatrix);
	PyWo = XMMatrixRotationY((float)time.Delta()) * PyWo;
	XMStoreFloat4x4(&pyramid.worldMatrix, PyWo);

	// MOVEMENT
	CameraMovement(camera, time, xMove, yMove, zMove);
	PointLightMovement(pL, time, xMovePL, yMovePL, zMovePL, obj.worldMatrix);

	// VIEW SETUP
	dContext->VSSetConstantBuffers(1, 1, &cBufferView);
	dContext->OMSetRenderTargets(1, &rTView, stenView);
	dContext->RSSetViewports(1, &vPort);

	dContext->Map(cBufferView, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &camera, sizeof(camera));
	dContext->Unmap(cBufferView, 0);

	dContext->RSSetState(rasState);

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

	// SCREEN CLEAR
	float color[4];
	color[2] = 1;
	dContext->ClearRenderTargetView(rTView, color);
	dContext->ClearDepthStencilView(stenView, D3D11_CLEAR_DEPTH, 1, 0);

	// COLORED OBJS
	unsigned int stride = sizeof(SIMPLE_VERTEX), offSet = 0;
	dContext->IASetInputLayout(ILayoutCOLOR);
	dContext->VSSetConstantBuffers(0, 1, &objCBuffer);
	dContext->VSSetShader(VShaderCOLOR, NULL, 0);
	dContext->PSSetShader(PShaderCOLOR, NULL, 0);

	// STAR
	dContext->IASetIndexBuffer(starIBuffer, DXGI_FORMAT_R32_UINT, offSet);
	dContext->Map(objCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &obj, sizeof(obj));
	dContext->Unmap(objCBuffer, 0);

	dContext->IASetVertexBuffers(0, 1, &starVBuffer, &stride, &offSet);
	dContext->IASetIndexBuffer(starIBuffer, DXGI_FORMAT_R32_UINT, offSet);

	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dContext->DrawIndexed(numIn, 0, 0);

	// TEXTURED OBJS
	stride = sizeof(OBJ_TO_VRAM), offSet = 0;
	dContext->IASetInputLayout(ILayoutUV);
	dContext->VSSetConstantBuffers(0, 1, &loadCBuffer);
	dContext->VSSetShader(VShaderUV, NULL, 0);
	dContext->PSSetShader(PShaderUV, NULL, 0);
	//ID3D11SamplerState *sState;
	//dContext->PSSetSamplers(0, 1, &sState);

	// GROUND
	dContext->IASetIndexBuffer(groundIBuffer, DXGI_FORMAT_R32_UINT, offSet);
	dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &ground, sizeof(ground));
	dContext->Unmap(loadCBuffer, 0);

	dContext->IASetVertexBuffers(0, 1, &groundVBuffer, &stride, &offSet);
	dContext->IASetIndexBuffer(groundIBuffer, DXGI_FORMAT_R32_UINT, offSet);

	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dContext->DrawIndexed(6, 0, 0);

	// PYRAMID
	dContext->IASetIndexBuffer(pyramid.IBuffer, DXGI_FORMAT_R32_UINT, offSet);
	dContext->Map(loadCBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &pyramid.worldMatrix, sizeof(pyramid.worldMatrix));
	dContext->Unmap(loadCBuffer, 0);

	dContext->IASetVertexBuffers(0, 1, &pyramid.VBuffer, &stride, &offSet);
	dContext->PSSetShaderResources(0, 1, &pyramid.SRView);

	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dContext->DrawIndexed(pyramid.indexCount, 0, 0);

	// TO FRONT BUFFER
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