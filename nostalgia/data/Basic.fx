
//--------------------------------------------------------------------------------------
// Texture2D basicTexture : register( t0 );
// SamplerState basicSampler : register( s0 );
Texture2D channel0 : register(t0);
Texture2D channel1 : register(t1);
Texture2D channel2 : register(t2);
Texture2D channel3 : register(t4);
SamplerState channelSampler0: register( s0 );
SamplerState channelSampler1: register( s1 );
SamplerState channelSampler2: register( s2 );
SamplerState channelSampler3: register( s3 );
float globalTime = 0.0;

float4 sample(float2 uv)
{
  float c = floor((uv.x)/0.1)%2;
  c += floor((uv.y)/0.1)%2;
  c %= 2;
  return float4(c,c,c,1.0);
}

float4 smooth(float2 uv)
{
  float2 dx = float2(0.0005,0);
  float2 dy = float2(0,0.0005);
  return 0.20*sample(uv) 
  + 0.2*sample(uv-dx) + 0.2*sample(uv+dx) 
  + 0.2*sample(uv-dy) + 0.2*sample(uv+dy);
}

//--------------------------------------------------------------------------------------
float4 FlagPS(float4 Pos, float2 uv)
{
  float2 uvsrc=uv;
  float pi=3.141592;  
  uv.x += uv.x*sin(pi*uv.y+globalTime)*0.05;
  uv.y += uv.y*2.0*cos(pi*uv.x+globalTime)*0.05;
  float4 samp = smooth(uv);

  float2 light=float2( sin(globalTime*0.2)*0.5+0.5,cos(globalTime*0.3)*0.5+0.5);
  float rad=0.4;
  float2 tol=uvsrc-light;
  float r = clamp(length(tol),0.0,rad)/rad;
  float i0 = 1.0-pow(r, 0.8);

  light=float2( sin(globalTime*0.5)*0.5+0.5,sin(globalTime*0.3)*0.5+0.5);
  rad=0.2;
  tol=uvsrc-light;
  r = clamp(length(tol),0.0,rad)/rad;
  float i1 = 1.0-pow(r, clamp(sin(globalTime*8)*0.5+0.5,0.2,1.0));
  
  return samp*i0+samp*float4(1,0,0,1)*i1;
}

//--------------------------------------------------------------------------------------
float4 pd0(float4 pos, float2 uv)
{
  float2 xy = 2*uv-float2(1,1);
  float d = length(xy);
  float a = atan2( xy.y, xy.x );
  
  uv.x = (globalTime*0.3%2)+d*cos(a+d*sin(globalTime));
  uv.y = sin(globalTime*0.2)+d*sin(a+d*sin(globalTime));
  return channel0.Sample( channelSampler0, uv );
}

//--------------------------------------------------------------------------------------
float4 pd1(float4 pos, float2 uv)
{
  uv.y += sin(globalTime*0.5)*0.3;
  uv.x += sin(globalTime*0.2)*0.6;
  float2 xy = 2*uv-float2(1,1);
  float d = length(xy);
  float a = atan2( xy.y, xy.x );

  uv.x = 1/abs(xy.y)+(globalTime%2);
  uv.y = xy.x/abs(xy.y);

  return channel0.Sample( channelSampler0, uv );
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( float4 Pos: SV_POSITION, float2 uv:TEXCOORD0) : SV_Target
{
  //return pd0(Pos,uv);
  //return FlagPS(Pos,uv);
  return pd1(Pos,uv);
}
