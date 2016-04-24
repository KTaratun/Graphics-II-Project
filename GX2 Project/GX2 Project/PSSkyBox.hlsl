TextureCube skyTexture : register(t0);
SamplerState SampleType : register(s0);

struct colorFromRasterizer
{
	float3 color : COLOR;
	float3 skyBox : POSITION;
	float4 projectedCoordinate : SV_POSITION;
};

float3 main(colorFromRasterizer input) : SV_TARGET
{
	float3 texColor = skyTexture.Sample(SampleType, input.skyBox.xyz);
	return texColor;
}