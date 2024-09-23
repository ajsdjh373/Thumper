Texture2D tex;

SamplerState splr;

float4 main(float2 tc : TexCoord) : SV_Target
{
	float4 atex = tex.Sample(splr,tc);
	clip(atex.a < 0.1f ? -1 : 1);
	return float4(atex.rgb, atex.a);
}