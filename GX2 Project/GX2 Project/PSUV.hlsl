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
	float4 dResult = lightRatioDL * colorDL * textureColor;

	// POINT
	// do an if check with a bool
	float4 LightDirectPL = normalize(positionPL - float4(input.world, 1));
	float lightRatioPL = saturate(dot(LightDirectPL, input.normals));
	if (lightRatioPL < .2)
		lightRatioPL = .2;
	float attenuation = 1.0 - saturate(length(positionPL - input.world) / 5);
	float4 pResult = lightRatioPL * attenuation * colorPL * textureColor;

	// SPOT
	// do an if check with a bool
	float4 LightDirectSL = normalize(positionSL - float4(input.world, 1));
	float SurfaceRatio = saturate(dot(-LightDirectSL, conDirectSL));
	float SpotFactor = (SurfaceRatio > 0.8) ? 1 : 0;
	float lightRatioSL = saturate(dot(LightDirectSL, input.normals));
	//float attenuation = 1.0 - saturate(length(positionSL - input.world) / 5);
	//float attenuation = 1.0 - saturate(length(coneRatioINNER - surfaceRatio) / INNERCONE - CONERATIO);
	float4 sResult = SpotFactor * lightRatioSL * colorSL * textureColor;

	return saturate(dResult + pResult + sResult);
}