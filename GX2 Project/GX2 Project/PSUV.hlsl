Texture2D shaderTexture;
SamplerState SampleType;

struct colorFromRasterizer
{
	float2 uv : UVS;
	float4 position : SV_POSITION;
};

float4 main(colorFromRasterizer input) : SV_TARGET
{
	float4 textureColor;
	textureColor = shaderTexture.Sample(SampleType, input.uv);

	return textureColor;
}