#include "Lights.h"

//void CreateDirectionalLight(ID3D11Device** device, DIRECTIONAL_LIGHT& dL)
//{
//	D3D11_BUFFER_DESC constBuffDesc;
//	constBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
//	constBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	constBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	constBuffDesc.ByteWidth = sizeof(DIRECTIONAL_LIGHT);
//	constBuffDesc.StructureByteStride = sizeof(float);
//	constBuffDesc.MiscFlags = 0;
//
//	(*device)->CreateBuffer(&constBuffDesc, nullptr, &dL.cBuffer);
//}

void PointLightMovement(POINT_LIGHT &pl, XTime &time, double &xMove,
	double &yMove, double &zMove, XMMATRIX& obj)
{
	if (GetAsyncKeyState('I'))
		zMove += 2 * time.Delta();
	else if (GetAsyncKeyState('K'))
		zMove -= 2 * time.Delta();
	if (GetAsyncKeyState('U'))
		yMove -= 2 * time.Delta();
	else if (GetAsyncKeyState('O'))
		yMove += 2 * time.Delta();
	if (GetAsyncKeyState('J'))
		xMove -= 2 * time.Delta();
	else if (GetAsyncKeyState('L'))
		xMove += 2 * time.Delta();

	XMMATRIX trans = XMMatrixTranslation((float)xMove, (float)yMove, (float)zMove);

	obj = trans;
	pl.pos = XMFLOAT4(trans.r[3].m128_f32[0], trans.r[3].m128_f32[1], trans.r[3].m128_f32[2], 0);
}

void SpotLightMovement(SPOT_LIGHT &sl, XTime &time, double &xMove,
	double &yMove, double &zMove)
{
	//if (GetAsyncKeyState('I'))
	//	zMove += 2 * time.Delta();
	//else if (GetAsyncKeyState('K'))
	//	zMove -= 2 * time.Delta();
	//if (GetAsyncKeyState('U'))
	//	yMove -= 2 * time.Delta();
	//else if (GetAsyncKeyState('O'))
	//	yMove += 2 * time.Delta();
	if (GetAsyncKeyState('T'))
		xMove -= 2 * time.Delta();
	else if (GetAsyncKeyState('Y'))
		xMove += 2 * time.Delta();

	if (GetAsyncKeyState('G'))
		yMove -= 2 * time.Delta();
	else if (GetAsyncKeyState('H'))
		yMove += 2 * time.Delta();

	sl.conDirect.x = yMove;
	XMMATRIX trans = XMMatrixTranslation((float)xMove, 0, 0);
	sl.pos.x = trans.r[3].m128_f32[0];
}