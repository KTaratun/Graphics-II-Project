#pragma once

#include "SharedDefines.h"
#include "Lights.h"
//#include <atlbase.h>
//CComPtr<int> kss;

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

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

struct QUAD
{
	ID3D11Buffer *VBuffer;
	ID3D11Buffer *IBuffer;
	ID3D11ShaderResourceView *SRView;
	unsigned int InBuffer[4];
	OBJECT_TO_VRAM TO_VRAM;
};

struct STAR
{
	ID3D11Buffer *VBuffer;
	ID3D11Buffer *IBuffer;
	ID3D11ShaderResourceView *SRView;
	unsigned int numIn = 60;
	unsigned int inBuffer[60];
	OBJECT_TO_VRAM TO_VRAM;
};

struct BOX
{
	ID3D11Buffer *VBuffer;
	ID3D11Buffer *IBuffer;
	ID3D11ShaderResourceView *SRView;
	unsigned int InBuffer[36];
	OBJECT_TO_VRAM TO_VRAM;
};

void CreateStar(STAR &star, ID3D11Device** device);

void CreateDepthBuffer(ID3D11Device** device, ID3D11DeviceContext** dContext, ID3D11RenderTargetView** rTView,
	ID3D11DepthStencilView** stenView, ID3D11RasterizerState** rasState, ID3D11Texture2D** texTwoD,
	IDXGISwapChain** swapChain);

void CreateQuadFlat(QUAD &quad, ID3D11Device** device);
void CreateQuadScreen(QUAD &quad, ID3D11Device** device);

void CameraMovement(SCENE_TO_VRAM &camera, XTime &time, double &xMove,
	double &yMove, double &zMove);

void CreateCube(BOX &box, ID3D11Device** device);

//void CameraMovement(SCENE_TO_VRAM &camera, SCENE_TO_VRAM &cameraTwo, XTime &time,
//	double &xMove, double &yMove, double &zMove,
//	double &xMoveTwo, double &yMoveTwo, double &zMoveTwo, bool &camOne);