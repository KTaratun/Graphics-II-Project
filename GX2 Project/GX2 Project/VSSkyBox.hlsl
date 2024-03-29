#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float3 color : COLOR;
	//float3 texCoor : SKYTEX;
};

struct OUTPUT_VERTEX
{
	float3 colorOut : COLOR;
	float3 skyBox : POSITION;
	float4 projectedCoordinate : SV_POSITION;
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
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);

	sendToRasterizer.projectedCoordinate = localH;

	// TODO : PART 4 STEP 4
	/*sendToRasterizer.projectedCoordinate.xy += constantOffset;*/

	// TODO : PART 3 STEP 7
	sendToRasterizer.colorOut = fromVertexBuffer.color;
	// END PART 3

	sendToRasterizer.skyBox = fromVertexBuffer.coordinate;

	// TODO: Move into view space, then projection space
	return sendToRasterizer;
}