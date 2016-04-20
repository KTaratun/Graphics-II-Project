Texture2D shaderTexture : register( t0 );
SamplerState SampleType : register( s0 );

struct colorFromRasterizer
{
	float2 uv : UVS;
	float4 position : SV_POSITION;
	float3 normals : NORMALS;
	float3 world : WORLD;
};

cbuffer DIRECTION : register(b0)
{
	float4 directionDL;
	float4 colorDL;
}

cbuffer POINT : register(b1)
{
	float4 colorPL;
	float4 positionPL;
}

cbuffer SPOT : register(b2)
{
	float4 conDirectSL;
	float4 colorSL;
	float4 positionSL;
}

float4 main(colorFromRasterizer input) : SV_TARGET
{
	input.normals = normalize(input.normals);

	// DIRECTIONAL
	// do an if check with a bool
	float4 textureColor = shaderTexture.Sample(SampleType, input.uv);
	float lightRatioDL = saturate(dot(-directionDL, input.normals));
	if (lightRatioDL < .2)
		lightRatioDL = .2;
	float dResult = lightRatioDL * colorDL * textureColor;

	// POINT
	// do an if check with a bool
	float3 LightDirectPL = normalize(positionPL - input.world);
	float lightRatioPL = saturate(dot(LightDirectPL, input.normals));
	if (lightRatioPL < .2)
		lightRatioPL = .2;
	float3 attenuation = 1.0 - saturate(length(positionPL - input.world) / 5);
	float pResult = lightRatioPL * attenuation * colorPL * textureColor;

	// SPOT
	// do an if check with a bool
	//float3 LightDirectSL = normalize(positionSL - input.world);
	//float SurfaceRatio = saturate(dot(-LightDirectSL, conDirectSL));
	//float SpotFactor = (SurfaceRatio >)?1:0;
	//float lightRatioSL = saturate(dot(LightDirectSL, input.normals));
	//float sResult = SpotFactor * lightRatioSL * colorSL * textureColor;


	//saturate(dResult + pResult)
	return pResult;
}