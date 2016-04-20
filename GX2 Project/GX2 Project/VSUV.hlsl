#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float2 uvs : UVS;
	float3 nrm : NORMALS;
};

struct OUTPUT_VERTEX
{
	float2 uvOut : UVS;
	float4 projectedCoordinate : SV_POSITION;
	float3 normals : NORMALS;
	float3 world : WORLD;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	// ensures translation is preserved during matrix multiply  
	float4 localH = float4(fromVertexBuffer.coordinate, 1);
		// move local space vertex from vertex buffer into world space.

	localH = mul(localH, worldMatrix);
	float3 worldPos = localH;
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);

	sendToRasterizer.world = worldPos;
	sendToRasterizer.projectedCoordinate = localH;

	sendToRasterizer.uvOut = fromVertexBuffer.uvs;

	sendToRasterizer.normals = mul(float4(fromVertexBuffer.nrm, 0), (float3x3)worldMatrix);

	return sendToRasterizer;
}