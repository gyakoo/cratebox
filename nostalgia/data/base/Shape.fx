cbuffer PerFrame : register(b0) 
{
  float4x4 g_matM;
  float4x4 g_matV;
  float4x4 g_matP;
  float4   g_color=float4(1,1,1,1);
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos   : POSITION;
};
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos   : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(mul(mul(float4(input.Pos.xyz,1.0f),g_matM), g_matV), g_matP);

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input) : SV_Target
{
    return g_color;
}