
#define D3D11DEVICE (gyGetRenderer().GetImpl()->D3D11GetDevice())

#define GY_CREATE_D3DSHADER(func) \
  HRESULT hr = D3D11DEVICE->func( (const void*)pByteCode->GetByteCode(), pByteCode->GetLength(),NULL,&pIShader); \
  if ( FAILED(hr) ) \
  GYDEBUGRET( "Cannot create shader. " #func "\n", -1 )


//////////////////////////////////////////////////////////////////////////
// gyRendererFactory::Impl
//////////////////////////////////////////////////////////////////////////
#pragma  region gyRendererFactory::Impl
class gyRendererFactory::Impl
{
public:
  void Destroy();
  int CompileShader( gyShaderByteCode* pByteCode, const gyShaderSourceDesc& sourceDesc );      
  gyShader* CreateShader( gyShaderByteCode* pByteCode );
  static int32_t D3D11CreateEmptyTexture2D( int32_t width, int32_t height, DXGI_FORMAT texf, uint32_t bindflags, ID3D11Texture2D** pOutTex );
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// gyRenderer::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyRenderer::Impl
class gyRenderer::Impl
{
public:
  Impl();
  int Create(gyVarDict& params);
  void Destroy();
  int Query( uint32_t qry, void* output );  

public:
  inline void          D3D11SetDevice( ID3D11Device* pdev ) { pd3dDevice = pdev; }
  inline ID3D11Device* D3D11GetDevice(){ return pd3dDevice; }
  ID3D11DeviceContext* D3D11GetMainContext();

public:
  // format conversion methods
  static uint32_t D3D11SemanticNameToVLSemantic( const char* semanticName );
  static uint32_t D3D11CountComponentMask(uint8_t mask);
  static uint8_t D3D11ComponentTypeToVLFormat( D3D10_REGISTER_COMPONENT_TYPE ctype, BYTE compMask, uint32_t* outsize );
  static DXGI_FORMAT ComponentFormatToD3D11( gyComponentFormat texf );
  static gyComponentFormat D3D11ToComponentFormat( DXGI_FORMAT dxgif );
  static const char* GetShaderProfile(gyShaderType stype, gyShaderModel sm );
  static gyShaderConstanType D3D11DimensionToSCT(D3D10_SRV_DIMENSION c);
  static gyShaderConstanType D3D11TypeToSCT(D3D11_SHADER_TYPE_DESC &c);
  static uint32_t D3D11SignatureDescToVertexElement( D3D11_SIGNATURE_PARAMETER_DESC* pVDesc, gyVertexElement* pElm, uint32_t offsetByte );
  static void VertexElementToD3D11InputElement(const gyVertexElement* pVElm, D3D11_INPUT_ELEMENT_DESC* pIElm);
  static DXGI_FORMAT IndexFormatToD3D11Format( gyIndexType iformat );
  static D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopoToD3D11Topology( gyPrimitiveTopology primTopo );
  static D3D11_TEXTURE_ADDRESS_MODE AddressModeToD3D11TextureAddress( gyAddress a );
  static void D3D11SamplerInfoToDesc( const gySamplerState* ssinfo, D3D11_SAMPLER_DESC* samplerDesc );
  static void D3D11BlendInfoToDesc( const gyBlendStateDesc* bsinfo, D3D11_RENDER_TARGET_BLEND_DESC* blendDesc );
  static D3D11_FILTER FilterTypeToD3D11Filter( gyFilterType filt );
  static D3D11_COMPARISON_FUNC ComparisonFuncToD3D11Comparison( gyComparisonFunc compFun );
  static D3D11_STENCIL_OP StencilOpToD3D11( gyStencilOp op );
  static void D3D11DepthStencilInfoToDesc( const gyDepthStencilState* depthState, D3D11_DEPTH_STENCIL_DESC* depthStencilDesc );
  static void D3D11RasterInfoToDesc( const gyRasterState* rasterState, D3D11_RASTERIZER_DESC* rasterDesc );
protected:
  void SetupViewport();

protected:  
  ID3D11Device* pd3dDevice;
};

gyRenderer::Impl::Impl()
  : pd3dDevice(NULL)
{
}

int gyRenderer::Impl::Create(gyVarDict& params)
{
  return R_OK;
}

void gyRenderer::Impl::Destroy()
{
  SafeRelease(pd3dDevice);
}

int gyRenderer::Impl::Query( uint32_t qry, void* output )
{
  switch ( qry )
  {
  case QRY_BACKBUFFER_DIM: { uint32_t* pdim = (uint32_t*)output; pdim[0]=0; pdim[1]=0; }break;
  case QRY_SIMULTANEOUS_RT_COUNT: { *((uint32_t*)output) = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; }break;
  default: return -1;
  }
  return R_OK;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
static const char* g_szShaderProfilesDX[ST_MAXSTAGES][SM_MAX] =
{
  //                      SM_11   SM_12,      SM_13,    SM_14,   SM_20,     SM_2a,    SM_2b,    SM_30,    SM_40,    SM_41,   SM_40_FEATLEV_91,    SM_40_FEATLEV_93,    SM_50
  /* VertexShader   */  { "vs_1_1", NULL,     NULL,     NULL,     "vs_2_0", "vs_2_a", "vs_2_0", "vs_3_0", "vs_4_0", "vs_4_1", "vs_4_0_level_9_1", "vs_4_0_level_9_3", "vs_5_0" },
  /* HullShader     */  { NULL,    NULL,      NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     NULL,               NULL,               "hs_5_0" },
  /* DomainShader   */  { NULL,    NULL,      NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     NULL,               NULL,               "ds_5_0" },
  /* GeometryShader */  { NULL,    NULL,      NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     "gs_4_0", "gs_4_1", NULL,               NULL,               "gs_5_0" },
  /* PixelShader    */  { "ps_1_1", "ps_1_2", "ps_1_3", "ps_1_4", "ps_2_0", "ps_2_a", "ps_2_b", "ps_3_0", "ps_4_0", "ps_4_1", "ps_4_0_level_9_1", "ps_4_0_level_9_3", "ps_5_0" },
  /* ComputeShader  */  { NULL,    NULL,      NULL,     NULL,     NULL,     NULL,     NULL,     NULL,     "cs_4_0", "cs_4_1", NULL,               NULL,               "cs_5_0" },
};
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
static DXGI_FORMAT  g_dx11Formats[FMT_MAX] = {
  DXGI_FORMAT_UNKNOWN, 
  DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT,
  DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT,
  DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT,
  DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM,
  DXGI_FORMAT_D24_UNORM_S8_UINT };

DXGI_FORMAT gyRenderer::Impl::ComponentFormatToD3D11( gyComponentFormat texf )
{
  return g_dx11Formats[texf];
}

gyComponentFormat D3D11ToComponentFormat( DXGI_FORMAT dxgif )
{
  for (int32_t i = 0; i < FMT_MAX; ++i )
    if ( g_dx11Formats[i] == dxgif ) 
      return (gyComponentFormat)i;
  return FMT_UNKNOW;
}

const char* gyRenderer::Impl::GetShaderProfile(gyShaderType stype, gyShaderModel sm )
{
  return g_szShaderProfilesDX[stype][sm];
}

gyShaderConstanType gyRenderer::Impl::D3D11DimensionToSCT(D3D10_SRV_DIMENSION c)
{
  switch (c)
  {
  case D3D11_SRV_DIMENSION_BUFFER:
  case D3D11_SRV_DIMENSION_TEXTURE1D:
  case D3D11_SRV_DIMENSION_TEXTURE2D:
  case D3D11_SRV_DIMENSION_TEXTURE2DMS:
    return SCT_TEXTURE;
  case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
  case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
  case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
    return SCT_TEXTURE2DARRAY;
  case D3D11_SRV_DIMENSION_TEXTURE3D:
    return SCT_TEXTURE3D;
  case D3D11_SRV_DIMENSION_TEXTURECUBE:
    return SCT_CUBEMAP;
  }
  return SCT_NONE;
}

gyShaderConstanType gyRenderer::Impl::D3D11TypeToSCT(D3D11_SHADER_TYPE_DESC &c)
{
  switch (c.Type)
  {
  case D3D10_SVT_BOOL:
    switch (c.Class)
    {
    case D3D10_SVC_SCALAR:
      return SCT_BOOL;
    case D3D10_SVC_VECTOR:
      if (c.Columns==2) return SCT_BOOL2;
      if (c.Columns==3) return SCT_BOOL3;
      return SCT_BOOL4;
    case D3D10_SVC_MATRIX_ROWS: ///< what to do here?
    case D3D10_SVC_MATRIX_COLUMNS:
      return SCT_BOOL4;
    }
    return SCT_BOOL;
  case D3D10_SVT_INT:
    switch (c.Class)
    {
    case D3D10_SVC_SCALAR:
      return SCT_INT;
    case D3D10_SVC_VECTOR:
      if (c.Columns==2) return SCT_INT2;
      if (c.Columns==3) return SCT_INT3;
      return SCT_INT4;
    case D3D10_SVC_MATRIX_ROWS: ///< what to do here?
    case D3D10_SVC_MATRIX_COLUMNS:
      return SCT_INT4;
    }
    return SCT_INT;
  case D3D10_SVT_FLOAT:
    switch (c.Class)
    {
    case D3D10_SVC_SCALAR:
      return SCT_FLOAT;
    case D3D10_SVC_VECTOR:
      if (c.Columns==2) return SCT_FLOAT2;
      if (c.Columns==3) return SCT_FLOAT3;
      return SCT_FLOAT4;
    case D3D10_SVC_MATRIX_ROWS: ///< what to do here?
    case D3D10_SVC_MATRIX_COLUMNS:
      if ( ((c.Columns==3)) && (c.Rows==4) ) return SCT_FLOAT43;
      if ( (c.Columns==3) && (c.Rows==3) ) return SCT_FLOAT33;
      if ( (c.Columns==4) && (c.Rows==4) ) return SCT_FLOAT44;
      return SCT_NONE;
    }
    return SCT_FLOAT;
  case D3D10_SVT_STRING:
    return SCT_STRING;
  case D3D10_SVT_SAMPLER:
    return SCT_SAMPLER;
  case D3D10_SVT_TEXTURE:
  case D3D10_SVT_TEXTURE1D:
  case D3D10_SVT_TEXTURE2D:
    return SCT_TEXTURE;
  case D3D10_SVT_TEXTURE1DARRAY:
  case D3D10_SVT_TEXTURE2DARRAY:
    return SCT_TEXTURE2DARRAY;
  case D3D10_SVT_TEXTURE3D:
    return SCT_TEXTURE3D;
  case D3D10_SVT_TEXTURECUBE:
    return SCT_CUBEMAP;  
  }
  return SCT_NONE;
}

uint32_t gyRenderer::Impl::D3D11SemanticNameToVLSemantic( const char* semanticName )
{
  FAIL_IF( !semanticName || !*semanticName, "Invalid semantic name" );
  if ( !_strnicmp(semanticName,"POSITION",8) || !_strnicmp(semanticName,"SV_Position",11) )
    return ( toupper(semanticName[8]) == 'T' ) ? VES_POSITIONTRANSFORMED : VES_POSITION;
  else if ( !_strnicmp(semanticName,"NORMAL",6) )
    return VES_NORMAL;
  else if ( !_strnicmp(semanticName,"COLOR",5) || !_strnicmp(semanticName,"SV_Target",9 ) )
    return VES_COLOR;
  else if ( !_strnicmp(semanticName,"BINORMAL",8) )
    return VES_BINORMAL;
  else if ( !_strnicmp(semanticName,"TANGENT",7) )
    return VES_TANGENT;
  else if ( !_strnicmp(semanticName,"TEXCOORD",8) )
    return VES_TEXCOORD;
  return VES_UNKNOWN;
}

const char* gsz_semanticNames[VES_MAX] = { "Unknown", "POSITION", "POSITIONT", "NORMAL", "COLOR", "BINORMAL", "TANGENT", "TEXCOORD" };

const char* VLSemanticTypeToD3D11Name(uint32_t semantic)
{
  return gsz_semanticNames[semantic];
}

uint32_t gyRenderer::Impl::D3D11CountComponentMask(uint8_t mask)
{
  switch ( mask )
  {
  case 1 : return 1;
  case 3 : return 2;
  case 7 : return 3;
  case 15: return 4;
  }
  return R_OK;
}

uint8_t gyRenderer::Impl::D3D11ComponentTypeToVLFormat( D3D10_REGISTER_COMPONENT_TYPE ctype, BYTE compMask, uint32_t* outsize )
{
  uint32_t noOfComponents = D3D11CountComponentMask( (uint8_t)compMask);
  *outsize = noOfComponents*4;
  switch (ctype)
  {
  case D3D10_REGISTER_COMPONENT_UNKNOWN: return TYPE_UNKNOW;
  case D3D10_REGISTER_COMPONENT_UINT32 : return uint8_t(FMT_R32_UI+noOfComponents-1); 
  case D3D10_REGISTER_COMPONENT_SINT32 : return uint8_t(FMT_R32_SI+noOfComponents-1);
  case D3D10_REGISTER_COMPONENT_FLOAT32: return uint8_t(FMT_R32_F+noOfComponents-1);
  }
  return (uint8_t)TYPE_UNKNOW;
}

uint32_t gyRenderer::Impl::D3D11SignatureDescToVertexElement( D3D11_SIGNATURE_PARAMETER_DESC* pVDesc, gyVertexElement* pElm, uint32_t offsetByte )
{
  uint32_t compSize=0;
  pElm->streamIndex(pVDesc->Stream);
  pElm->semanticIndex(pVDesc->SemanticIndex);
  pElm->semantic( D3D11SemanticNameToVLSemantic( pVDesc->SemanticName ) );
  pElm->format( D3D11ComponentTypeToVLFormat(pVDesc->ComponentType,pVDesc->Mask,&compSize) );
  pElm->offsetByte( offsetByte );
  return compSize;
}

void gyRenderer::Impl::VertexElementToD3D11InputElement(const gyVertexElement* pVElm, D3D11_INPUT_ELEMENT_DESC* pIElm)
{
  pIElm->SemanticName = VLSemanticTypeToD3D11Name( pVElm->semantic() );
  pIElm->SemanticIndex= pVElm->semanticIndex();
  pIElm->Format = ComponentFormatToD3D11( (gyComponentFormat)pVElm->format() );
  pIElm->InputSlot = pVElm->streamIndex();
  pIElm->AlignedByteOffset = pVElm->offsetByte();
  pIElm->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  pIElm->InstanceDataStepRate = 0;
}

DXGI_FORMAT gyRenderer::Impl::IndexFormatToD3D11Format( gyIndexType iformat )
{
  DXGI_FORMAT mapTable[INDEX_MAX] = { DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R32_UINT };
  return mapTable[iformat];
}

D3D11_PRIMITIVE_TOPOLOGY gyRenderer::Impl::PrimitiveTopoToD3D11Topology( gyPrimitiveTopology primTopo )
{
  D3D11_PRIMITIVE_TOPOLOGY mapTable[PT_MAX] = { D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
  return mapTable[primTopo];
}

D3D11_TEXTURE_ADDRESS_MODE gyRenderer::Impl::AddressModeToD3D11TextureAddress( gyAddress a )
{
  D3D11_TEXTURE_ADDRESS_MODE mapTable[AD_MAX] = { D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_MIRROR,
    D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE };
  return mapTable[a];
}

D3D11_FILTER gyRenderer::Impl::FilterTypeToD3D11Filter( gyFilterType filt )
{
  D3D11_FILTER mapTable[FT_MAX] = { D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT, D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR, D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT, D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_FILTER_ANISOTROPIC};

  return mapTable[filt];
}

D3D11_COMPARISON_FUNC gyRenderer::Impl::ComparisonFuncToD3D11Comparison( gyComparisonFunc compFun ) 
{
  D3D11_COMPARISON_FUNC mapTable[COMP_MAX] = {D3D11_COMPARISON_NEVER, D3D11_COMPARISON_NEVER, D3D11_COMPARISON_LESS, D3D11_COMPARISON_EQUAL, D3D11_COMPARISON_LESS_EQUAL,
    D3D11_COMPARISON_GREATER, D3D11_COMPARISON_NOT_EQUAL, D3D11_COMPARISON_GREATER_EQUAL, D3D11_COMPARISON_ALWAYS};

  return mapTable[compFun];
}

D3D11_STENCIL_OP gyRenderer::Impl::StencilOpToD3D11( gyStencilOp op )
{
  D3D11_STENCIL_OP mapTable[SOP_MAX]={ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_ZERO, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_INCR_SAT,
    D3D11_STENCIL_OP_DECR_SAT, D3D11_STENCIL_OP_INVERT,D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_DECR };
  return mapTable[op];
}

void gyRenderer::Impl::D3D11SamplerInfoToDesc( const gySamplerState* ssinfo, D3D11_SAMPLER_DESC* samplerDesc )
{
  gyAddress u,v,w;
  ssinfo->GetAddresses(&u,&v,&w);
  samplerDesc->AddressU = AddressModeToD3D11TextureAddress( u );
  samplerDesc->AddressV = AddressModeToD3D11TextureAddress( v );
  samplerDesc->AddressW = AddressModeToD3D11TextureAddress( w );
  samplerDesc->Filter = FilterTypeToD3D11Filter( ssinfo->GetFilter() );
  samplerDesc->MipLODBias = ssinfo->GetMipLODBias();
  samplerDesc->MaxAnisotropy = ssinfo->GetMaxAniso();
  samplerDesc->ComparisonFunc = ComparisonFuncToD3D11Comparison( ssinfo->GetCompFunction() );
  ssinfo->GetBorderColor( samplerDesc->BorderColor );
  ssinfo->GetLOD( &samplerDesc->MinLOD, &samplerDesc->MaxLOD );
}

void gyRenderer::Impl::D3D11BlendInfoToDesc( const gyBlendStateDesc* bsinfo, D3D11_RENDER_TARGET_BLEND_DESC* blendDesc )
{
  blendDesc->BlendEnable = TRUE;
  blendDesc->SrcBlend = (D3D11_BLEND)bsinfo->Src();
  blendDesc->DestBlend= (D3D11_BLEND)bsinfo->Dst();
  blendDesc->BlendOp = (D3D11_BLEND_OP)bsinfo->Op();
  blendDesc->SrcBlendAlpha = (D3D11_BLEND)bsinfo->SrcA();
  blendDesc->DestBlendAlpha= (D3D11_BLEND)bsinfo->DstA();
  blendDesc->BlendOpAlpha = (D3D11_BLEND_OP)bsinfo->OpA();
  blendDesc->RenderTargetWriteMask = (UINT8)bsinfo->RTWriteMask();
}
 
void gyRenderer::Impl::D3D11DepthStencilInfoToDesc( const gyDepthStencilState* depthState, D3D11_DEPTH_STENCIL_DESC* depthStencilDesc )
{
  depthStencilDesc->DepthEnable = depthState->GetDepthTestEnabled();
  depthStencilDesc->DepthWriteMask = depthState->GetDepthWriteEnabled() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
  depthStencilDesc->DepthFunc = ComparisonFuncToD3D11Comparison( depthState->GetDepthFunc() );

  depthStencilDesc->StencilEnable = depthState->GetStencilTest();
  depthStencilDesc->StencilReadMask = depthState->GetStencilReadMask();
  depthStencilDesc->StencilWriteMask = depthState->GetStencilWriteMask();

  gyStencilOp op,dop,pop;
  gyComparisonFunc f;
  for ( int i = 0; i < 2; ++i )
  {
    D3D11_DEPTH_STENCILOP_DESC* pDOp = i==0?&depthStencilDesc->FrontFace:&depthStencilDesc->BackFace;
    depthState->GetStencilOp( (gyDepthStencilState::eFace)i, &op, &dop, &pop, &f );
    pDOp->StencilFailOp = StencilOpToD3D11( op );
    pDOp->StencilDepthFailOp = StencilOpToD3D11( dop );
    pDOp->StencilPassOp = StencilOpToD3D11( pop );
    pDOp->StencilFunc = ComparisonFuncToD3D11Comparison(f);
  }
}

void gyRenderer::Impl::D3D11RasterInfoToDesc( const gyRasterState* rasterState, D3D11_RASTERIZER_DESC* rasterDesc )
{
  uint8_t flags = rasterState->GetFlags();
  rasterDesc->AntialiasedLineEnable = (flags & RASTER_AALINE_ENABLED) != 0;
  rasterDesc->DepthClipEnable = (flags & RASTER_DEPTHCLIP_ENABLED) != 0;
  rasterDesc->FrontCounterClockwise = (flags & RASTER_COUNTERCLOCKWISE) != 0;
  rasterDesc->MultisampleEnable = (flags & RASTER_MULTISAMPLE_ENABLED) != 0;
  rasterDesc->ScissorEnable = (flags & RASTER_SCISSOR_ENABLED) != 0;
  rasterDesc->CullMode = (D3D11_CULL_MODE)rasterState->GetCullMode();
  rasterDesc->FillMode = (D3D11_FILL_MODE)rasterState->GetFillMode();
  rasterDesc->DepthBias = rasterState->GetDepthBias();
  rasterDesc->DepthBiasClamp = rasterState->GetDepthBiasClamp();
  rasterDesc->SlopeScaledDepthBias = rasterState->GetSlopeScaledDepthBias();
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
// DX11CBuffer
//////////////////////////////////////////////////////////////////////////
#pragma region DX11CBuffer
// each buffer has a mirror in system memory, used by cpu.
// -> double buffering, updating the driver buffer only where and when needed
struct DX11CBuffer
{
  DX11CBuffer():d3dBuffer(0), memBuffer(0), sizeInBytes(0), index(-1), dirty(false){}

  int32_t Create();
  void  Release();
  void  Flush(ID3D11DeviceContext* pContext);

  ID3D11Buffer* d3dBuffer;
  float* memBuffer; // sys memory buffer
  uint32_t sizeInBytes;
  int32_t index;
  bool  dirty; // true when the driver/device buffer (d3dBuffer) needs to be updated with the sys mem buffer (memBuffer)
};
// *******************************************************************************************************************
// *******************************************************************************************************************
int32_t DX11CBuffer::Create()
{
  // creating the device buffer
  D3D11_BUFFER_DESC bd;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.Usage = D3D11_USAGE_DYNAMIC;
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  bd.MiscFlags = 0;
  bd.StructureByteStride = 0;
  bd.ByteWidth = sizeInBytes;
  HRESULT hr = D3D11DEVICE->CreateBuffer( &bd, NULL, &d3dBuffer );
  R_FAIL_IF( FAILED(hr), "Cannot create device buffer" );

  // creating the sys mem buffer
  memBuffer = (float*)calloc(sizeInBytes,1);
  R_FAIL_IF( !memBuffer, "Cannot create mem buffer" );
  return R_OK;
}
// --------------------------------------------------------------------------------------------------
void DX11CBuffer::Release()
{
  SafeRelease(d3dBuffer);
  SafeFree(memBuffer);
}
// --------------------------------------------------------------------------------------------------
void DX11CBuffer::Flush(ID3D11DeviceContext* pContext)
{
  if ( !dirty ) return;
  //pContext->UpdateSubresource( d3dBuffer, 0, NULL, memBuffer, 0, 0 );
  D3D11_MAPPED_SUBRESOURCE mapped;
  HRESULT hr = pContext->Map( d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
  if ( SUCCEEDED(hr) )
  {
    memcpy( mapped.pData, memBuffer, this->sizeInBytes ); // alignment/pitch issues?
    pContext->Unmap(d3dBuffer, 0);
  }
  dirty = false;
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyShaderByteCode::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyShaderByteCode::Impl
class gyShaderByteCode::Impl
{
public:
  Impl():pShaderBlob(NULL){ }
  void Destroy(){ SafeRelease(pShaderBlob); }
  uint8_t* GetByteCode(){ return (uint8_t*)pShaderBlob->GetBufferPointer(); }
  uint32_t GetLength(){ return (uint32_t)pShaderBlob->GetBufferSize(); }

  ID3DBlob* pShaderBlob;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyRenderTarget::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyRenderTarget::Impl
class gyRenderTarget::Impl
{
public:
  Impl():pRTTexture(0), pRTView(0), pSRView(0), pRTDSTexture(0), pRTDSView(0)
  {}
  void Destroy()
  {
    SafeRelease( pRTView );
    SafeRelease( pRTTexture );
    SafeRelease( pSRView );
    SafeRelease( pRTDSTexture );
    SafeRelease( pRTDSView );
  }

  int Init( int32_t width, int32_t height, gyComponentFormat texf, uint32_t flags )
  {
    HRESULT result;
    bool asDepth = FlagIsSet(flags,RT_CREATE_DEPTH_STENCIL);
    DXGI_FORMAT texFormat = gyRenderer::Impl::ComponentFormatToD3D11(texf);
    ID3D11Device* pd3dDev = D3D11DEVICE;

    // create the texture 2d for the target view if set
    if ( FlagIsSet(flags,RT_CREATE_TEXTURE_2D) )
    {
      unsigned int bindFlags = D3D11_BIND_RENDER_TARGET;
      if ( FlagIsSet(flags,RT_USED_AS_SHADER_TEXTURE) )
        bindFlags |= D3D11_BIND_SHADER_RESOURCE;
      if ( gyRendererFactory::Impl::D3D11CreateEmptyTexture2D( width, height, texFormat, bindFlags, &pRTTexture ) != 0 )
        GYDEBUGRET( "Cannot create texture 2D\n", NULL );
    }else if ( !pRTTexture && !asDepth )
    {
      GYDEBUGRET( "RT_CREATE_TEXTURE_2D not specified, but no texture 2D provided\n", -1);
    }

    // Create the render target view (uses the texture as render target)
    if ( pRTTexture )
    {
      result = pd3dDev->CreateRenderTargetView( pRTTexture, NULL, &pRTView);
      if( FAILED(result) )
        GYDEBUGRET( "Cannot create Render Target view\n", -1 );
    }

    // Create the shader resource view.
    if ( FlagIsSet(flags,RT_USED_AS_SHADER_TEXTURE) )
    {
      if ( FAILED(pd3dDev->CreateShaderResourceView( pRTTexture, NULL, &pSRView)) )
        GYDEBUGRET( "Cannot create Shader Resource View\n", -1 );
    }

    if ( FlagIsSet(flags,RT_CREATE_DEPTH_STENCIL) )
    {
      if ( gyRendererFactory::Impl::D3D11CreateEmptyTexture2D( width, height, texFormat, D3D11_BIND_DEPTH_STENCIL, &pRTDSTexture ) != 0 )
        GYDEBUGRET( "Cannot create texture 2d for depth stencil\n", -1 );

      if ( FAILED(pd3dDev->CreateDepthStencilView( pRTDSTexture, NULL, &pRTDSView )) )
        GYDEBUGRET( "Cannot create depth stencil view\n", -1 );
    }
    return R_OK;
  }

  ID3D11Texture2D*           pRTTexture;
  ID3D11RenderTargetView*    pRTView;
  ID3D11ShaderResourceView*  pSRView;
  ID3D11Texture2D*           pRTDSTexture;
  ID3D11DepthStencilView*    pRTDSView;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyRenderCommandBuffer::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyRenderCommandBuffer::Impl
class gyRenderCommandBuffer::Impl
{
public:
  Impl();
  ~Impl();  
  int Create(gyRenderCommandBuffer& rcb, int width, int height, bool fullscreen);

  void Destroy( );
  void Present();
  int32_t SetDepthTarget( gyIDRenderTarget rtDepth );
  int32_t SetRenderTarget( gyIDRenderTarget* allRts, gyIDRenderTarget stOMDepthTargetID, uint32_t startSlot, int32_t count );
  void SetMainRenderTarget( gyRenderTarget* pMainRT);
  void SetViewport( float left, float top, float width, float height );
  void ClearRenderTarget( gyRenderTarget* pRTDx, uint32_t flags, float* rgba, float depth, uint8_t stencil );
  void SetInputLayout(gyVertexLayout* pVL, gyShader* pVS);

  void SetMeshBuffers( gyIDMeshBuffer* meshBufferIDs, gyPrimitiveTopology& primTopo, uint32_t start, uint32_t count );
  void SetDepthStencilState( gyDepthStencilState* pDSS, uint32_t stencilRef );
  void SetRasterState( gyRasterState* pRS);
  void SetBlendState( gyBlendState* pBS );
  void SetVSShaderCB( gyShader* pVS );
  void SetVSShader( gyShader* pVS );
  void SetPSSampler( gySamplerState* pSampler, uint32_t slot );
  void SetPSTexture( gyTexture* tex, uint32_t slot );
  void SetPSShaderCBConstants( gyShader* pPS);
  void SetPSShader( gyShader* pPS );
  void Draw(gyMeshBuffer* mb);

  int32_t D3D11CreateImmediateContext( gyRenderCommandBuffer& rcb, HWND hWnd, int width, int height, bool windowed );                          // only for immediate context
  ID3D11DeviceContext* D3D11GetDeviceContext(){ return pDevContext; }
  IDXGISwapChain* D3D11GetSwapChain(){ return pSwapChain; }               // only for immediate context
  void UnboundAllPSResources( );  

  int32_t CreateMainRenderTarget(gyRenderCommandBuffer& rcb, uint32_t width, uint32_t height);          // only for immediate context
  int32_t ValidateVSInputLayout();

private:
  ID3D11DeviceContext*  pDevContext;
  IDXGISwapChain*  pSwapChain;                              // only for immediate context
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyShaderConstantBuffer::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyShaderConstantBuffer::Impl
class gyShaderConstantBuffer::Impl
{
public:  
  Impl(gyShaderConstantBuffer* srcCBuff) 
    : srcCBuff(srcCBuff), arrayOfDX11CBuffers(NULL), cbs(NULL), cbsCount(0)
  {}
  void Destroy();
  int32_t SetConstantValue( int32_t constantNdx, const float* values, uint32_t count );
  int32_t SetConstantValue( int32_t constantNdx, const bool* values, uint32_t count );
  int32_t SetConstantValue( int32_t constantNdx, const int* values, uint32_t count );
  int32_t SetConstantValue( int32_t constantNdx, gyIDTexture tex, uint32_t count=1 );
  int32_t SetConstantValue( int32_t constantNdx, gyIDSamplerState, uint32_t count=1 );

  int32_t D3D11FillFromReflector( ID3D11ShaderReflection* pReflector, uint32_t noShaderCBs );
  void D3D11FlushBuffers( gyRenderCommandBuffer::Impl* pContext );

  gyShaderConstantBuffer* srcCBuff;
  ID3D11Buffer** arrayOfDX11CBuffers; // just a convenient array
  DX11CBuffer*  cbs;
  uint32_t  cbsCount;
};

void gyShaderConstantBuffer::Impl::Destroy()
{
  for ( uint32_t i = 0; i < cbsCount; ++i )
    cbs[i].Release();
  SafeDeleteA(cbs);
  SafeDeleteA(arrayOfDX11CBuffers);
}

int32_t gyShaderConstantBuffer::Impl::D3D11FillFromReflector( ID3D11ShaderReflection* pReflector, uint32_t noShaderCBs )
{
  R_FAIL_IF( !pReflector, "Invalid reflector" );
  R_FAIL_IF( !srcCBuff->constants || !srcCBuff->constantsCount, "No constant table allocated" );
  // ************************* first pass, only TEXTURES and SAMPLERS at the beginning of array *************************
  // general info
  D3D11_SHADER_DESC shDesc;
  pReflector->GetDesc(&shDesc);

  uint32_t noCopied = 0;  // store the actual number of constants retrieved. should be == constantsCount at the end
  // filling our constants table
  for ( uint32_t i = 0; i < shDesc.BoundResources; ++i )
  {
    D3D11_SHADER_INPUT_BIND_DESC resDesc;
    pReflector->GetResourceBindingDesc(i,&resDesc);    
    // we reflect the textures and samplers only
    if ( resDesc.Type != D3D10_SIT_TEXTURE && resDesc.Type != D3D10_SIT_SAMPLER ) continue;

#ifdef _DEBUG
    strncpy_s( srcCBuff->constants[noCopied].name, resDesc.Name, gyShaderConstant::MAX_CONSTANT_NAME );
    srcCBuff->constants[noCopied].name[gyShaderConstant::MAX_CONSTANT_NAME-1] = 0;
#endif
    srcCBuff->constants[noCopied].nameHash = gyComputeHash( (void*)resDesc.Name, (uint32_t)strlen(resDesc.Name) );
    srcCBuff->constants[noCopied].index = resDesc.BindPoint;
    srcCBuff->constants[noCopied].size = (int8_t)resDesc.BindCount;
    // is it a texture unit or a sampler state?
    srcCBuff->constants[noCopied].type = resDesc.Type == D3D10_SIT_TEXTURE
      ? srcCBuff->constants[noCopied].type = (int8_t)gyRenderer::Impl::D3D11DimensionToSCT(resDesc.Dimension)
      : SCT_SAMPLER;
    R_FAIL_IF( srcCBuff->constants[noCopied].IsInvalid(), "Invalid constant info copied" );
    ++noCopied;
  } // for

  // ************************* second pass, rest of VARIABLES *************************
  // create internal dx11 c.buffers array
  if ( noShaderCBs )
  {
    cbsCount = noShaderCBs;
    cbs = new DX11CBuffer[noShaderCBs];
    // helper array for batch d3d11 api calls
    arrayOfDX11CBuffers = new ID3D11Buffer*[noShaderCBs];
    ZeroMemory( arrayOfDX11CBuffers, PtrSize*noShaderCBs);
  }
  uint32_t cbIndex = 0;   // current internal cb index
  for ( uint32_t i = 0; i < shDesc.BoundResources; ++i )
  {
    D3D11_SHADER_INPUT_BIND_DESC resDesc;
    pReflector->GetResourceBindingDesc(i,&resDesc);    
    if ( resDesc.Type != D3D10_SIT_CBUFFER ) continue;
    // getting CB and its desc
    ID3D11ShaderReflectionConstantBuffer* pCB = pReflector->GetConstantBufferByName(resDesc.Name);
    D3D11_SHADER_BUFFER_DESC bufDesc; pCB->GetDesc(&bufDesc);
    // internal cbuffers array
    cbs[cbIndex].index = resDesc.BindPoint; // the real shader bind point index
    cbs[cbIndex].sizeInBytes = bufDesc.Size;// constant buffer size
    // filling each variable
    for ( uint32_t v = 0; v < bufDesc.Variables; ++v )
    {
      ID3D11ShaderReflectionVariable* pV = pCB->GetVariableByIndex(v);
      D3D11_SHADER_VARIABLE_DESC varDesc; pV->GetDesc(&varDesc);
      ID3D11ShaderReflectionType* pVT = pV->GetType();
      D3D11_SHADER_TYPE_DESC typeDesc; pVT->GetDesc(&typeDesc);
#ifdef _DEBUG
      strncpy_s( srcCBuff->constants[noCopied].name, varDesc.Name, gyShaderConstant::MAX_CONSTANT_NAME );
      srcCBuff->constants[noCopied].name[gyShaderConstant::MAX_CONSTANT_NAME-1] = 0;
#endif
      srcCBuff->constants[noCopied].nameHash = gyComputeHash( (void*)varDesc.Name, (uint32_t)strlen(varDesc.Name) );
      srcCBuff->constants[noCopied].index = (cbIndex << 24) | (varDesc.StartOffset&0x00ffffff); // 8bits for internal cb index and 24bits for offset inside
      srcCBuff->constants[noCopied].type = (int8_t)gyRenderer::Impl::D3D11TypeToSCT(typeDesc);
      srcCBuff->constants[noCopied].size = (int8_t)varDesc.Size;
      ++noCopied;          
    }
    ++cbIndex;
    R_FAIL_IF( srcCBuff->constants[noCopied-1].IsInvalid(), "Invalid constant info copied" );
  }//for

  // ****************************************************************************
  R_FAIL_IF( srcCBuff->constantsCount != noCopied, "No. of constants copied != allocated ones" );

  // create internal D3D11 buffers for the CBs (noInternalCBs numbers), if proceed
  for ( uint32_t i = 0; i < cbsCount; ++i )
  {
    if ( cbs[i].Create( ) != 0 )
      GYDEBUGRET( "Error creating DX11CBuffer\n", -1 );
    arrayOfDX11CBuffers[i] = cbs[i].d3dBuffer; // just a cache of pointers for convenient dx11 api calls
  }
  return R_OK;
}
// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
int32_t gyShaderConstantBuffer::Impl::SetConstantValue( int32_t constantNdx, const float* values, uint32_t count )
{
  if ( constantNdx == -1 ) return -1;
  R_FAIL_IF( !values || !count, "Invalid values or count" );
  R_FAIL_IF((uint32_t)constantNdx>=srcCBuff->constantsCount, "Index out of range");
  gyShaderConstant& constant = srcCBuff->constants[constantNdx];
  if ( !constant.IsFloatArray() )
    GYDEBUGRET( "Trying to set float values to an invalid constant", -1 );

  uint32_t cbsIndex = (constant.index&0xff000000)>>24;
  uint32_t regIndex = (constant.index&0x00ffffff);
  R_FAIL_IF( cbsIndex >= cbsCount, "Invalid internal constant buffer index" );

  DX11CBuffer& dxcbuf = cbs[cbsIndex];

  // update the system mem buffer in dxcbuf and marks it as dirty
  // - first have to compute the offset in the buffer taking in to account the regIndex
  uint32_t memoffset = regIndex>>2; // div by 4 because is the number of components for each register in the CB
  FAIL_IF( (memoffset + sizeof(float)*count) > dxcbuf.sizeInBytes, "Count causes buffer overflow" );
  // makes the copy to the dst. buffer
  float* pMem = dxcbuf.memBuffer+memoffset;
  for ( uint32_t i = 0; i < count; ++i ) pMem[i] = values[i]; // would it be faster by using memcpy? (count<=4 most of the times)
  dxcbuf.dirty = true;

  return R_OK;
}
// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
int32_t gyShaderConstantBuffer::Impl::SetConstantValue( int32_t constantNdx, const bool* values, uint32_t count )
{
  GYUNUSED(constantNdx);
  GYUNUSED(values);
  GYUNUSED(count);
  FAIL_ALWAYS( "NOT IMPLEMENTED" );
  return R_OK;
}
// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
int32_t gyShaderConstantBuffer::Impl::SetConstantValue( int32_t constantNdx, const int* values, uint32_t count )
{
  GYUNUSED(constantNdx);
  GYUNUSED(values);
  GYUNUSED(count);
  FAIL_ALWAYS( "NOT IMPLEMENTED" );
  return R_OK;
}
// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
int32_t gyShaderConstantBuffer::Impl::SetConstantValue( int32_t constantNdx, gyIDTexture tex, uint32_t count/*=1*/ )
{
  if ( constantNdx == -1 ) return -1;
  R_FAIL_IF( !count, "Invalid count" );
  gyShaderConstant& constant = srcCBuff->constants[constantNdx];
  if ( !constant.IsTexture() )
    GYDEBUGRET( "Trying to set texture to an invalid constant", -1 )

    // cache renderer object
    constant.resNumber = tex.Number();
  return R_OK;
}
// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
int32_t gyShaderConstantBuffer::Impl::SetConstantValue( int32_t constantNdx, gyIDSamplerState sid, uint32_t count/*=1*/ )
{
  if ( constantNdx == -1 ) return -1;
  R_FAIL_IF( !count, "Invalid count" );
  gyShaderConstant& constant = srcCBuff->constants[constantNdx];
  if ( !constant.IsSampler() )
    GYDEBUGRET( "Trying to set a sampler to an invalid constant", -1 )

    // caches renderer object
    constant.resNumber = sid.Number();
  return R_OK;
}

// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
void gyShaderConstantBuffer::Impl::D3D11FlushBuffers(gyRenderCommandBuffer::Impl* pcmdBuff)
{
  for ( uint32_t i = 0; i < cbsCount; ++i )
    cbs[i].Flush( pcmdBuff->D3D11GetDeviceContext() );
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyVertexLayout::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyVertexLayout::Impl
class gyVertexLayout::Impl
{
public:
  Impl():pD3D11InputLayout(0){}
  void Destroy(){ SafeRelease(pD3D11InputLayout); }
  int32_t Init(gyVertexLayout& vl, gyShaderByteCode* pByteCode)
  {
    return D3D11CreateInternalInterface(vl,pByteCode);
  }

  void Ensure(gyVertexLayout& vl,gyShaderByteCode* pByteCode){ D3D11CreateInternalInterface(vl,pByteCode); }

  int32_t D3D11CreateInternalInterface(gyVertexLayout& vl, gyShaderByteCode* pByteCode )
  {
    if ( !pByteCode || pD3D11InputLayout )
      return R_OK;

    // temporary creation of d3d11 input element descriptors array
    D3D11_INPUT_ELEMENT_DESC inputElements[128];

    for ( uint32_t i = 0; i < vl.GetElementCount(); ++i )
      gyRenderer::Impl::VertexElementToD3D11InputElement( &vl.elements[i], &inputElements[i] );

    // creating the device interface object
    ID3D11Device* pDev=D3D11DEVICE;
    HRESULT hr = pDev->CreateInputLayout(inputElements,vl.count,pByteCode->GetByteCode(), pByteCode->GetLength(),&pD3D11InputLayout);
    if ( FAILED(hr) )
      GYDEBUGRET( "Error creating device Input Layout", hr );
    return R_OK;
  }

  ID3D11InputLayout* pD3D11InputLayout;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyMeshBuffer::Impl
//////////////////////////////////////////////////////////////////////////
class gyMeshBuffer::Impl
{
public:
  gyMeshBuffer::Impl(): vb(0), ib(0), vbCount(0), ibCount(0)
  {
  }

  void Destroy()
  {
    SafeRelease(vb);
    SafeRelease(ib);
  }

  int Init( gyMeshBuffer& mb )
  {
    return R_OK;
  }

  int CreateVertices( gyMeshBuffer& mb, uint32_t count, void* initData, uint32_t vertexStride)
  {
    if ( vb ) 
      GYDEBUGRET( "Vertices already exists\n", -1 );

    SafeRelease(vb);
    // if no stride passed, compute it from the vlayout
    if ( vertexStride == 0 )
    {
      gyVertexLayout* pVL = gyGetRendererFactory().LockVertexLayout( mb.GetVertexLayout() );
      R_FAIL_IF( !pVL, "Retrieved vertex layout is null" );
      vertexStride = pVL->GetSizeInBytes();
    }

    R_FAIL_IF( vertexStride == 0, "Invalid vertex size\n" );
    mb.vertexStride = vertexStride;

    mb.vertexCount = count;
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = vertexStride * count;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA* pInitData = NULL;
    D3D11_SUBRESOURCE_DATA id;
    if ( initData )
    {
      pInitData = &id;
      pInitData->SysMemPitch = 0;
      pInitData->SysMemSlicePitch = 0;
      pInitData->pSysMem = initData;
    }
    HRESULT hr = D3D11DEVICE->CreateBuffer( &bd, pInitData, &vb );
    if ( FAILED(hr) )
      GYDEBUGRET( "Error creating VB\n", hr );

    return R_OK;
  }

  int CreateIndices( gyMeshBuffer& mb, uint32_t count, void* initData )
  {
    if ( ib )
      GYDEBUGRET( "Indices already exists\n", -1 );
    SafeRelease(ib);

    mb.indexCount = count;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd,sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = ( mb.GetIndexFormat()==INDEX_16?2:4 ) * count;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA* pInitData = NULL;
    D3D11_SUBRESOURCE_DATA id;
    if ( initData )
    {
      pInitData = &id;
      pInitData->SysMemPitch = 0;
      pInitData->SysMemSlicePitch = 0;
      pInitData->pSysMem = initData;
    }
    HRESULT hr = D3D11DEVICE->CreateBuffer( &bd, pInitData, &ib );
    if ( FAILED(hr) )
      GYDEBUGRET( "Error creating IB\n", hr );
    return R_OK;
  }

  int FillVertices( gyMeshBuffer& mb, uint32_t count, void* data )
  {
    if ( !vb ) return -1;
    D3D11_BOX updateBox = { 0, 0, 0, mb.GetVertexStride()*count, 1, 1 };
    gyGetRenderer().GetImpl()->D3D11GetMainContext()->UpdateSubresource( vb, 0, &updateBox, data, 0, 0 );
    return R_OK;
  }

  int FillIndices( gyMeshBuffer& mb, uint32_t count, void* data )
  {
    if ( !ib ) return -1;
    D3D11_BOX updateBox = { 0, 0, 0, mb.GetIndexFormat()*2*count, 1, 1 };
    gyGetRenderer().GetImpl()->D3D11GetMainContext()->UpdateSubresource( ib, 0, &updateBox, data, 0, 0 );
    return R_OK;
  }

  int MapVertices( void** pData )
  {
    if ( !vb ) return -1;
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = gyGetRenderer().GetImpl()->D3D11GetMainContext()->Map( vb, 0, D3D11_MAP_WRITE, 0, &mapped );
    if ( FAILED(hr) )
      return -1;
    *pData = mapped.pData;
    return R_OK;
  }

  void UnmapVertices()
  {
    gyGetRenderer().GetImpl()->D3D11GetMainContext()->Unmap( vb, 0 );
  }

  inline DXGI_FORMAT D3D11GetIndexFormat(const gyMeshBuffer& mb){ return gyRenderer::Impl::IndexFormatToD3D11Format(mb.GetIndexFormat()); }
  inline D3D11_PRIMITIVE_TOPOLOGY D3D11GetPrimitiveTopology(const gyMeshBuffer& mb){ return gyRenderer::Impl::PrimitiveTopoToD3D11Topology( mb.GetPrimitiveTopology() ); }

public:
  ID3D11Buffer*  vb;
  ID3D11Buffer*  ib;
  uint32_t vbCount;
  uint32_t ibCount;
};

//////////////////////////////////////////////////////////////////////////
// gyShaderDX11
//////////////////////////////////////////////////////////////////////////
#pragma region gyShaderDX11

template<typename T>
class gyShaderDX11 : public gyShader
{
public:
  gyShaderDX11(gyShaderType st=ST_NONE, gyShaderModel sm=SM_NONE)
    : gyShader(st,sm), pIShader(NULL)
  { 
  }
  ~gyShaderDX11()
  {
    SafeRelease(pIShader); 
    gyGetRendererFactory().ReleaseResource(inputLayoutID);
  }

  gy_override int32_t Init( gyShaderByteCode* pByteCode )
  {
    gyShader::Init(pByteCode);
    ID3D11ShaderReflection* pReflector;
    HRESULT hr = D3DReflect( pByteCode->GetByteCode(), pByteCode->GetLength(), IID_ID3D11ShaderReflection, (void**)&pReflector );
    if ( FAILED(hr) )
      GYDEBUGRET( "Cannot Reflect shader\n", -1 );  

    if ( CreateInternal(pByteCode,pReflector) != 0 )
      GYDEBUGRET( "Cannot create internal\n", -1 );

    SafeRelease(pReflector);
    return R_OK;
  }

  int32_t D3D11ValidateInputLayout( gyIDVertexLayout il )
  {
    FAIL_IF( !il.IsValid(), "Invalid input layout" );
    if ( inputLayoutID == il ) return R_OK;
    gyVertexLayout* pMine = gyGetRendererFactory().LockVertexLayout(inputLayoutID); 
    gyVertexLayout* pOther= (VertexLayoutDX11*)gyGetRendererFactory().LockVertexLayout(il);

    FAIL_IF( !pMine || !pOther, "Invalid" );
    return pMine->GetHashValue() - pOther->GetHashValue();
  }
public:
  T* D3D11GetInterface(){ return pIShader; }
  static const char* D3D11GetShaderProfile(gyShaderType stype, gyShaderModel sm );

protected:
  int32_t CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector );
  int32_t Reflect( ID3D11ShaderReflection* pReflector ); //extract info from shader
  int32_t ReflectInputLayout( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector );

protected:
  T* pIShader;
  gyIDVertexLayout inputLayoutID; // only makes sense for Vertex Shaders so far
};

typedef gyShaderDX11<ID3D11VertexShader>   gyShaderDX11VS;
typedef gyShaderDX11<ID3D11HullShader>     gyShaderDX11HS;
typedef gyShaderDX11<ID3D11DomainShader>   gyShaderDX11DS;
typedef gyShaderDX11<ID3D11GeometryShader> gyShaderDX11GS;
typedef gyShaderDX11<ID3D11PixelShader>    gyShaderDX11PS;
typedef gyShaderDX11<ID3D11ComputeShader>  gyShaderDX11CS;

// --------------------------------------------------------------------
template<>
int32_t gyShaderDX11<ID3D11VertexShader>::CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector ) // template specialization
{
  GY_CREATE_D3DSHADER(CreateVertexShader);
  if ( Reflect(pReflector) != 0 )
    return -1;
  return ReflectInputLayout(pByteCode,pReflector); // only gets input signatures for VertexShader
}
// --------------------------------------------------------------------
template<>
int32_t gyShaderDX11<ID3D11HullShader>::CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector ) // template specialization
{
  GY_CREATE_D3DSHADER(CreateHullShader);
  return Reflect(pReflector);
}
// --------------------------------------------------------------------
template<>
int32_t gyShaderDX11<ID3D11DomainShader>::CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector ) // template specialization
{
  GY_CREATE_D3DSHADER(CreateDomainShader);
  return Reflect(pReflector);
}
// --------------------------------------------------------------------
template<>
int32_t gyShaderDX11<ID3D11GeometryShader>::CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector ) // template specialization
{
  GY_CREATE_D3DSHADER(CreateGeometryShader);
  return Reflect(pReflector);
}
// --------------------------------------------------------------------
template<>
int32_t gyShaderDX11<ID3D11PixelShader>::CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector ) // template specialization
{
  GY_CREATE_D3DSHADER(CreatePixelShader);
  return Reflect(pReflector);
}
// --------------------------------------------------------------------
template<>
int32_t gyShaderDX11<ID3D11ComputeShader>::CreateInternal( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector ) // template specialization
{
  GY_CREATE_D3DSHADER(CreateComputeShader);
  return Reflect(pReflector);
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------
template<typename T>
int32_t gyShaderDX11<T>::Reflect( ID3D11ShaderReflection* pReflector )
{
  // general desc
  D3D11_SHADER_DESC shDesc;
  if ( FAILED( pReflector->GetDesc(&shDesc) ) ) 
    GYDEBUGRET("Cannot get desc on reflected shader\n", -1 );

  // -- first pass, just count the number of variables (constants in buffers and textures)
  uint32_t constantsFound = 0;
  uint32_t noShaderCBs = 0;
  for ( uint32_t i = 0; i < shDesc.BoundResources; ++i )
  {
    D3D11_SHADER_INPUT_BIND_DESC resDesc;
    pReflector->GetResourceBindingDesc(i,&resDesc);
    if ( resDesc.Type == D3D10_SIT_TEXTURE || resDesc.Type == D3D10_SIT_SAMPLER )
    {
      ++constantsFound;
    }else if ( resDesc.Type == D3D10_SIT_CBUFFER )
    {
      ID3D11ShaderReflectionConstantBuffer* pCB = pReflector->GetConstantBufferByName(resDesc.Name);
      D3D11_SHADER_BUFFER_DESC bufDesc;
      pCB->GetDesc(&bufDesc);
      constantsFound += bufDesc.Variables;
      ++noShaderCBs;
    }
  }
  // -- second pass, creates the constant buffer and filling it in
  constantBuffer = new gyShaderConstantBuffer;
  if ( constantsFound > 0 )
  {
    if ( constantBuffer->Init(constantsFound) != 0 )
      GYDEBUGRET( "Error creating constant buffer dx11\n", -1 );

    if ( ((gyShaderConstantBuffer*)constantBuffer)->GetImpl()->D3D11FillFromReflector(pReflector,noShaderCBs) != 0 )
      GYDEBUGRET( "Error filling constant table from reflector\n", -1 );  
  }

  return R_OK;
}
// --------------------------------------------------------------------
template<typename T>
int32_t gyShaderDX11<T>::ReflectInputLayout( gyShaderByteCode* pByteCode, ID3D11ShaderReflection* pReflector )
{
  D3D11_SHADER_DESC shDesc;
  if ( FAILED( pReflector->GetDesc(&shDesc) ) ) 
    GYDEBUGRET("Cannot get desc on reflected shader\n", -1 );

  R_FAIL_IF( shDesc.InputParameters == 0 || shDesc.InputParameters > MAX_VERTEXELEMENT_COUNT, "Invalid number of vertex elements" );

  // allocate temporary array of vertex-elements
  uint32_t elementsCount = shDesc.InputParameters;
  gyVertexElement elements[128];
  uint32_t acumOffset=0;
  for ( uint32_t i = 0; i < elementsCount; ++i )
  {
    D3D11_SIGNATURE_PARAMETER_DESC pVDesc;
    pReflector->GetInputParameterDesc( i, &pVDesc );
    acumOffset += gyRenderer::Impl::D3D11SignatureDescToVertexElement( &pVDesc, &elements[i], acumOffset );
  }
  
  // create the vertex layout with the array of elements, and store in our internal
  inputLayoutID = gyGetRendererFactory().CreateVertexLayout( elements, elementsCount, pByteCode );
  R_FAIL_IF( !inputLayoutID.IsValid(), "Cannot find/create vertex layout" );

  return R_OK;
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// gyRendererFactory::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyRendererFactory::Impl

void gyRendererFactory::Impl::Destroy()
{
}

int gyRendererFactory::Impl::CompileShader( gyShaderByteCode* pByteCode, const gyShaderSourceDesc& sourceDesc )
{
  DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
  flags |= D3DCOMPILE_DEBUG;
#endif
  ID3DBlob* pError;
  const char* prof = gyRenderer::Impl::GetShaderProfile(pByteCode->shaderType, pByteCode->shaderModel);
  FAIL_IF( !prof, "Invalid combination shader-type and shader-profile" );
  HRESULT hr = D3DX11CompileFromMemory( (LPCSTR)sourceDesc.sourceData, sourceDesc.sourceLength, NULL, NULL, NULL, 
    sourceDesc.entryPoint, prof, flags, 0, NULL, &pByteCode->GetImpl()->pShaderBlob, &pError, NULL );
  if ( FAILED(hr) )
  {
    if ( pError )
      GYDEBUG( (char*)pError->GetBufferPointer() );
    SafeRelease(pError);
    return -1;
  }
  SafeRelease(pError);
  return R_OK;
}

gyShader* gyRendererFactory::Impl::CreateShader( gyShaderByteCode* pByteCode )
{
  gyShader* pShader=NULL;
  const gyShaderType& st = pByteCode->shaderType;
  const gyShaderModel& sm = pByteCode->shaderModel;
  switch ( st )
  {
  case ST_VERTEXSHADER  : pShader = new gyShaderDX11VS(st,sm); break;
  case ST_HULLSHADER    : pShader = new gyShaderDX11HS(st,sm); break;
  case ST_DOMAINSHADER  : pShader = new gyShaderDX11DS(st,sm); break;
  case ST_GEOMETRYSHADER: pShader = new gyShaderDX11GS(st,sm); break;
  case ST_PIXELSHADER   : pShader = new gyShaderDX11PS(st,sm); break;
  case ST_COMPUTESHADER : pShader = new gyShaderDX11CS(st,sm); break;
  }
  FAIL_IF( !pShader, "Cannot create shader type" );
  return pShader;
}

int32_t gyRendererFactory::Impl::D3D11CreateEmptyTexture2D( int32_t width, int32_t height, DXGI_FORMAT texf, uint32_t bindflags, ID3D11Texture2D** pOutTex )
{
  D3D11_TEXTURE2D_DESC descDepth;
  ZeroMemory( &descDepth, sizeof(descDepth) );
  descDepth.Width = width;
  descDepth.Height = height;
  descDepth.MipLevels = 1;
  descDepth.ArraySize = 1;
  descDepth.Format = texf;
  descDepth.SampleDesc.Count = 1;
  descDepth.SampleDesc.Quality = 0;
  descDepth.Usage = D3D11_USAGE_DEFAULT;
  descDepth.BindFlags = bindflags;
  descDepth.CPUAccessFlags = 0;
  descDepth.MiscFlags = 0;
  return D3D11DEVICE->CreateTexture2D( &descDepth, NULL, pOutTex );
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////
// gyTexture::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyTexture::Impl
class gyTexture::Impl
{
public:
  Impl():pResource(0), pShaderV(0), pRenderTargetV(0), pDepthStencilV(0)
  {}
  void Destroy()
  {
    SafeRelease(pDepthStencilV);
    SafeRelease(pRenderTargetV);
    SafeRelease(pShaderV);
    SafeRelease(pResource);
  }
  
  int Init( gyTextureType type, uint8_t* data, uint32_t size, uint32_t flags = 0 )
  {
    // creating resource
    HRESULT hr = D3DX11CreateTextureFromMemory( D3D11DEVICE, data, size, NULL, NULL, (ID3D11Resource**)&pResource, NULL );
    R_FAIL_IF( FAILED(hr), "Cannot load texture from memory\n" );

    // common texture creation
    R_FAIL_IF( D3D11CreateInternal(type,flags)!=0, "Failing to create internal texture" );
    return R_OK;
  }
  
  int Init( gyTextureType type, const char* filename, uint32_t flags = 0 )
  {
    // creating resource
    HRESULT hr = D3DX11CreateTextureFromFileA( 
      D3D11DEVICE, 
      gyFileSystemHelper::ComposeSearchPathAndFileName(filename), 
      NULL, NULL, (ID3D11Resource**)&pResource, NULL );
    FAIL_IF( FAILED(hr), "Cannot load texture from file\n" );

    // common texture creation
    R_FAIL_IF( D3D11CreateInternal(type,flags)!=0, "Failing to create internal texture" );
    return R_OK;
  }
  
  int Init( gyTextureType type, gyComponentFormat texf, uint8_t* data, uint32_t width, uint32_t height, uint32_t flags = 0 )
  {
    D3D11_SUBRESOURCE_DATA srdata;
    srdata.pSysMem = (void*)data;
    srdata.SysMemPitch = width * gyGetComponentFormatSize(texf);
    srdata.SysMemSlicePitch = width * height * gyGetComponentFormatSize(texf);
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Format = gyRenderer::Impl::ComponentFormatToD3D11(texf);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    ID3D11Device* dev = D3D11DEVICE;
    R_FAIL_IF( FAILED(dev->CreateTexture2D(&desc,&srdata,&pResource)), "Cannot create texture 2D" );
    // common texture creation
    R_FAIL_IF( D3D11CreateInternal(type,flags)!=0, "Failing to create internal texture" );
    return R_OK;
  }

  int Init( gyIDRenderTarget rtid )
  {
    gyRenderTarget* pRT = gyGetRendererFactory().LockRenderTarget(rtid);
    R_FAIL_IF( !pRT || !pRT->GetImpl()->pSRView, "Invalid RT object or does not contain SR View" );
    pShaderV = pRT->GetImpl()->pSRView;
    pShaderV->AddRef(); // keep one reference
    return R_OK;
  }

  int32_t D3D11CreateInternal( gyTextureType type, uint32_t flags )
  {
    GYUNUSED(type);
    GYUNUSED(flags);
    HRESULT hr = D3D11DEVICE->CreateShaderResourceView( pResource, NULL, &pShaderV);
    R_FAIL_IF( FAILED(hr), "Cannot create Shader Resource View\n" );
    return R_OK;
  }
public:
  ID3D11Texture2D*          pResource;
  ID3D11ShaderResourceView* pShaderV;
  ID3D11RenderTargetView*   pRenderTargetV;
  ID3D11DepthStencilView*   pDepthStencilV;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyDepthStencilState::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyDepthStencilState::Impl
class gyDepthStencilState::Impl
{
public:
  Impl():pDepthStencilState(0){}
  void Destroy(){ SafeRelease(pDepthStencilState); }
  int Init(const gyDepthStencilState& dss)
  {
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    gyRenderer::Impl::D3D11DepthStencilInfoToDesc( &dss, &depthStencilDesc );
    HRESULT hr = D3D11DEVICE->CreateDepthStencilState( &depthStencilDesc, &pDepthStencilState );
    return SUCCEEDED(hr) ? R_OK : -1;
  }

  ID3D11DepthStencilState* pDepthStencilState;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyRasterState::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gyRasterState::Impl
class gyRasterState::Impl
{
public:
  Impl():pRasterState(0){}
  void Destroy(){ SafeRelease(pRasterState); }
  int Init(const gyRasterState& rs)
  {
    D3D11_RASTERIZER_DESC rasterDesc;
    gyRenderer::Impl::D3D11RasterInfoToDesc( &rs, &rasterDesc );
    HRESULT hr = D3D11DEVICE->CreateRasterizerState( &rasterDesc, &pRasterState );
    return SUCCEEDED(hr) ? R_OK : -1;
  }

  ID3D11RasterizerState* pRasterState;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gyBlendState::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gylBlendState::Impl
class gyBlendState::Impl
{
public:
  Impl(): pBlendState(NULL){}
  void Destroy(){ SafeRelease(pBlendState); }
  int Init(bool alphaToCov, bool indepBlend, gyBlendStateDesc* descs, uint32_t count)
  {
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = alphaToCov;
    blendDesc.IndependentBlendEnable = indepBlend;
    for ( uint32_t i = 0; i < count; ++i )
      gyRenderer::Impl::D3D11BlendInfoToDesc( descs+i, blendDesc.RenderTarget+i);
    HRESULT hr = D3D11DEVICE->CreateBlendState(&blendDesc, &pBlendState);
    return SUCCEEDED(hr) ? R_OK : -1;
  }
  ID3D11BlendState* pBlendState;
};
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// gySamplerState::Impl
//////////////////////////////////////////////////////////////////////////
#pragma region gySamplerState::Impl
class gySamplerState::Impl
{
public:
  Impl():pSamplerState(0){}
  void Destroy(){ SafeRelease(pSamplerState); }
  int Init(const gySamplerState& ss)
  {
    D3D11_SAMPLER_DESC samplerDesc;
    gyRenderer::Impl::D3D11SamplerInfoToDesc( &ss, &samplerDesc );
    HRESULT hr = D3D11DEVICE->CreateSamplerState(&samplerDesc, &pSamplerState );
    return SUCCEEDED(hr) ? R_OK : -1;
  }

  ID3D11SamplerState* pSamplerState;
};
#pragma endregion


#pragma region gyRenderCommandBuffer::Impl
gyRenderCommandBuffer::Impl::Impl()
  : pDevContext(NULL), pSwapChain(NULL)
{
}
gyRenderCommandBuffer::Impl::~Impl()
{
  Destroy();
}
int gyRenderCommandBuffer::Impl::Create(gyRenderCommandBuffer& rcb, int width, int height, bool fullscreen )
{
  HWND hwnd = (HWND)gyGetCommonDict().Get("win_hwnd",0).GetI();
  FAIL_IF( !hwnd, "No HWND present" );
  return D3D11CreateImmediateContext(rcb, hwnd, width, height, !fullscreen);
}

void gyRenderCommandBuffer::Impl::Destroy()
{
  if ( pDevContext ) 
    pDevContext->ClearState();
  SafeRelease(pDevContext);
  SafeRelease(pSwapChain);
}

void gyRenderCommandBuffer::Impl::Present()
{
  pSwapChain->Present(0,0);
}

void gyRenderCommandBuffer::Impl::ClearRenderTarget( gyRenderTarget* pRTDx, uint32_t clearFlags, float* rgba, float depth, uint8_t stencil )
{
  if ( ! pRTDx ) return;
  gyRenderTarget::Impl* rtImpl = pRTDx->GetImpl();
  if ( FlagIsSet(clearFlags, CS_CLEAR_COLOR) && rtImpl->pRTView )
  {
    pDevContext->ClearRenderTargetView( rtImpl->pRTView, rgba );
  }
  if ( rtImpl->pRTDSView )
  {
    UINT dsflags = (FlagIsSet(clearFlags,CS_CLEAR_DEPTH) ? D3D11_CLEAR_DEPTH : 0) | 
      (FlagIsSet(clearFlags,CS_CLEAR_STENCIL) ? D3D11_CLEAR_STENCIL : 0);
    if ( dsflags != 0 )
      pDevContext->ClearDepthStencilView(rtImpl->pRTDSView, dsflags, depth, (UINT8)stencil);
  }
}

int32_t gyRenderCommandBuffer::Impl::D3D11CreateImmediateContext( gyRenderCommandBuffer& rcb, HWND hWnd, int width, int height, bool windowed )                          // only for immediate context
{
  // backbuff dimension wanted
  uint32_t dim[2]={width,height};

  HRESULT hr = S_OK;  
  UINT createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_DRIVER_TYPE  driverType;
  D3D_FEATURE_LEVEL featureLevel;

  // fallbacks in order
  D3D_DRIVER_TYPE driverTypes[] ={ D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
  UINT numDriverTypes = ARRAYSIZE( driverTypes );
  D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
  UINT numFeatureLevels = ARRAYSIZE( featureLevels );

  // preparing swap chain structure
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory( &sd, sizeof( sd ) );
  sd.BufferCount = 1;
  sd.BufferDesc.Width = dim[0];
  sd.BufferDesc.Height = dim[1];
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = windowed;

  // try to create device falling back in the arrays above
  hr = E_FAIL;
  ID3D11Device* pDev=NULL;
  bool doItAgainWODebugFlags;
  do
  {
    doItAgainWODebugFlags = false;
    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes && FAILED(hr); driverTypeIndex++ )
    {
      driverType = driverTypes[driverTypeIndex];
      hr = D3D11CreateDeviceAndSwapChain( NULL, driverType, NULL, createDeviceFlags, 
        featureLevels, numFeatureLevels,
        D3D11_SDK_VERSION, &sd, &pSwapChain, &pDev, &featureLevel, 
        &pDevContext );
    }
    // if it couldn't and there's device_debug flag, try again without that flag
    // that flag causes problems in some system with outdated sdk debug layers.
    if ( FAILED(hr) && (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG)!=0 )
    {
      doItAgainWODebugFlags = true;
      createDeviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
    }
  }while ( doItAgainWODebugFlags );
  gyGetRenderer().GetImpl()->D3D11SetDevice(pDev);
  // if failed the last one, return
  if( FAILED( hr ) )
    return -1;

  if ( CreateMainRenderTarget( rcb, dim[0], dim[1] ) != R_OK )
    return -1;
  return R_OK;
}

void gyRenderCommandBuffer::Impl::UnboundAllPSResources()
{
  ID3D11ShaderResourceView* allnull[MAX_PS_TEXTUREUNITS]={NULL};
  pDevContext->PSSetShaderResources( 0, MAX_PS_TEXTUREUNITS, allnull );
}

int32_t gyRenderCommandBuffer::Impl::CreateMainRenderTarget(gyRenderCommandBuffer& rcb, uint32_t width, uint32_t height)
{
  rcb.mainRT = new gyRenderTarget;
  // getting backbuffer texture into our main RT texture
  HRESULT hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&rcb.mainRT->GetImpl()->pRTTexture );
  if( FAILED( hr ) )
    GYDEBUGRET("Cannot retrieve back buffer from device\n", -1 );

  // just creating rt view (no depth at the moment)
  if ( rcb.mainRT->Init(width,height, FMT_R24G8_UI, 0) != 0 )
    GYDEBUGRET("Cannot create Main RT\n", -1);

  // set the RT
  SetMainRenderTarget(rcb.mainRT);

  return R_OK;
}

int32_t gyRenderCommandBuffer::Impl::SetRenderTarget( gyIDRenderTarget* allRts, gyIDRenderTarget stOMDepthTargetID, uint32_t start, int32_t count )
{
  UnboundAllPSResources();

  gyRendererFactory& factory = gyGetRendererFactory();
  // depth target
  ID3D11DepthStencilView* pDepthStencilView=NULL;
  if ( stOMDepthTargetID.IsValid() ) 
  {
    gyRenderTarget* pRTDx = factory.LockRenderTarget(stOMDepthTargetID);
    pDepthStencilView = pRTDx->GetImpl()->pRTDSView;
  }

  // RT views
  ID3D11RenderTargetView* rtvs[MAX_OM_RENDERTARGETS];
  gyRenderTarget* pRT;
  for ( uint32_t i = start; i < start+count; ++i )
  {
    pRT = factory.LockRenderTarget(allRts[i]);
    rtvs[i] = pRT ? pRT->GetImpl()->pRTView : NULL;
  }
  pDevContext->OMSetRenderTargets( count, rtvs, pDepthStencilView );
  return R_OK;
}

void gyRenderCommandBuffer::Impl::SetMainRenderTarget( gyRenderTarget* pMainRT)
{
  gyRenderTarget::Impl* impl = pMainRT->GetImpl();
  pDevContext->OMSetRenderTargets( 1, &impl->pRTView, impl->pRTDSView );
}

void gyRenderCommandBuffer::Impl::SetViewport( float left, float top, float width, float height )
{
  // Setup the viewport
  D3D11_VIEWPORT vp;
  vp.Width = (FLOAT)width;
  vp.Height = (FLOAT)height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = left;
  vp.TopLeftY = top;
  pDevContext->RSSetViewports( 1, &vp );
}

void gyRenderCommandBuffer::Impl::SetInputLayout(gyVertexLayout* pVL, gyShader* pVS)
{
  if ( pVS )
    pVL->Ensure(pVS->GetByteCode());
  pDevContext->IASetInputLayout( pVL->GetImpl()->pD3D11InputLayout );
}

void gyRenderCommandBuffer::Impl::SetMeshBuffers( gyIDMeshBuffer* meshBufferIDs, gyPrimitiveTopology& primTopo, uint32_t start, uint32_t count )
{
  gyRendererFactory& factory = gyGetRendererFactory();
  // RT views
  ID3D11Buffer* vbuffers[MAX_IA_VERTEXBUFFERS];
  uint32_t strides[MAX_IA_VERTEXBUFFERS];
  uint32_t offsets[MAX_IA_VERTEXBUFFERS];
  gyMeshBuffer* pMB;
  for ( uint32_t i = 0; i < MAX_IA_VERTEXBUFFERS; ++i )
  {
    if ( i >= start && i+start < count )
    {
      pMB = factory.LockMeshBuffer(meshBufferIDs[i]);
      if ( pMB )
      {
        gyMeshBuffer::Impl* mbimpl = pMB->GetImpl();
        vbuffers[i] = mbimpl->vb;
        strides[i] = pMB->GetVertexStride();
        offsets[i] = 0;

        // if this mesh has IB, set it
        if ( mbimpl->ib )
          pDevContext->IASetIndexBuffer( mbimpl->ib, mbimpl->D3D11GetIndexFormat(*pMB), 0 );

        // set the primitive topology
        if ( pMB->GetPrimitiveTopology() != PT_NONE && primTopo != pMB->GetPrimitiveTopology() )
        {
          pDevContext->IASetPrimitiveTopology( mbimpl->D3D11GetPrimitiveTopology(*pMB) );
          primTopo = pMB->GetPrimitiveTopology();
        }
        continue;
      }
    }
    vbuffers[i] = NULL;
    strides[i] = offsets[i] = 0;
  }
  pDevContext->IASetVertexBuffers( 0, MAX_IA_VERTEXBUFFERS, vbuffers, strides, offsets );
}

void gyRenderCommandBuffer::Impl::SetDepthStencilState( gyDepthStencilState* pDSS, uint32_t stencilRef )
{
  if ( pDSS && pDSS->GetImpl()->pDepthStencilState )
  {
    pDevContext->OMSetDepthStencilState( pDSS->GetImpl()->pDepthStencilState, stencilRef );
  }
}

void gyRenderCommandBuffer::Impl::SetRasterState( gyRasterState* pRS )
{
  if ( pRS && pRS->GetImpl()->pRasterState )
    pDevContext->RSSetState( pRS->GetImpl()->pRasterState );
}

void gyRenderCommandBuffer::Impl::SetVSShaderCB( gyShader* pVS )
{
  if ( !pVS ) return;
  // set VS constant buffers + shader resources (move away this from here)
  gyShaderConstantBuffer* pCB = pVS->GetConstantBuffer();
  if ( pCB )
  {
    pCB->GetImpl()->D3D11FlushBuffers(this);
    // upload the buffers no matter if are unchanged in the device...!!!, we have to change this!!
    pDevContext->VSSetConstantBuffers( 0, pCB->GetImpl()->cbsCount, pCB->GetImpl()->arrayOfDX11CBuffers ); 
  }
}

void gyRenderCommandBuffer::Impl::SetVSShader( gyShader* pVS)
{
  if ( !pVS ) 
    return;
  gyShaderDX11VS* vs = static_cast<gyShaderDX11VS*>(pVS);
  pDevContext->VSSetShader( vs->D3D11GetInterface(), NULL, 0 );
}

void gyRenderCommandBuffer::Impl::SetPSSampler( gySamplerState* pSampler, uint32_t slot )
{
  if ( !pSampler ) return;
  pDevContext->PSSetSamplers(slot, 1, &pSampler->GetImpl()->pSamplerState );
}

void gyRenderCommandBuffer::Impl::SetPSTexture( gyTexture* tex, uint32_t slot )
{
  if ( !tex ) return;
  pDevContext->PSSetShaderResources( slot, 1, &tex->GetImpl()->pShaderV );
}

void gyRenderCommandBuffer::Impl::SetPSShaderCBConstants(gyShader* pPS)
{
  gyShaderConstantBuffer* pCB = pPS->GetConstantBuffer();

  // upload the buffers no matter if are unchanged in the device...!!!, we have to change this!!
  gyShaderConstantBuffer::Impl* cbimpl = pCB->GetImpl();
  cbimpl->D3D11FlushBuffers(this);
  pDevContext->PSSetConstantBuffers( 0, cbimpl->cbsCount, cbimpl->arrayOfDX11CBuffers );
}

void gyRenderCommandBuffer::Impl::SetPSShader(gyShader* pPS)
{
  if ( !pPS ) return;
  gyShaderDX11PS* ps = static_cast<gyShaderDX11PS*>(pPS);
  pDevContext->PSSetShader( ps->D3D11GetInterface(), NULL, 0 );
}

void gyRenderCommandBuffer::Impl::SetBlendState(gyBlendState* pBS)
{
  if ( !pBS )
    pDevContext->OMSetBlendState( NULL, NULL, 0xffffffff );
  else
    pDevContext->OMSetBlendState( pBS->GetImpl()->pBlendState, NULL, 0xffffffff );
}

void gyRenderCommandBuffer::Impl::Draw(gyMeshBuffer* mb)
{
  if ( !mb ) return;
  gyMeshBuffer::Impl* mbImpl = mb->GetImpl();
  if ( mbImpl->ib )
    pDevContext->DrawIndexed( mb->GetIndexCount(), 0, 0 );
  else
    pDevContext->Draw( mb->GetVertexCount(), 0 );
}
#pragma endregion

ID3D11DeviceContext* gyRenderer::Impl::D3D11GetMainContext()
{ 
  return gyGetRenderer().GetRenderConmmandBuffer(MAIN_RENDER_THREAD).GetImpl()->D3D11GetDeviceContext(); 
}
