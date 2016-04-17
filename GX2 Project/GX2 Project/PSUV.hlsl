Texture2D shaderTexture : register( t0 );
SamplerState SampleType : register( s0 );

struct colorFromRasterizer
{
	float2 uv : UVS;
	float4 position : SV_POSITION;
};

float4 main(colorFromRasterizer input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(SampleType, input.uv);
	textureColor *= float4(0, 1, 2, 0);
	return textureColor;
}