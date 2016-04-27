#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float2 uvs : UVS;
	float3 nrm : NORMALS;
};

struct OUTPUT_VERTEX
{
	float4 local : SV_POSITION;
	float2 uvs : UVS;
	float3 nrm : NORMALS;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	// ensures translation is preserved during matrix multiply  
	sendToRasterizer.local = float4(fromVertexBuffer.coordinate, 1);
	sendToRasterizer.uvs = fromVertexBuffer.uvs;
	return sendToRasterizer;
}