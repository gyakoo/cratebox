
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState smDiffuse : register( s0 );

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = input.Pos;
	output.Tex = input.Tex;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input) : SV_Target
{
    return txDiffuse.Sample( smDiffuse, input.Tex );
}