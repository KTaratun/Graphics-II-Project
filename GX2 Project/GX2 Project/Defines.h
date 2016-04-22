#pragma once

#include "SharedDefines.h"
#include "Lights.h"
//#include <atlbase.h>
//CComPtr<int> kss;

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

void CreateStar(unsigned int* inBuffer, ID3D11Device** device, ID3D11Buffer** starVBuffer,
	ID3D11Buffer** iBuffer);

void CreateDepthBuffer(ID3D11Device** device, ID3D11DeviceContext** dContext, ID3D11RenderTargetView** rTView,
	ID3D11DepthStencilView** stenView, ID3D11RasterizerState** rasState, ID3D11Texture2D** texTwoD,
	IDXGISwapChain** swapChain);

void CreateGround(unsigned int* inBuffer, ID3D11Device** device, ID3D11Buffer **groundVBuffer, 
	ID3D11Buffer **groundIBuffer, ID3D11ShaderResourceView **groundSRView);

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

void CameraMovement(SCENE_TO_VRAM &camera, XTime &time, double &xMove,
	double &yMove, double &zMove);


//void CameraMovement(SCENE_TO_VRAM &camera, SCENE_TO_VRAM &cameraTwo, XTime &time,
//	double &xMove, double &yMove, double &zMove,
//	double &xMoveTwo, double &yMoveTwo, double &zMoveTwo, bool &camOne);