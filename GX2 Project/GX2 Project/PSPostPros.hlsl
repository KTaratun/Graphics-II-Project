Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct colorFromRasterizer
{
	float4 local : SV_POSITION;
	float2 uv : UVS;
	float3 nrm : NORMALS;
};

float4 main(colorFromRasterizer input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(SampleType, input.uv);
	return textureColor;
}