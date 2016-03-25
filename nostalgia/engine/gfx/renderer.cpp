#include <stdafx.h>
#include <engine/gfx/renderer.h>

#include GY_PLATINC_RENDERER

#define GY_CHECKID_AND_RETURN_RES(id,arr) \
  if ( !id.IsValid() ) GYDEBUGRET( #arr " - Invalid resource id\n", NULL ); \
  return arr.Get(id)

#pragma region gyRenderer ///////////////////////////////////////////////////////////////////
gyRenderer* gyRenderer::instance = 0;
gyRenderer::gyRenderer() : frameCount(0)
{ 
  impl = new Impl;
}

gyRenderer::~gyRenderer()
{
  for ( size_t i = 0; i < renderContexts.size(); ++i )
    SafeDestroyDelete( renderContexts[i] );
  renderContexts.clear();

  for ( size_t i = 0; i < renderCommandBuffers.size(); ++i )
    SafeDestroyDelete( renderCommandBuffers[i] );
  renderCommandBuffers.clear();

  SafeDestroyDelete(impl);
}

int gyRenderer::Create(gyVarDict& params)
{
  FAIL_IF( !impl, "No implementation!");

  // create the main/immediate command buffer
  gyRenderCommandBuffer* pMainRCB = new gyRenderCommandBuffer;
  if ( pMainRCB->Create(params) != R_OK )
  {
    SafeDelete(pMainRCB);
    return -1;
  }
  gyGetRenderer().AddRenderCommandBuffer(pMainRCB);

  return impl->Create(params);
}

int gyRenderer::Query( int qry, void* output )
{
  FAIL_IF( !impl, "No implementation!");
  return impl->Query(qry,output);
}


void gyRenderer::BeginFrame()
{
}

void gyRenderer::EndFrame()
{
  RenderCommandBufferList::reverse_iterator it = renderCommandBuffers.rbegin();
  for ( ; it != renderCommandBuffers.rend(); ++it )
  {
    (*it)->Present();
  }
}

void gyRenderer::Render()
{
  BeginFrame();
  ++frameCount;
  gyRenderContext* rf;
  for ( size_t i = 0; i < renderContexts.size(); ++i )
  {
    rf = renderContexts[i];
    if ( rf == NULL ) break;
    rf->DoRender();
  }
  EndFrame();
}

static bool AscendentPriority( gyRenderContext* a, gyRenderContext* b )
{
  return a->GetPriority() <= b->GetPriority();  
}

int gyRenderer::AddRenderContext( gyRenderContext* rf )
{
  R_FAIL_IF( rf == NULL, "RenderContext object is null" );
  RenderContextList::iterator it = std::find( renderContexts.begin(), renderContexts.end(), rf );
  int ret = -1;
  if ( it == renderContexts.end() )
  {
    renderContexts.push_back(rf);
    SortRenderContexts();
    ret = R_OK;
  }
  
  return ret;
}

void gyRenderer::SortRenderContexts()
{
  std::sort( renderContexts.begin(), renderContexts.end(), AscendentPriority );
}

int gyRenderer::AddRenderCommandBuffer(gyRenderCommandBuffer* rcb)
{
  R_FAIL_IF( rcb == NULL, "No RCB valid" );
  RenderCommandBufferList::iterator it = std::find( renderCommandBuffers.begin(), renderCommandBuffers.end(), rcb );
  int ret = -1;
  if ( it == renderCommandBuffers.end() )
  {
    renderCommandBuffers.push_back(rcb);
    ret= R_OK;
  }
  return ret;
}

void gyRenderer::FillCreateTemplate(gyVarDict& outTemplate)
{
  GY_DEFAULT_PARAM( outTemplate, "width"      , 1024 );
  GY_DEFAULT_PARAM( outTemplate, "height"     , 768);
  GY_DEFAULT_PARAM( outTemplate, "fullscreen" , false);
}

#pragma endregion

#pragma region gyRendererFactory ///////////////////////////////////////////////////////////////////
gyRendererFactory::gyRendererFactory()
{
  impl = new Impl;
}

gyRendererFactory::~gyRendererFactory()
{ 
  Release(); 
}

void gyRendererFactory::Release()
{
  SafeDestroyDelete(impl);
  FAIL_IF(
     vertexLayoutArray.GetValidElementCount()>0
  || rtArray.GetValidElementCount()>0
  || textureArray.GetValidElementCount()>0
  || meshBuffArray.GetValidElementCount()>0
  || shaderArray.GetValidElementCount()>0
  || samplersArray.GetValidElementCount()>0
  || rasterArray.GetValidElementCount()>0
  || depthStateArray.GetValidElementCount()>0
  || blendStateArray.GetValidElementCount()>0, "Some id weren't released");


  vertexLayoutArray.ReleaseAll();
  rtArray.ReleaseAll(); 
  textureArray.ReleaseAll();
  meshBuffArray.ReleaseAll();
  shaderArray.ReleaseAll();
  samplersArray.ReleaseAll();
  rasterArray.ReleaseAll();
  depthStateArray.ReleaseAll();
  blendStateArray.ReleaseAll();
}

gyIDRenderTarget  gyRendererFactory::CreateRenderTarget( int32_t width, int32_t height, gyComponentFormat texf, uint32_t rtflags)
{
  gySharedPtr<gyRenderTarget> pNewRT = new gyRenderTarget;
  if ( pNewRT->Init(width,height,texf,rtflags) != R_OK )
    GYDEBUGRET( "Cannot create RenderTarget\n", gyIDRenderTarget::INVALID() );

  return rtArray.Add(pNewRT);
}

gyIDVertexLayout gyRendererFactory::CreateVertexLayout( gyVertexElement* elements, uint32_t count, gyShaderByteCode* pVSByteCode/*=null*/ )
{
  gySharedPtr<gyVertexLayout> pVL = new gyVertexLayout;
  if ( pVL->Init(elements,count,pVSByteCode) != R_OK )
    GYDEBUGRET("Error creating vertex layout\n", gyIDVertexLayout::INVALID() );
  return vertexLayoutArray.AddUnique(pVL);
}

gyIDMeshBuffer gyRendererFactory::CreateMeshBuffer( gyIDVertexLayout vLayout, gyPrimitiveTopology primTopology, gyIndexType indexType )
{
  // create the object
  gySharedPtr<gyMeshBuffer> pMB = new gyMeshBuffer;
  if ( pMB->Init(vLayout, primTopology,indexType) != R_OK )
    GYDEBUGRET( "Cannot create MeshBuffer\n", gyIDMeshBuffer::INVALID() );

  // add the object, return the id, find available slot
  return meshBuffArray.Add(pMB);
}

gySharedPtr<gyShaderByteCode> gyRendererFactory::CompileShader( gyShaderType stype, const gyShaderSourceDesc& sourceDesc )
{
  gySharedPtr<gyShaderByteCode> pSBC = new gyShaderByteCode;
  pSBC->shaderType = stype;
  pSBC->shaderModel = sourceDesc.shaderModel;

  if ( impl->CompileShader(pSBC, sourceDesc) != R_OK )
    return NULL;

  return pSBC;
}

gyIDShader gyRendererFactory::CreateShader ( gyShaderByteCode* pByteCode )
{
  FAIL_IF( !pByteCode || pByteCode->shaderType==ST_NONE || pByteCode->shaderModel==SM_NONE, "Invalid shader byte code" );
  
  // creating the shader
  gySharedPtr<gyShader> pShader = impl->CreateShader(pByteCode);  
  
  if ( pShader->Init( pByteCode ) != R_OK )
    GYDEBUGRET( "Error creating the shader\n", gyIDShader::INVALID() );

  // adding to the array, returning id
  return shaderArray.Add(pShader);
}

gyIDTexture gyRendererFactory::CreateTexture( gyTextureType type, uint8_t* data, uint32_t size, uint32_t flags )
{
  gySharedPtr<gyTexture> pTex = new gyTexture;
  return InternalAddTexture( pTex, pTex->Init(type, data, size, flags) );
}

gyIDTexture gyRendererFactory::CreateTexture( gyTextureType type, const char* filename, uint32_t flags)
{
  gySharedPtr<gyTexture> pTex = new gyTexture;
  return InternalAddTexture( pTex, pTex->Init(type, filename, flags) );
}

gyIDTexture gyRendererFactory::CreateTexture( gyIDRenderTarget rt )
{
  gySharedPtr<gyTexture> pTex = new gyTexture;
  return InternalAddTexture( pTex, pTex->Init(rt) );
}

gyIDTexture gyRendererFactory::CreateTexture( gyTextureType type, gyComponentFormat texf, uint8_t* data, uint32_t width, uint32_t height, uint32_t flags )
{
  gySharedPtr<gyTexture> pTex = new gyTexture;
  return InternalAddTexture( pTex, pTex->Init(type, texf, data, width, height, flags ) );
}

gyIDTexture gyRendererFactory::InternalAddTexture( gyTexture* tex, int initRes )
{
  if ( !tex || initRes != R_OK )
    GYDEBUGRET( "Cannot create Texture\n", gyIDTexture::INVALID() );
  return textureArray.Add(tex);
}

gyIDSamplerState gyRendererFactory::CreateSamplerState( gyFilterType filter, gyAddress u, gyAddress v, gyAddress w, 
                                                       float lodBias, uint8_t maxAniso, const float* bordercol, gyComparisonFunc compFunc)
{

  gySharedPtr<gySamplerState> pState = new gySamplerState;
  if ( pState->Init( filter, u, v, w, lodBias, maxAniso, bordercol, compFunc ) != R_OK )
  {
    GYDEBUGRET( "Cannot create Sampler state\n", gyIDSamplerState::INVALID() );
  }

  return samplersArray.AddUnique(pState);
}

gyIDDepthStencilState gyRendererFactory::CreateDepthStencilState( bool depthTest, bool depthWrite, gyComparisonFunc depthFunc, 
                                              bool stencilTest, uint8_t stencilReadMask, uint8_t stencilWriteMask,
                                              gyStencilOp frontFail, gyStencilOp frontDepthFail, gyStencilOp frontPass, gyComparisonFunc frontFunc,
                                              gyStencilOp backFail, gyStencilOp backDepthFail, gyStencilOp backPass, gyComparisonFunc backFunc)
{
  // create object and init
  gySharedPtr<gyDepthStencilState> pState = new gyDepthStencilState;
  if ( pState->Init( depthTest, depthWrite, depthFunc, stencilTest, stencilReadMask, stencilWriteMask
                   , frontFail, frontDepthFail, frontPass, frontFunc
                   , backFail, backDepthFail, backPass, backFunc ) != R_OK )
    GYDEBUGRET( "Cannot create Depth stencil state\n", gyIDDepthStencilState::INVALID() );

  return depthStateArray.AddUnique(pState);
}

gyIDRasterState gyRendererFactory::CreateRasterState( gyFillMode fm, gyCullMode cm, int32_t dbias, float dbclamp, float ssdb, uint8_t flags)
{
  // create object and init
  gySharedPtr<gyRasterState> pState = new gyRasterState;
  if ( pState->Init( fm, cm, dbias, dbclamp, ssdb, flags ) != R_OK )
    GYDEBUGRET( "Cannot create Raster state\n", gyIDRasterState::INVALID() );
  
  return rasterArray.AddUnique(pState);
}

gyIDBlendState gyRendererFactory::CreateBlendState( bool alphaToCoverage, gyBlend src, gyBlend dst, gyBlendOp op, gyBlend srcA, gyBlend dstA, gyBlendOp opA, uint8_t writeMask)
{
  gySharedPtr<gyBlendState> state = new gyBlendState;
  gyBlendStateDesc bs(src,dst,op, srcA,dstA,opA);
  if ( state->Init(alphaToCoverage, &bs, 1) != R_OK )
    GYDEBUGRET( "Cannot create BlendState", gyIDBlendState::INVALID() );
  return blendStateArray.AddUnique(state);
}

int32_t gyRendererFactory::CreateShaderTechnique( gyShaderTechnique* pOutTech, gyShaderSourceDesc* stageSources )
{
  R_FAIL_IF( !stageSources, "Invalid stage sources array" );
  R_FAIL_IF( !pOutTech, "Invalid out technique pointer" );

  for ( uint32_t i = ST_FIRSTSTAGE; i < ST_MAXSTAGES; ++i )
  {
    gyIDShader shId;
    if ( ! gyShaderSourceDesc::IsNull(stageSources[i]) )
    {
      gySharedPtr<gyShaderByteCode> pByteCode = CompileShader( (gyShaderType)i, stageSources[i] );
      FAIL_IF( !pByteCode, "Cannot compile shader from this source for this stage" );
      if ( pByteCode )
      {
        shId = CreateShader( pByteCode );
        FAIL_IF( !shId.IsValid(), "Cannot create shader from this bytecode for this stage" );
      }
    }
    pOutTech->SetStageShader( (gyShaderType)i, shId );
  }
  return R_OK;
}

int32_t gyRendererFactory::CreateShaderTechnique( gyShaderTechnique* pOutTech, gyShaderSourceDesc* stageSources, gyShaderType* types, int32_t count )
{
  R_FAIL_IF( !pOutTech, "Invalid out technique pointer" );
  R_FAIL_IF( !stageSources, "Invalid stage sources array" );
  R_FAIL_IF( !types, "Invalid types array" );

  gyIDShader shId;
  for ( int i = 0; i < count; ++i )
  {
    if ( gyShaderSourceDesc::IsNull(stageSources[i]) ) continue;
    gySharedPtr<gyShaderByteCode> pByteCode = CompileShader( types[i], stageSources[i] );
    FAIL_IF( !pByteCode, "Cannot compile shader from this source for this stage" );
    if ( pByteCode )
    {
      shId = CreateShader( pByteCode );
      FAIL_IF( !shId.IsValid(), "Cannot create shader from this bytecode for this stage" );
      if ( shId.IsValid() )
      {
        pOutTech->SetStageShader( types[i], shId );
        shId.Invalidate();
      }
    }
  }
  return R_OK;
}


int32_t gyRendererFactory::CreateShaderTechnique( gyShaderTechnique* pOutTech, gyShaderByteCode** stageByteCodes )
{
  R_FAIL_IF( !stageByteCodes, "Invalid stage bytecodes array" );
  R_FAIL_IF( !pOutTech, "Invalid out technique pointer" );
  for ( uint32_t i = ST_FIRSTSTAGE; i < ST_MAXSTAGES; ++i )
  {
    gyIDShader shId;
    if ( stageByteCodes[i] )
    { 
      shId = CreateShader( stageByteCodes[i] );
      R_FAIL_IF( !shId.IsValid(), "Cannot create shader from this bytecode for this stage" );
    }
    pOutTech->SetStageShader( (gyShaderType)i, shId );
  }
  return R_OK;
}

int32_t gyRendererFactory::CreateShaderTechnique( gyShaderTechnique* pOutTech, gyIDShader* stageShaders )
{
  R_FAIL_IF( !stageShaders, "Invalid stage shaders array" );
  R_FAIL_IF( !pOutTech, "Invalid out technique pointer" );

  for ( uint32_t i = ST_FIRSTSTAGE; i < ST_MAXSTAGES; ++i )
    pOutTech->SetStageShader( (gyShaderType)i, stageShaders[i] );
  return R_OK;
}

gyTexture* gyRendererFactory::LockTexture( gyIDTexture id ){ GY_CHECKID_AND_RETURN_RES(id, textureArray); }
gyRenderTarget* gyRendererFactory::LockRenderTarget( gyIDRenderTarget id ){ GY_CHECKID_AND_RETURN_RES(id, rtArray); }
gyVertexLayout* gyRendererFactory::LockVertexLayout( gyIDVertexLayout id ){ GY_CHECKID_AND_RETURN_RES(id, vertexLayoutArray); }
gyShader* gyRendererFactory::LockShader( gyIDShader id ){ GY_CHECKID_AND_RETURN_RES(id, shaderArray); }
gyMeshBuffer* gyRendererFactory::LockMeshBuffer( gyIDMeshBuffer id ){ GY_CHECKID_AND_RETURN_RES(id, meshBuffArray); }
gySamplerState* gyRendererFactory::LockSamplerState( gyIDSamplerState id ){ GY_CHECKID_AND_RETURN_RES(id, samplersArray); }
gyRasterState* gyRendererFactory::LockRasterState( gyIDRasterState rsId ){ GY_CHECKID_AND_RETURN_RES(rsId, rasterArray); }
gyDepthStencilState* gyRendererFactory::LockDepthStencilState( gyIDDepthStencilState id ){ GY_CHECKID_AND_RETURN_RES(id, depthStateArray);}
gyBlendState* gyRendererFactory::LockBlendState( gyIDBlendState id ){ GY_CHECKID_AND_RETURN_RES(id, blendStateArray);}

// todo: implement locking mechanism
void gyRendererFactory::UnlockRenderTarget( gyIDRenderTarget rtId ){}
void gyRendererFactory::UnlockVertexLayout( gyIDVertexLayout vlId ){}
void gyRendererFactory::UnlockMeshBuffer( gyIDMeshBuffer mbId ){}
void gyRendererFactory::UnlockShader( gyIDShader shId ){}
void gyRendererFactory::UnlockTexture( gyIDTexture texId ){}
void gyRendererFactory::UnlockSamplerState( gyIDSamplerState ssId ){}
void gyRendererFactory::UnlockRasterState( gyIDRasterState rsId ){}
void gyRendererFactory::UnlockDepthStencilState( gyIDDepthStencilState dsId ){}
void gyRendererFactory::UnlockBlendState( gyIDBlendState bsId ){}

int32_t gyRendererFactory::ReleaseResource( uint32_t resId )
{
  if ( !gyIDGeneric::CreateFrom(resId).IsValid() ) 
    return -1;
  uint32_t type = gyIDGeneric::ExtractType(resId);
  int rno  = gyIDGeneric::ExtractNumber(resId);

  switch( type )
  {
  case ID_RENDERTARGET: rtArray.Release(gyIDRenderTarget(rno));break;
  case ID_VERTEXLAYOUT: vertexLayoutArray.Release(gyIDVertexLayout(rno)); break;
  case ID_MESHBUFFER  : meshBuffArray.Release(gyIDMeshBuffer(rno)); break;
  case ID_SHADER      : shaderArray.Release(gyIDShader(rno)); break;
  case ID_TEXTURE     : textureArray.Release( gyIDTexture(rno) ); break;
  case ID_RASTERSTATE : rasterArray.Release(gyIDRasterState(rno)); break;
  case ID_SAMPLERSTATE: samplersArray.Release(gyIDSamplerState(rno)); break;
  case ID_DEPTHSTENCILSTATE:depthStateArray.Release(gyIDDepthStencilState(rno)); break;
  case ID_BLENDSTATE  : blendStateArray.Release(gyIDBlendState(rno)); break;
  default:
    FAIL_ALWAYS("invalid resource");break;
  }
  return -1;
}

#pragma endregion

#pragma region gyShaderByteCode ///////////////////////////////////////////////////////////////////
gyShaderByteCode::gyShaderByteCode()
  : shaderType(ST_NONE),shaderModel(SM_NONE)
{
  impl = new Impl;
}
gyShaderByteCode::~gyShaderByteCode()
{
  SafeDestroyDelete( impl );
}
uint8_t* gyShaderByteCode::GetByteCode()
{
  return impl->GetByteCode();
  //return (ubyte*)pShaderBlob->GetBufferPointer();
}
uint32_t gyShaderByteCode::GetLength()
{
  return impl->GetLength();
  //return (uint32)pShaderBlob->GetBufferSize();
}
#pragma endregion

#pragma region gyVertexLayout ///////////////////////////////////////////////////////////////////
gyVertexLayout::gyVertexLayout():elements(0), count(0)
{
  impl = new Impl;
}
gyVertexLayout::~gyVertexLayout()
{
  SafeFree(elements);
  SafeDestroyDelete(impl);
}

int gyVertexLayout::Init( gyVertexElement* elms, uint32_t elmsCount, gyShaderByteCode* pByteCode )
{
  R_FAIL_IF( !elms || elmsCount > MAX_VERTEXELEMENT_COUNT, "Elements is null or count invalid\n" );
  SafeFree(elements);
  elements = (gyVertexElement*)calloc( sizeof(gyVertexElement), elmsCount );
  count = elmsCount;
  memcpy( this->elements, elms, sizeof(gyVertexElement)*elmsCount );
  return impl->Init(*this,pByteCode);
}

void gyVertexLayout::Ensure(gyShaderByteCode* pByteCode)
{
  impl->Ensure(*this,pByteCode);
}

uint32_t gyVertexLayout::GetSizeInBytes()
{
  uint32_t s = 0;
  for ( uint32_t i = 0; i < count; ++i )
    s += gyGetComponentFormatSize( (gyComponentFormat)elements[i].format() );
  return s;
}

uint32_t gyVertexLayout::GetElementCount(){ return count; }

int32_t gyVertexLayout::GetElementByIndex( uint32_t ndx, gyVertexElement* pOutVE )
{
  R_FAIL_IF( !elements || !pOutVE || ndx > MAX_VERTEXELEMENT_COUNT, "Invaid parameters or null elements" );
  *pOutVE = elements[ndx];
  return R_OK;
}

#pragma endregion

#pragma region gyRenderCommandBuffer ///////////////////////////////////////////////////////////////////
gyRenderCommandBuffer::gyRenderCommandBuffer()
{
  impl = new Impl;
}

void gyRenderCommandBuffer::Destroy()
{
  quad.Release();
  mainRT = NULL; // smart ptr
  SafeDestroyDelete(impl);
}

int gyRenderCommandBuffer::Create( gyVarDict& params)
{ 
  int width = gyGetCommonDict().Get("win_width",1024);
  int height= gyGetCommonDict().Get("win_height", 768);
  bool fullscreen = gyGetCommonDict().Get("win_fs",0).GetI()!=0;  
  if ( impl->Create(*this, width, height, fullscreen) != R_OK )
    return -1;
  SetViewport(0.0f, 0.0f, (float)width, (float)height );
  return R_OK;
}

void gyRenderCommandBuffer::Present( )
{
  impl->Present();
}

void gyRenderCommandBuffer::ClearRenderTarget( gyIDRenderTarget* rts, uint32_t numRts, uint32_t clearFlags, float* rgba, float depth, uint8_t stencil)
{
  float defrgba[4]={0.0f,0.0f,0.0f,1.0f};
  rgba = rgba?rgba:defrgba;

  // if those are null, then clearing MAIN render target
  if ( rts == NULL && numRts == 0 )
  {
    impl->ClearRenderTarget( mainRT, clearFlags, rgba, depth, stencil );
    return;
  }else
  {
    gyRendererFactory& factory = gyGetRenderer().GetFactory();
    // if not null, then clearing normal render targets
    FAIL_IF( !numRts, "Render target count bad" );
    for ( uint32_t i = 0; i < numRts; ++i )
    {
      gyRenderTarget* pRTDx = factory.LockRenderTarget(rts[i]);
      if ( !pRTDx ) continue;
      impl->ClearRenderTarget( pRTDx, clearFlags, rgba, depth, stencil );
    }

    // if depth stencil target active in separate slot
    gyRenderTarget* pRTDx = factory.LockRenderTarget(stOMDepthTargetID);
    impl->ClearRenderTarget( pRTDx, CS_CLEAR_DEPTH|CS_CLEAR_STENCIL, NULL, depth, stencil );
  }
}

int32_t gyRenderCommandBuffer::SetDepthTarget( gyIDRenderTarget rtDepth )
{
  if ( stOMDepthTargetID == rtDepth ) 
    return R_OK;  
  stOMDepthTargetID = rtDepth;
  return R_OK;
}

int32_t gyRenderCommandBuffer::SetRenderTarget( gyIDRenderTarget* rts, uint32_t numRts, uint32_t startSlot)
{
  R_FAIL_IF( !rts || !numRts || startSlot+numRts>MAX_OM_RENDERTARGETS, "Invalid Rts" );

  bool anyChange = false;
  const uint32_t c = MAX_OM_RENDERTARGETS-startSlot;
  const uint32_t count = c < numRts ? c : numRts;
  gyIDRenderTarget newId,oldId;
  for ( uint32_t i = startSlot; i < count; ++i )
  {
    newId = rts[i-startSlot];
    oldId = stOMRenderTargetsID[i];
    if ( newId != oldId )
    {
      anyChange = true;
      stOMRenderTargetsID[i] = newId;
    }
  }
  if ( anyChange )
  {
    impl->SetRenderTarget(stOMRenderTargetsID,stOMDepthTargetID,startSlot,count);    
  }
  return R_OK;
}

int32_t gyRenderCommandBuffer::SetMainRenderTarget( )
{
  impl->SetMainRenderTarget(mainRT);
  return R_OK;
}

void gyRenderCommandBuffer::SetViewport( float left, float top, float width, float height )
{
  impl->SetViewport(left,top,width,height);
}

void gyRenderCommandBuffer::SetInputLayout( gyIDVertexLayout ilayout )
{
  if ( !ilayout.IsValid() || ilayout == stIAInputLayout ) return;
  stIAInputLayout = ilayout;

  gyRendererFactory& factory = gyGetRenderer().GetFactory();
  gyVertexLayout* pVL = factory.LockVertexLayout( ilayout );
  gyShader* pVS = NULL;
  if ( stVSShaderID.IsValid() )
    pVS = factory.LockShader( stVSShaderID );

  impl->SetInputLayout( pVL, pVS );  
}

void gyRenderCommandBuffer::SetMeshBuffers( gyIDMeshBuffer* mbId, uint32_t numMbs, uint32_t master, uint32_t startStream)
{
  stIAMasterMB = master;
  bool anyChange = false;
  const uint32_t c = MAX_IA_VERTEXBUFFERS-startStream;
  const uint32_t count = c < numMbs ? c : numMbs;
  gyIDMeshBuffer newId,oldId;
  for ( uint32_t i = startStream; i < count; ++i )
  {
    newId = mbId[i-startStream];
    oldId = stIAMeshBufferID[i];
    if ( newId != oldId )
    {
      anyChange = true;
      stIAMeshBufferID[i] = newId;
    }
  }
  if ( anyChange )
  {
    impl->SetMeshBuffers(stIAMeshBufferID, stIAPrimTopology, startStream, count);
  }
}

void gyRenderCommandBuffer::SetDepthStencilState( gyIDDepthStencilState dsId, uint32_t stencilRef )
{
  if ( dsId == stDepthStencil ) 
    return;
  
  gyDepthStencilState* pDSS = gyGetRenderer().GetFactory().LockDepthStencilState(dsId);
  impl->SetDepthStencilState(pDSS, stencilRef);    
  stDepthStencil = dsId;
}

void gyRenderCommandBuffer::SetRasterState( gyIDRasterState rsId )
{
  if ( rsId == stRSState ) return;
  
  gyRasterState* pRS = gyGetRenderer().GetFactory().LockRasterState(rsId);
  impl->SetRasterState(pRS);
  stRSState = rsId;
}

void gyRenderCommandBuffer::SetVSShaderCB( gyIDShader shId )
{
  gyShader* pVS = gyGetRenderer().GetFactory().LockShader( shId );
  impl->SetVSShaderCB(pVS);
}

void gyRenderCommandBuffer::SetVSShader( gyIDShader shId )
{
  if ( shId == stVSShaderID ) return;
  stVSShaderID = shId;
  //ValidateVSInputLayout(); // must to be called when draw()
  gyShader* pVS = gyGetRenderer().GetFactory().LockShader( shId );
  impl->SetVSShader(pVS);
}

void gyRenderCommandBuffer::SetPSSampler( gyIDSamplerState sId, uint32_t slot )
{
  if ( slot >= MAX_PS_SAMPLERS || stPSSamplers[slot] == sId ) return;
  stPSSamplers[slot] = sId;
  gySamplerState* pSampler = gyGetRenderer().GetFactory().LockSamplerState( sId );
  impl->SetPSSampler(pSampler,slot);
}

void gyRenderCommandBuffer::SetPSTexture( gyIDTexture tId, uint32_t slot )
{
  if ( slot >= MAX_PS_TEXTUREUNITS || stPSTexturesID[slot] == tId ) return;
  stPSTexturesID[slot] = tId;

  gyTexture* tex = gyGetRenderer().GetFactory().LockTexture(tId);  
  impl->SetPSTexture(tex,slot);
}

void gyRenderCommandBuffer::SetPSShaderCB( gyIDShader shId, bool applyConstants, bool applyTextures, bool applySamplers)
{
  // set PS constant buffers + shader resources + samplers
  gyShader* pPS = gyGetRenderer().GetFactory().LockShader( shId );
  FAIL_IF(!pPS, "Invalid pixelshader");
  // -- first, constant buffers
  if ( applyConstants )
    impl->SetPSShaderCBConstants(pPS);    

  if ( !applySamplers && !applyTextures ) return;

  gyShaderConstantBuffer* pCB = pPS->GetConstantBuffer();

  // -- second, textures + samplers
  for ( uint32_t i = 0; i < pCB->GetConstantsCount(); ++i )
  {
    const gyShaderConstant& sc = pCB->GetConstant(i);
    if ( applyTextures && sc.IsTexture() ) 
    { 
      gyIDTexture tid; 
      tid.Number( sc.resNumber ); 
      if ( tid.IsValid() ) 
        SetPSTexture( tid, sc.index ); 
    }
    
    if ( applySamplers && sc.IsSampler() ) 
    { 
      gyIDSamplerState sid; 
      sid.Number( sc.resNumber ); 
      SetPSSampler( sid, sc.index ); 
    }

    if ( !sc.IsTexture() && !sc.IsSampler() ) 
      break; // because textures and samplers come first in the array
  }
}

void gyRenderCommandBuffer::SetPSShader( gyIDShader shId )
{
  if ( stPSShaderID == shId ) return;
  stPSShaderID = shId;
  gyShader* pPS = gyGetRenderer().GetFactory().LockShader( shId );
  impl->SetPSShader(pPS);
}

void gyRenderCommandBuffer::UnboundAllPSResources()
{
  memset( stPSTexturesID, gyIDTexture::INVALID(), sizeof(gyIDTexture)*MAX_PS_TEXTUREUNITS );
  impl->UnboundAllPSResources();
}

void gyRenderCommandBuffer::SetBlendState( gyIDBlendState bsId )
{
  if ( stOMBlendState == bsId ) return;
  stOMBlendState = bsId;
  gyBlendState* pBS = gyGetRendererFactory().LockBlendState(bsId);
  impl->SetBlendState(pBS); // will will default
}

// -- drawing
void gyRenderCommandBuffer::Draw( )
{
  // more validation should go here
  // validate VS input layout with the mesh buffer vertex element layout
  //ValidateVSInputLayout();

  // draw depending on the mesh buffer set
  gyMeshBuffer* mesh = gyGetRenderer().GetFactory().LockMeshBuffer( stIAMeshBufferID[stIAMasterMB] );  
  impl->Draw(mesh);  
}

// -- high level drawing
void gyRenderCommandBuffer::DrawQuadScreen( gyIDTexture texture, gyIDShader ps, gyIDSamplerState sampler )
{
  if ( !quad.IsValid() ) 
    quad.CreateDefault(); // create screen quad at demand  

  gyIDSamplerState finalSamp = sampler.IsValid() ? sampler : quad.sampler;
  bool useDefaultPS = !ps.IsValid(); 
  gyIDShader finalPS;
  if ( useDefaultPS )
  {
    finalPS = quad.ps;
    gyShaderConstantBuffer* pCB = gyGetRenderer().GetFactory().LockShader( quad.ps )->GetConstantBuffer();
    pCB->SetConstantValue( quad.psRegDiffuse, texture );
  }else
  {
    finalPS = ps;
  }
  // IA geometry
  SetInputLayout(quad.vertexLayout);
  SetMeshBuffers(&quad.mesh,1);
  // vs
  SetVSShader( quad.vs );
  // ps
  SetPSShaderCB( finalPS );
  SetPSShader( finalPS );
  // submit call
  Draw();
}

void gyRenderCommandBuffer::DrawQuadScreen( gyIDTexture texture, gyIDSamplerState sampler)
{
  DrawQuadScreen(texture, gyIDShader::INVALID(), sampler);
}

void gyRenderCommandBuffer::DrawQuadScreen( gyIDShader ps, gyIDSamplerState sampler )
{
  if ( !ps.IsValid() ) 
    return;
  if ( !quad.IsValid() ) 
    quad.CreateDefault(); // create screen quad at demand  

  gyIDSamplerState finalSamp = sampler.IsValid() ? sampler : quad.sampler;  

  // IA geometry
  SetInputLayout(quad.vertexLayout);
  SetMeshBuffers(&quad.mesh,1);
  // vs
  SetVSShader( quad.vs );
  // ps
  SetPSShaderCB( ps );
  SetPSShader( ps );
  // submit call
  Draw();
}

#pragma endregion

#pragma region gyShaderConstantBuffer ///////////////////////////////////////////////////////////////////
gyShaderConstantBuffer::gyShaderConstantBuffer()
  : constants(0), constantsCount(0)
{
  impl = new Impl(this);
}

void gyShaderConstantBuffer::Destroy()
{
  SafeDeleteA(constants);
  SafeDestroyDelete(impl);
}

int gyShaderConstantBuffer::Init( uint32_t ccount )
{
  R_FAIL_IF( ccount == 0 || ccount > 256, "Invalid constant variables count" );
  constantsCount = ccount;
  constants = new gyShaderConstant[constantsCount];
  return R_OK;
}

int32_t gyShaderConstantBuffer::FindConstantIndexByName( const char* name )
{
  if ( ! constants ) return -1;
  R_FAIL_IF( !constants, "No constants array available" );
  uint32_t hashName = gyComputeHash( (void*)name, (uint32_t)strlen(name) );
  for ( uint32_t i = 0; i < constantsCount; ++i )
  {
    if ( constants[i].nameHash == hashName ) 
      return (int32_t)i;
  }
  return -1;
}

int32_t gyShaderConstantBuffer::SetConstantValue( int32_t constantNdx, const float* values, uint32_t count ){ return impl->SetConstantValue(constantNdx, values, count); }
int32_t gyShaderConstantBuffer::SetConstantValue( int32_t constantNdx, const bool* values, uint32_t count ){ return impl->SetConstantValue(constantNdx, values, count); }
int32_t gyShaderConstantBuffer::SetConstantValue( int32_t constantNdx, const int* values, uint32_t count ){ return impl->SetConstantValue(constantNdx, values, count); }
int32_t gyShaderConstantBuffer::SetConstantValue( int32_t constantNdx, gyIDTexture tex, uint32_t count ){ return impl->SetConstantValue(constantNdx, tex, count); }
int32_t gyShaderConstantBuffer::SetConstantValue( int32_t constantNdx, gyIDSamplerState sid, uint32_t count ){ return impl->SetConstantValue(constantNdx, sid, count); }

#pragma endregion

#pragma region Misc functions ///////////////////////////////////////////////////////////////////
int gyGetComponentFormatSize( gyComponentFormat format )
{
  switch ( format )
  {
  case FMT_R32_F:
  case FMT_R32G32_F:
  case FMT_R32G32B32_F:
  case FMT_R32G32B32A32_F: return (format-FMT_R32_F+1)*4;
  case FMT_R32_SI:               
  case FMT_R32G32_SI:
  case FMT_R32G32B32_SI:
  case FMT_R32G32B32A32_SI: return (format-FMT_R32_SI+1)*4;
  case FMT_R32_UI:
  case FMT_R32G32_UI:
  case FMT_R32G32B32_UI:
  case FMT_R32G32B32A32_UI: return (format-FMT_R32_UI+1)*4;
  case FMT_R8:
  case FMT_R8G8:
  case FMT_R8G8B8:
  case FMT_R8G8B8A8: return (format-FMT_R8+1);
  case FMT_R24G8_UI: return 4;
  }
  return R_OK;
}
#pragma endregion

#pragma region gyRenderTarget ///////////////////////////////////////////////////////////////////
gyRenderTarget::gyRenderTarget()
{
  impl = new Impl;
}

gyRenderTarget::~gyRenderTarget()
{
  SafeDestroyDelete(impl);
}

int gyRenderTarget::Init( int32_t width, int32_t height, gyComponentFormat texf, uint32_t flags )
{
  return impl->Init(width,height,texf,flags);
}

#pragma endregion

#pragma region gyMeshBuffer ///////////////////////////////////////////////////////////////////
gyMeshBuffer::gyMeshBuffer() 
  : vertexStride(0), primTopo(PT_NONE), indexType(INDEX_NONE), vertexCount(0), indexCount(0)
{
  impl = new Impl;
}

gyMeshBuffer::~gyMeshBuffer()
{
  SafeDestroyDelete(impl);
}

int gyMeshBuffer::Init( gyIDVertexLayout vLayout, gyPrimitiveTopology primTopology, gyIndexType indexType)
{
  vertexLayout = vLayout;
  primTopo = primTopology;
  this->indexType = indexType;
  return impl->Init(*this);
}

int32_t gyMeshBuffer::CreateVertices( uint32_t count, void* initData, uint32_t vertexStride)
{
  R_FAIL_IF(!vertexLayout.IsValid(), "No vertex layout provided\n");
  R_FAIL_IF( count == 0, "Invalid count of vertices\n" );
  return impl->CreateVertices(*this, count, initData, vertexStride);
}

int32_t gyMeshBuffer::CreateIndices( uint32_t count, void* initData )
{
  R_FAIL_IF( indexType == INDEX_NONE, "No index type provided\n" );
  R_FAIL_IF( count == 0, "Invalid count of indices\n" );
  return impl->CreateIndices(*this, count, initData);
}

int32_t gyMeshBuffer::FillVertices( uint32_t count, void* data )
{
  if ( !count || !data ) return -1;
  return impl->FillVertices(*this, count, data);
}

int32_t gyMeshBuffer::FillIndices( uint32_t count, void* data )
{
  if ( !count || !data || indexType == INDEX_NONE ) return -1;
  return impl->FillIndices(*this,count,data);
}

int32_t gyMeshBuffer::MapVertices( void** pData )
{
  if ( !pData ) return -1;
  return impl->MapVertices(pData);
}

void  gyMeshBuffer::UnmapVertices( )
{
  impl->UnmapVertices();
}

#pragma endregion

#pragma region gyShader ///////////////////////////////////////////////////////////////////
gyShader::gyShader(gyShaderType st, gyShaderModel sm)
  :constantBuffer(NULL), byteCode(NULL), shaderType(st), shaderModel(sm)
{
}

gyShader::~gyShader()
{
  SafeDestroyDelete(constantBuffer);
}

int32_t gyShader::Init( gyShaderByteCode* pByteCode )
{
  byteCode = pByteCode;
  return R_OK;
}

#pragma endregion

#pragma region gyShaderTechnique ///////////////////////////////////////////////////////////////////
gyShaderTechnique::gyShaderTechnique()
{
}

void gyShaderTechnique::Destroy()
{
  gyGetRenderer().GetFactory().ReleaseResources(stageShaders,ST_MAXSTAGES);
}

void gyShaderTechnique::SetStageShader(gyShaderType stage, gyIDShader shader )
{
  gyGetRenderer().GetFactory().ReleaseResource(stageShaders[stage]);
  stageShaders[stage] = shader;
}
#pragma endregion

#pragma region gyTexture ///////////////////////////////////////////////////////////////////
gyTexture::gyTexture():creationFlags(0)
{
  impl = new Impl;
}

gyTexture::~gyTexture()
{
  SafeDestroyDelete(impl);
}

int gyTexture::Init( gyTextureType type, uint8_t* data, uint32_t size, uint32_t flags )
{
  R_FAIL_IF( !data || !size || type==TT_NONE, "Invalid texture creation params" );
  R_FAIL_IF( type==TT_TEXTURE3D || type==TT_CUBEMAP || type==TT_RENDERABLE || type==TT_TEXTURE2DARRAY, "Not supported so far" );
  creationFlags = flags;
  return impl->Init(type,data,size,flags);
}

int gyTexture::Init( gyTextureType type, const char* filename, uint32_t flags)
{
  R_FAIL_IF( type==TT_NONE || type==TT_TEXTURE3D || type==TT_CUBEMAP || type==TT_RENDERABLE || type==TT_TEXTURE2DARRAY, "Not supported so far" );
  creationFlags=flags;
  return impl->Init(type,filename,flags);
}

int gyTexture::Init( gyTextureType type, gyComponentFormat texf, uint8_t* data, uint32_t width, uint32_t height, uint32_t flags )
{
  R_FAIL_IF( type==TT_NONE || type==TT_TEXTURE3D || type==TT_CUBEMAP || type==TT_RENDERABLE || type==TT_TEXTURE2DARRAY, "Not supported so far" );
  R_FAIL_IF( texf != FMT_R8G8B8A8, "Only supported FMT_R8G8B8A8" );
  R_FAIL_IF( width == 0 || height == 0, "Invalid dimension" );
  creationFlags = flags;
  return impl->Init(type,texf,data,width,height,flags);
}

int gyTexture::Init( gyIDRenderTarget rtid )
{
  R_FAIL_IF( !rtid.IsValid(), "Invalid RT id" );
  return impl->Init(rtid);
}
#pragma endregion

#pragma region gyBlendState //////////////////////////////////////////////////////////////////////
gyBlendState::gyBlendState() : impl(new Impl), alphaToCoverage(false), independentBlending(false)
{
}

gyBlendState::~gyBlendState()
{
  SafeDestroyDelete(impl);
}

int32_t gyBlendState::Init(bool alphaToCov, gyBlendStateDesc* descs, uint32_t count)
{
  FAIL_IF( count==0 || count >8, "Count should be in 1..8" );
  if ( count > 1 )
  {
    GYERROR( "Only 1 RT blend description supported" );
    return -1;
  }

  alphaToCoverage = alphaToCov;
  independentBlending = count > 1;
  desc = *descs;
  return impl->Init(alphaToCov, independentBlending, descs, count);
}

#pragma endregion

#pragma region gyDepthStencilState ///////////////////////////////////////////////////////////////////
gyDepthStencilState::gyDepthStencilState()
{
  impl = new Impl;
}
gyDepthStencilState::~gyDepthStencilState()
{
  SafeDestroyDelete(impl);
}

int gyDepthStencilState::Init(bool depthTest, bool depthWrite, gyComparisonFunc depthFunc, 
                               bool stencilTest, uint8_t stencilReadMask, uint8_t stencilWriteMask,
                               gyStencilOp frontFail, gyStencilOp frontDepthFail, gyStencilOp frontPass, gyComparisonFunc frontFunc,
                               gyStencilOp backFail, gyStencilOp backDepthFail, gyStencilOp backPass, gyComparisonFunc backFunc)
{
 SetDepthEnabled( depthTest );
 SetDepthWrite( depthWrite );
 SetDepthFunc( depthFunc );
 SetStencilTest(stencilTest);
 SetStencilReadMask(stencilReadMask);
 SetStencilWriteMask(stencilWriteMask);
 SetStencilOp( gyDepthStencilState::FRONTFACE, frontFail, frontDepthFail, frontPass, frontFunc );
 SetStencilOp( gyDepthStencilState::BACKFACE, backFail, backDepthFail, backPass, backFunc );
 return impl ? impl->Init(*this) : -1;
}

#pragma endregion

#pragma region gyRasterState///////////////////////////////////////////////////////////////////
gyRasterState::gyRasterState()
{
  impl = new Impl;
}

gyRasterState::~gyRasterState()
{
  SafeDestroyDelete(impl);
}

int gyRasterState::Init( gyFillMode fm, gyCullMode cm, int32_t dbias, float dbclamp, float ssdb, uint8_t flags)
{
  SetFillMode(fm);
  SetCullMode(cm);
  SetDepthBias(dbias);
  SetDepthBiasClamp(dbclamp);
  SetSlopeScaledDepthBias(ssdb);
  SetFlags(flags);  
  return impl ? impl->Init(*this) : -1;
}

#pragma endregion

#pragma region gySamplerState ///////////////////////////////////////////////////////////////////
gySamplerState::gySamplerState()
{
  impl = new Impl;
}

gySamplerState::~gySamplerState()
{
  SafeDestroyDelete(impl);
}

int gySamplerState::Init( gyFilterType filter, gyAddress u, gyAddress v, gyAddress w, float lodBias, uint8_t maxAniso, const float* bordercol, gyComparisonFunc compFunc)
{
  float black[4]={0,0,0,0};
  SetBorderColor( bordercol?bordercol:black );
  SetLOD( 0.0f, 3.402823466e+38F );
  SetMipLODBias(lodBias);
  SetFilter( filter );
  SetAddresses(u,v,w);
  SetCompFunction( compFunc );
  SetMaxAniso( maxAniso );
  return impl ? impl->Init(*this) : -1;
}

#pragma endregion

int32_t gyScreenQuad::CreateDefault()
{
  gyRendererFactory& factory = gyGetRenderer().GetFactory();
  // preparing vertex elements, layout and mesh buffer
  gyVertexElement elements[] = 
  {
    gyVertexElement( VES_POSITION,   0, FMT_R32G32B32A32_F, 0 ),
    gyVertexElement( VES_TEXCOORD,  16, FMT_R32G32_F,       0 )
  };
  vertexLayout = factory.CreateVertexLayout(elements,2);
  if ( !vertexLayout.IsValid() )
    GYDEBUGRET( "Error creating vlayout for screen quad\n", -1 );

  mesh = factory.CreateMeshBuffer( vertexLayout, PT_TRILIST );
  gyMeshBuffer* quadMeshBuf = factory.LockMeshBuffer( mesh );
  // creating vertices x,y,z  u,v
  float  verts[] = {
    -1.0f, 1.0f,0.0f,1.0f,    0.0f,0.0f,
    1.0f, 1.0f,0.0f,1.0f,     1.0f,0.0f,
    -1.0f,-1.0f,0.0f,1.0f,    0.0f,1.0f,
    1.0f, 1.0f,0.0f,1.0f,     1.0f,0.0f,
    1.0f,-1.0f,0.0f,1.0f,     1.0f,1.0f,
    -1.0f,-1.0f,0.0f,1.0f,    0.0f,1.0f };
  if ( quadMeshBuf->CreateVertices(6,verts) != 0 )
    GYDEBUGRET( "Error creating vertices\n", -1 );

  // temp  
  uint32_t len = (uint32_t)gyFileSystemHelper::FileReadToEnd( "base/ScreenQuad.fx", gyGetBigStackChunkST(), gyGetBigStackChunkSTSize() );
  if ( len == 0 )
  {
    FAIL_ALWAYS("cannot read shader file");
    GYDEBUGRET( "Cannot read shader file\n", -1 );
  }

  // vs
  gyShaderSourceDesc vsSourceDesc = { SM_40, "VS", gyGetBigStackChunkST(), len };
  gySharedPtr<gyShaderByteCode> pbcode = factory.CompileShader( ST_VERTEXSHADER, vsSourceDesc );
  FAIL_IF( !pbcode, "cannot compile vs" );
  factory.LockVertexLayout(vertexLayout)->Ensure(pbcode);
  vs = factory.CreateShader( pbcode );
  FAIL_IF( !vs.IsValid(), "cannot create shader" );
  gyShaderSourceDesc psSourceDesc = { SM_40, "PS", (uint8_t*)gyGetBigStackChunkST(), len };
  pbcode = factory.CompileShader( ST_PIXELSHADER, psSourceDesc );
  FAIL_IF( !pbcode, "cannot compile ps" );
  ps = factory.CreateShader( pbcode );
  FAIL_IF( !ps.IsValid(), "cannot create shader" );

  gyShaderConstantBuffer* pPSCB = factory.LockShader(ps)->GetConstantBuffer();
  psRegDiffuse = pPSCB->FindConstantIndexByName("txDiffuse");
  psRegSampler = pPSCB->FindConstantIndexByName("smDiffuse");
  sampler = factory.CreateSamplerState( FT_MIN_MAG_MIP_POINT, AD_WRAP, AD_WRAP, AD_WRAP );
  pPSCB->SetConstantValue( psRegSampler, sampler );  
  return R_OK;
}

void gyScreenQuad::Release()
{
  gyGetRendererFactory().ReleaseResource(mesh);
  gyGetRendererFactory().ReleaseResource(ps);
  gyGetRendererFactory().ReleaseResource(vs);
  gyGetRendererFactory().ReleaseResource(sampler);
  gyGetRendererFactory().ReleaseResource(vertexLayout);
}

#undef GY_CHECKID_AND_RETURN_RES
