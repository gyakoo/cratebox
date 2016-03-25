#ifndef _RENDERER_H_
#define _RENDERER_H_
#include <engine/gfx/renderertypes.h>

#define gyReleaseResource(idRes) { gyGetRenderer().GetFactory().ReleaseResource(idRes); idRes.Invalidate(); }

// ******************************************************************************************************************************
// RendererFactory
// ******************************************************************************************************************************
class gyRendererFactory
{
  GY_PIMPL_CLASS();
public:
  gyRendererFactory();
  ~gyRendererFactory();
  void  Release();
  // Creation
  gyIDRenderTarget  CreateRenderTarget( int32_t width, int32_t height, gyComponentFormat texf, uint32_t rtflags=RT_ALLFLAGS );
  gyIDVertexLayout  CreateVertexLayout( gyVertexElement* elements, uint32_t count, gyShaderByteCode* pVSByteCode=0 );
  gyIDMeshBuffer    CreateMeshBuffer( gyIDVertexLayout vLayout, gyPrimitiveTopology primTopology=PT_NONE, gyIndexType indexType=INDEX_NONE );  
  gySharedPtr<gyShaderByteCode> CompileShader( gyShaderType stype, const gyShaderSourceDesc& sourceDesc );
  gyIDShader        CreateShader ( gyShaderByteCode* pByteCode );
  gyIDTexture       CreateTexture( gyTextureType type, uint8_t* data, uint32_t size, uint32_t flags = TF_NONE );
  gyIDTexture       CreateTexture( gyTextureType type, const char* filename, uint32_t flags = TF_NONE );
  gyIDTexture       CreateTexture( gyIDRenderTarget rt );
  gyIDTexture       CreateTexture( gyTextureType type, gyComponentFormat texf, uint8_t* data, uint32_t width, uint32_t height, uint32_t flags = TF_NONE );
  gyIDSamplerState  CreateSamplerState( gyFilterType filter, gyAddress u, gyAddress v, gyAddress w, float lodBias=0.0f, uint8_t maxAniso=16, const float* bordercol=0, gyComparisonFunc compFunc=COMP_NONE);
  gyIDDepthStencilState CreateDepthStencilState( bool depthTest, bool depthWrite, gyComparisonFunc depthFunc=COMP_LESS_EQUAL, 
    bool stencilTest=false, uint8_t stencilReadMask=0xff, uint8_t stencilWriteMask=0xff,
    gyStencilOp frontFail=SOP_KEEP, gyStencilOp frontDepthFail=SOP_KEEP, gyStencilOp frontPass=SOP_KEEP, gyComparisonFunc frontFunc=COMP_ALWAYS,
    gyStencilOp backFail=SOP_KEEP, gyStencilOp backDepthFail=SOP_KEEP, gyStencilOp backPass=SOP_KEEP, gyComparisonFunc backFunc=COMP_ALWAYS );
  gyIDRasterState CreateRasterState( gyFillMode fm=FM_SOLID, gyCullMode cm=CULL_BACK, int32_t dbias=0, float dbclamp=0, float ssdb=0, uint8_t flags=RASTER_DEPTHCLIP_ENABLED);
  gyIDBlendState CreateBlendState( bool aphaToCoverage, gyBlend src, gyBlend dst, gyBlendOp op, gyBlend srcA=BLEND_ONE, gyBlend dstA=BLEND_ZERO, gyBlendOp opA=BLENDOP_ADD, uint8_t writeMask=0xff );

  // Shader Techniques
  int32_t CreateShaderTechnique( gyShaderTechnique* pOutTech, gyShaderSourceDesc* stageSources );
  int32_t CreateShaderTechnique( gyShaderTechnique* pOutTech, gyShaderByteCode** stageByteCodes );
  int32_t CreateShaderTechnique( gyShaderTechnique* pOutTech, gyIDShader* stageShaders );
  int32_t CreateShaderTechnique( gyShaderTechnique* pOutTech, gyShaderSourceDesc* stageSources, gyShaderType* types, int32_t count );

  // Accessing to internal resources (only for local operations, do not use the resource pointers. use ids instead)
  gyRenderTarget*   LockRenderTarget( gyIDRenderTarget rtId );
  gyVertexLayout*   LockVertexLayout( gyIDVertexLayout vlId );
  gyMeshBuffer*     LockMeshBuffer( gyIDMeshBuffer mbId );
  gyShader*         LockShader( gyIDShader shId );
  gyTexture*        LockTexture( gyIDTexture texId );
  gySamplerState*   LockSamplerState( gyIDSamplerState ssId );
  gyRasterState*    LockRasterState( gyIDRasterState rsId );
  gyDepthStencilState* LockDepthStencilState( gyIDDepthStencilState dsId );
  gyBlendState*     LockBlendState( gyIDBlendState bsId );

  void UnlockRenderTarget( gyIDRenderTarget rtId );
  void UnlockVertexLayout( gyIDVertexLayout vlId );
  void UnlockMeshBuffer( gyIDMeshBuffer mbId );
  void UnlockShader( gyIDShader shId );
  void UnlockTexture( gyIDTexture texId );
  void UnlockSamplerState( gyIDSamplerState ssId );
  void UnlockRasterState( gyIDRasterState rsId );
  void UnlockDepthStencilState( gyIDDepthStencilState dsId );
  void UnlockBlendState( gyIDBlendState bsId );

  // Destroying resources
  int32_t ReleaseResource( uint32_t resId );
  template<typename T>
  void ReleaseResources( T* resources, uint32_t count )
  {
    for ( uint32_t i = 0; i < count; ++i )
      ReleaseResource( (uint32_t)resources[i]);
  }

protected:
  gyIDTexture InternalAddTexture( gyTexture* pTex, int initRes );

  gyResourceMgr<gyTexture, ID_TEXTURE> textureArray;
  gyResourceMgr<gyVertexLayout, ID_VERTEXLAYOUT> vertexLayoutArray;
  gyResourceMgr<gyRenderTarget, ID_RENDERTARGET> rtArray;
  gyResourceMgr<gyMeshBuffer, ID_MESHBUFFER> meshBuffArray;
  gyResourceMgr<gyShader, ID_SHADER> shaderArray;
  gyResourceMgr<gySamplerState, ID_SAMPLERSTATE> samplersArray;
  gyResourceMgr<gyRasterState, ID_RASTERSTATE> rasterArray;
  gyResourceMgr<gyDepthStencilState, ID_DEPTHSTENCILSTATE> depthStateArray;
  gyResourceMgr<gyBlendState, ID_BLENDSTATE> blendStateArray;
};

// ******************************************************************************************************************************
// Renderer
// ******************************************************************************************************************************
class gyRenderer : public gyReflectedObject
{  
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyRenderer);
public:
  gyRenderer();
  ~gyRenderer();
  gy_override int Create(gyVarDict& params);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);
  int Query( int qry, void* output );

public: // frame methods
  void Render( );  
  int AddRenderContext( gyRenderContext* rcontext );
  gyRenderContext* GetRenderContext( int ndx );
  void SortRenderContexts();
  int AddRenderCommandBuffer(gyRenderCommandBuffer* rcb);

public: // base and helper methods
  gyRendererFactory& GetFactory(){ return factory; }
  gyRenderCommandBuffer& GetRenderConmmandBuffer( int ndx ){ return *renderCommandBuffers[ndx]; }

protected:
  void BeginFrame();
  void EndFrame();

protected: // data members  
  typedef std::vector<gyRenderContext*> RenderContextList;
  typedef std::vector<gyRenderCommandBuffer*> RenderCommandBufferList;

  uint64_t frameCount;
  static gyRenderer* instance;
  RenderCommandBufferList renderCommandBuffers;  // the index 0 is the main one.
  RenderContextList renderContexts;
  gyRendererFactory factory;                    // creates renderer objects
};

// ******************************************************************************************************************************
// ******************************************************************************************************************************
struct gyScreenQuad
{
  int32_t CreateDefault();
  void Release();
  bool IsValid(){ return mesh.IsValid(); }

  gyIDVertexLayout vertexLayout;
  gyIDMeshBuffer mesh;
  gyIDShader vs;
  gyIDShader ps;
  int32_t psRegDiffuse;
  int32_t psRegSampler;
  gyIDSamplerState sampler;
};

// ******************************************************************************************************************************
// Base Render command buffer
// ******************************************************************************************************************************
class gyRenderCommandBuffer
{
  GY_PIMPL_CLASS();
public:  
  gyRenderCommandBuffer();
  gy_override int Create( gyVarDict& params);
  void Destroy( );

  // -- getters and operations
  void Present( );
  void ClearRenderTarget( gyIDRenderTarget* rts, uint32_t numRts, uint32_t clearFlags = CS_CLEAR_ALL, float* rgba=NULL, float depth=1.0f, uint8_t stencil=0 );  

  // -- states
  int32_t SetDepthTarget( gyIDRenderTarget rtDepth );
  int32_t SetRenderTarget( gyIDRenderTarget* rts, uint32_t numRts, uint32_t startSlot=0 );
  int32_t SetMainRenderTarget( );
  void SetViewport( float left, float top, float width, float height );
  void SetInputLayout( gyIDVertexLayout ilayout );
  void SetMeshBuffers( gyIDMeshBuffer* mbId, uint32_t numMbs, uint32_t master=0, uint32_t startStream=0 );
  void SetDepthStencilState( gyIDDepthStencilState dsId, uint32_t stencilRef=~0 );
  void SetRasterState( gyIDRasterState rsId );
  void SetVSShaderCB( gyIDShader shId );
  void SetVSShader( gyIDShader shId );
  void SetPSSampler( gyIDSamplerState sId, uint32_t slot );
  void SetPSTexture( gyIDTexture tId, uint32_t slot );
  void SetPSShaderCB( gyIDShader shId, bool applyConstants=true, bool applyTextures=true, bool applySamplers=true );
  void SetPSShader( gyIDShader shId );
  void SetBlendState( gyIDBlendState bsId );

  void UnboundAllPSResources();

  // -- drawing
  void Draw( );

  // -- high level drawing
  void DrawQuadScreen( gyIDTexture texture, gyIDShader ps=gyIDShader::INVALID(), gyIDSamplerState sampler=gyIDSamplerState::INVALID() );
  void DrawQuadScreen( gyIDTexture texture, gyIDSamplerState sampler);
  void DrawQuadScreen( gyIDShader ps, gyIDSamplerState sampler=gyIDSamplerState::INVALID() );

protected:
  gyScreenQuad  quad;
  gySharedPtr<gyRenderTarget>  mainRT;                                 // only for immediate context

  // -- RS stage
  gyIDRasterState   stRSState;

  // -- IA stage
  uint32_t              stIAMasterMB;
  gyIDMeshBuffer      stIAMeshBufferID[MAX_IA_VERTEXBUFFERS];
  gyPrimitiveTopology stIAPrimTopology;
  gyIDVertexLayout    stIAInputLayout;

  // -- VS stage  
  gyIDShader stVSShaderID;  

  // -- PS stage  
  gyIDSamplerState stPSSamplers[MAX_PS_SAMPLERS];
  gyIDTexture      stPSTexturesID[MAX_PS_TEXTUREUNITS];
  gyIDShader       stPSShaderID;

  // -- OM stage
  gyIDRenderTarget  stOMRenderTargetsID[MAX_OM_RENDERTARGETS];
  gyIDRenderTarget stOMDepthTargetID;
  gyIDDepthStencilState stDepthStencil;
  gyIDBlendState  stOMBlendState;
};

// ******************************************************************************************************************************
// RenderContext - Provides a OnRender method to make the rendering.
// ******************************************************************************************************************************
class gyRenderContext
{
public:
  gyRenderContext():priority(0), renderCommandBufferId(MAIN_RENDER_THREAD)
  {
#ifdef _DEBUG
    *name = 0;
#endif
  }
  virtual ~gyRenderContext(){}
  void DoRender(){ OnRender(); }
  virtual void Destroy(){}
  uint32_t GetPriority(){ return priority; }
  void SetPriority( uint32_t p )
  { 
    if ( priority != p )
    {
      priority = p;
      gyGetRenderer().SortRenderContexts();
    }
  }
#ifdef _DEBUG
  const char* GetName(){ return name; }
  void SetName(const char* _name){ memcpy_s(name,64,_name,64); }
#else
  const char* GetName(){ return ""; }
  void SetName( const char* ){}
#endif
  void SetRenderCommandBuffer( uint32_t rcId ){ renderCommandBufferId = rcId; }
  uint32_t GetRenderCommandBuffer( ){ return renderCommandBufferId; }
protected:
  virtual void OnRender() = 0;

protected:
  uint32_t priority;
  uint32_t renderCommandBufferId;
#ifdef _DEBUG
  char name[64];
#endif
};

// ******************************************************************************************************************************
// RenderTarget
// ******************************************************************************************************************************
class gyRenderTarget : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gyRenderTarget();
  ~gyRenderTarget();

  int Init( int32_t width, int32_t height, gyComponentFormat texf, uint32_t flags=RT_ALLFLAGS );
};

// ******************************************************************************************************************************
// VertexElement
// ******************************************************************************************************************************
struct gyVertexElement // trying fit a v element desc in 32 bits
{
  enum{ OFFMASK =0xfff00000, OFFSHIFT=20, // up to 2^12=4096 bytes addressed
        FMTMASK =0x000ff000, FMTSHIFT=12, // up to 256 format types
        SEMMASK =0x00000f00, SEMSHIFT=8,  // up to 16 semantic types
        SNXMASK =0x000000f0, SNXSHIFT=4,  // up to 16 semantic indices
        STRMASK =0x0000000f, STRSHIFT=0 };// up to 16 stream indices -- not used
  
  uint32_t  veid;

  gyVertexElement(){  *( (uint32_t*)this) = 0; }
  gyVertexElement( uint32_t sem, uint32_t offs, uint32_t fmt, uint32_t semndx, uint32_t strm=0 ) 
    : veid(0)
  {
    semantic(sem); offsetByte(offs); format(fmt); 
    semanticIndex(semndx); streamIndex(strm);
  }
  uint32_t offsetByte()const{ return (veid&OFFMASK)>>OFFSHIFT; }
  uint32_t format()    const{ return (veid&FMTMASK)>>FMTSHIFT; }
  uint32_t semantic()  const{ return (veid&SEMMASK)>>SEMSHIFT; }
  uint32_t semanticIndex()const{ return (veid&SNXMASK)>>SNXSHIFT; }
  uint32_t streamIndex()const{return (veid&STRMASK)>>STRSHIFT; }

  void offsetByte(uint32_t v){ veid = (veid&~OFFMASK)| ( (v<<OFFSHIFT)&OFFMASK); }
  void format(uint32_t v)    { veid = (veid&~FMTMASK)| ( (v<<FMTSHIFT)&FMTMASK); }
  void semantic(uint32_t v)  { veid = (veid&~SEMMASK)| ( (v<<SEMSHIFT)&SEMMASK); }
  void semanticIndex(uint32_t v){ veid = (veid&~SNXMASK)|( (v<<SNXSHIFT)&SNXMASK); }
  void streamIndex(uint32_t v){ veid = (veid&~STRMASK)|( (v<<STRSHIFT)&STRMASK); }
};

// ******************************************************************************************************************************
// VertexLayout
// ******************************************************************************************************************************
class gyVertexLayout : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gyVertexLayout();
  ~gyVertexLayout();
  int Init( gyVertexElement* elements, uint32_t count, gyShaderByteCode* pByteCode=0 );
  void Ensure(gyShaderByteCode* pByteCode);
  gy_override uint32_t ComputeHash(){ return gyComputeHash( (const void*)elements, sizeof(gyVertexElement)*count ); } 

  uint32_t GetSizeInBytes();
  uint32_t GetElementCount();
  int32_t GetElementByIndex( uint32_t ndx, gyVertexElement* pOutVE );

protected:
  gyVertexElement* elements;
  uint32_t count;
};

// ******************************************************************************************************************************
// MeshBuffer
// ******************************************************************************************************************************
class gyMeshBuffer : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gyMeshBuffer();
  ~gyMeshBuffer();

  int Init( gyIDVertexLayout vLayout, gyPrimitiveTopology primTopology=PT_TRILIST, gyIndexType indexType=INDEX_NONE );
  int32_t CreateVertices( uint32_t count, void* initData=0, uint32_t vertexStride=0 );
  int32_t CreateIndices( uint32_t count, void* initData=0 );
  int32_t FillVertices( uint32_t count, void* data );
  int32_t FillIndices( uint32_t count, void* data );
  int32_t MapVertices( void** pData );
  void  UnmapVertices( );

  uint32_t GetVertexStride(){ return vertexStride; }
  uint32_t GetVertexCount(){ return vertexCount; }
  uint32_t GetIndexCount(){ return indexCount; }
  gyPrimitiveTopology GetPrimitiveTopology()const{ return (gyPrimitiveTopology)primTopo; }
  gyIndexType GetIndexFormat()const{ return (gyIndexType)indexType; }
  gyIDVertexLayout GetVertexLayout(){ return vertexLayout; } 

protected:
  gyIDVertexLayout vertexLayout;
  uint32_t vertexStride;
  uint32_t vertexCount;
  uint32_t indexCount;
  uint16_t primTopo;
  uint16_t indexType;
};

// ******************************************************************************************************************************
// ShaderConstant
// ******************************************************************************************************************************
struct gyShaderConstant
{
#ifdef _DEBUG
  enum { MAX_CONSTANT_NAME = 128 };
#endif
  gyShaderConstant() : nameHash(0), index(-1), resNumber(gyResourceID<0>::MAXNUMB), type((int8_t)SCT_NONE), size(0)
  { 
#ifdef _DEBUG
    *name=0; 
#endif
  }

  bool IsInvalid()const{ return index==-1 || type==SCT_NONE || size==0 || nameHash==0;}
  bool IsTexture()const{ return type>=SCT_TEXTURE && type<=SCT_TEXTURE2DARRAY; }
  bool IsSampler()const{ return type>=SCT_SAMPLER; }
  bool IsFloatArray()const{ return type>=SCT_FLOAT && type<=SCT_FLOAT43; }

#ifdef _DEBUG
  char name[128];
#endif
  uint32_t nameHash;
  int32_t index;
  uint32_t resNumber;   // only used when IsTexture() or IsSampler(). it is the resource number
  int8_t  type;         // eShaderConstanType
  uint8_t size;
  // padding, makes sense to pack(1)? makes sense to use a dirty flag here?
};

// ******************************************************************************************************************************
// ShaderConstantBuffer
// ******************************************************************************************************************************
class gyShaderConstantBuffer
{
  GY_PIMPL_CLASS();
public:
  gyShaderConstantBuffer();
  int Init( uint32_t ccount );
  void Destroy();
  int32_t SetConstantValue( int32_t constantNdx, const float* values, uint32_t count );
  int32_t SetConstantValue( int32_t constantNdx, const bool* values, uint32_t count );
  int32_t SetConstantValue( int32_t constantNdx, const int* values, uint32_t count );
  int32_t SetConstantValue( int32_t constantNdx, gyIDTexture tex, uint32_t count=1 );
  int32_t SetConstantValue( int32_t constantNdx, gyIDSamplerState sid, uint32_t count=1 );

  uint32_t GetConstantsCount(){ return constantsCount; }
  int32_t  FindConstantIndexByName( const char* name );
  const gyShaderConstant& GetConstant( int32_t index ) { return constants[index]; }

protected:
  gyShaderConstant* constants; // textures only at the beginning of this array
  uint32_t  constantsCount;
};

struct gyShaderSourceDesc
{
  gyShaderModel shaderModel;
  const char* entryPoint;
  uint8_t* sourceData;
  uint32_t sourceLength;

  static gyShaderSourceDesc MakeNull() { gyShaderSourceDesc ssd = { SM_NONE, 0, 0, 0 }; return ssd; }
  static bool IsNull( const gyShaderSourceDesc& ssd ){ return ssd.shaderModel == SM_NONE || !ssd.entryPoint || !ssd.sourceData || !ssd.sourceLength; }
};
#define GY_MAKENULL_SHADERDESC gyShaderSourceDesc::MakeNull()

class gyShaderByteCode : public gyRefCounted
{
  GY_PIMPL_CLASS();
public:
  gyShaderByteCode();
  ~gyShaderByteCode();
  uint8_t* GetByteCode();
  uint32_t GetLength(); 
  
  gyShaderType shaderType;
  gyShaderModel shaderModel;
};

class gyShader : public gyResourceBase
{
public:
  gyShader(gyShaderType st=ST_NONE, gyShaderModel sm=SM_NONE);
  ~gyShader();
  virtual int32_t Init( gyShaderByteCode* pByteCode );

  gyShaderType GetType(){ return shaderType; }
  gyShaderModel GetShaderModel(){ return shaderModel; }
  gyShaderConstantBuffer* GetConstantBuffer( ){ return constantBuffer; }
  gyShaderByteCode* GetByteCode(){ return byteCode; }

protected:
  gyShaderConstantBuffer* constantBuffer;
  gySharedPtr<gyShaderByteCode> byteCode;
  gyShaderType shaderType;
  gyShaderModel shaderModel;
};

class gyShaderTechnique
{
public:
  gyShaderTechnique();
  void Destroy();
  gyIDShader GetStageShader( gyShaderType stage ){ return stageShaders[stage]; }
  void    SetStageShader( gyShaderType stage, gyIDShader shader );

protected:
  gyIDShader stageShaders[ST_MAXSTAGES];
};

class gyTexture : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gyTexture();
  ~gyTexture();
  int Init( gyTextureType type, uint8_t* data, uint32_t size, uint32_t flags = 0 );
  int Init( gyTextureType type, const char* filename, uint32_t flags = 0 );
  int Init( gyTextureType type, gyComponentFormat texf, uint8_t* data, uint32_t width, uint32_t height, uint32_t flags = 0 );
  int Init( gyIDRenderTarget rtid );

protected:
  uint32_t creationFlags;
};

struct gySamplerState : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gySamplerState();
  ~gySamplerState();
  int Init( gyFilterType filter, gyAddress u, gyAddress v, gyAddress w, float lodBias, uint8_t maxAniso, const float* bordercol, gyComparisonFunc compFunc);

  inline void GetBorderColor(float* rgba)const
  { 
    uint32_t r,g,b,a; 
    gyColor_PackedRGBAToRGBA(borderCol,r,g,b,a); 
    rgba[0]=r/255.0f; rgba[1]=g/255.0f; 
    rgba[2]=b/255.0f; rgba[3]=a/255.0f; 
  }
  inline void GetLOD( float* _min, float* _max )const{ *_min=minLOD; *_max=maxLOD; }
  inline float GetMipLODBias() const{ return mipLODBias/10.0f; }
  inline gyFilterType GetFilter()const{ return (gyFilterType)filter; }
  inline gyComparisonFunc GetCompFunction()const{ return (gyComparisonFunc)compFunc; }
  inline uint8_t GetMaxAniso()const { return maxAniso; }
  inline void GetAddresses( gyAddress* u, gyAddress* v, gyAddress* w ) const
  { 
    *u = gyAddress((address>>6)&0x7); 
    *v = gyAddress((address>>3)&0x7); 
    *w = gyAddress((address&0x7));
  }
  inline void SetBorderColor(const float* rgba){ borderCol = gyColor_FloatRGBAToPackedRGBA( rgba[0], rgba[1], rgba[2], rgba[3] ); }
  inline void SetLOD( float _min, float _max ){ minLOD=_min; maxLOD=_max; }
  inline void SetMipLODBias( float mlb ){ mipLODBias = uint8_t(mlb* 10.0f); }
  inline void SetFilter(gyFilterType filt){ filter=(uint8_t)filt; }
  inline void SetCompFunction( gyComparisonFunc cfunc ){ compFunc=(uint8_t)cfunc;}
  inline void SetMaxAniso(uint8_t ma){ maxAniso = ma; }
  inline void SetAddresses( gyAddress u, gyAddress v, gyAddress w )
  { 
    address = ((u&0x7)<<6)|((v&0x7)<<3)|(w&0x7); 
  }

  gy_override uint32_t ComputeHash(){ return gyComputeHash( (void*)&borderCol, 18 ); }
protected:
  uint32_t borderCol;   // rgba 8 bits per channel ( what about sampling hdr? )
  float  minLOD;
  float  maxLOD;       
  uint16_t address;      // 3 bits for each
  uint8_t  mipLODBias;   // from 0.0 to 25.5f  (1 decimal) x10 and /10 to convert
  uint8_t  filter;       // up to 256 different filters  
  uint8_t  compFunc;     // up to 256 different functions
  uint8_t  maxAniso;     // from 0 to 256
};

struct gyRasterState : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gyRasterState();
  ~gyRasterState();
  int Init( gyFillMode fm=FM_SOLID, gyCullMode cm=CULL_BACK, int32_t dbias=0, float dbclamp=0, float ssdb=0, uint8_t flags=RASTER_DEPTHCLIP_ENABLED);
  gy_override uint32_t ComputeHash(){ return gyComputeHash( (void*)&depthBias, 15 ); }

  inline void SetFillMode(gyFillMode fm){ fillMode = (uint8_t)fm; }
  inline void SetCullMode(gyCullMode cm){ cullMode = (uint8_t)cm; }
  inline void SetDepthBias(int32_t db){ depthBias = db; }
  inline void SetDepthBiasClamp(float dbc){ depthBiasClamp = dbc; }
  inline void SetSlopeScaledDepthBias(float ssdb){ slopeScaledDepthBias = ssdb; }
  inline void SetFlags(uint8_t f){ flags = f; }

  inline gyFillMode GetFillMode()const{ return (gyFillMode)fillMode; }  
  inline gyCullMode GetCullMode()const{ return (gyCullMode)cullMode; }
  inline int32_t GetDepthBias()const{ return depthBias; }
  inline float GetDepthBiasClamp()const{ return depthBiasClamp; }
  inline float GetSlopeScaledDepthBias()const{ return slopeScaledDepthBias; }
  inline uint8_t GetFlags()const{ return flags; }

protected:
  int32_t depthBias;            // depth value added to a pixel
  float depthBiasClamp;       // max depth bias of a pixel
  float slopeScaledDepthBias; // scalar on a given pixel slope
  uint8_t fillMode;             // eFillMode
  uint8_t cullMode;             // eCullMode
  uint8_t flags;                // eRasterFlags  
};

struct gyDepthStencilState : public gyResourceBase
{
  GY_PIMPL_CLASS();
public:
  gyDepthStencilState();
  ~gyDepthStencilState();
  int Init(bool depthTest, bool depthWrite, gyComparisonFunc depthFunc, 
          bool stencilTest, uint8_t stencilReadMask, uint8_t stencilWriteMask,
          gyStencilOp frontFail, gyStencilOp frontDepthFail, gyStencilOp frontPass, gyComparisonFunc frontFunc,
          gyStencilOp backFail, gyStencilOp backDepthFail, gyStencilOp backPass, gyComparisonFunc backFunc);  

  gy_override uint32_t ComputeHash(){ return gyComputeHash( (void*)&front, 4*sizeof(uint32_t) ); }

  enum eFace{ FRONTFACE=0,BACKFACE=1};
  // depth
  inline void SetDepthEnabled(bool e){ gyBits::PackByte(depth,2,int(e)*0xff); }
  inline void SetDepthWrite(bool w){ gyBits::PackByte(depth,1,int(w)*0xff); }
  inline void SetDepthFunc(gyComparisonFunc f){ gyBits::PackByte(depth,0,(uint8_t)f); }
  // stencil
  inline void SetStencilTest(bool e){ gyBits::PackByte(stencil,2,int(e)*0xff); }
  inline void SetStencilReadMask(uint8_t rm){ gyBits::PackByte(stencil,1,rm); }
  inline void SetStencilWriteMask(uint8_t wm){ gyBits::PackByte(stencil,0,wm); }
  inline void SetStencilOp( eFace face, gyStencilOp op, gyStencilOp depthOp, gyStencilOp passOp, gyComparisonFunc func )
  {
    uint32_t& ref= (face==FRONTFACE)?front:back;
    gyBits::PackByte( ref, 0, (uint8_t)func );
    gyBits::PackByte( ref, 1, (uint8_t)passOp );
    gyBits::PackByte( ref, 2, (uint8_t)depthOp );
    gyBits::PackByte( ref, 3, (uint8_t)op );
  }

public:
  inline bool GetDepthTestEnabled()const{ return gyBits::UnpackByte(depth,2)!=0; }
  inline bool GetDepthWriteEnabled()const{ return gyBits::UnpackByte(depth,1)!=0; }
  inline gyComparisonFunc GetDepthFunc()const{ return (gyComparisonFunc)gyBits::UnpackByte(depth,0); }
  inline bool GetStencilTest()const{ return gyBits::UnpackByte(stencil,2)!=0; }
  inline uint8_t GetStencilReadMask()const{ return gyBits::UnpackByte(stencil,1); }
  inline uint8_t GetStencilWriteMask()const{ return gyBits::UnpackByte(stencil,0); }
  inline void GetStencilOp( eFace face, gyStencilOp* op, gyStencilOp* depthOp, gyStencilOp* passOp, gyComparisonFunc* func )const
  {
    uint32_t ref= (face==FRONTFACE)?front:back;
    *op = (gyStencilOp)gyBits::UnpackByte(ref,3);
    *depthOp = (gyStencilOp)gyBits::UnpackByte(ref,2);
    *passOp = (gyStencilOp)gyBits::UnpackByte(ref,1);
    *func = (gyComparisonFunc)gyBits::UnpackByte(ref,0);
  }
  
protected:
  uint32_t front;   // fail | depthFail | pass     | func
  uint32_t back;    // fail | depthFail | pass     | func
  uint32_t depth;   // 0x00 | test      | write    | func
  uint32_t stencil; // 0x00 | test      | readMask | writeMask
                  // 3    | 2         | 1        | 0
};

// ******************************************************************************************************************************
// Blend State 
// ******************************************************************************************************************************
struct gyBlendStateDesc
{
  gyBlendStateDesc(gyBlend src=BLEND_ONE, gyBlend dst=BLEND_ZERO, gyBlendOp op=BLENDOP_ADD, gyBlend srcA=BLEND_ONE, gyBlend dstA=BLEND_ZERO, gyBlendOp opA=BLENDOP_ADD, uint8_t rtwm=0x0f)
  {
    Set(src,dst,op,srcA,dstA,opA,rtwm);
  }

  void Set(gyBlend src, gyBlend dst, gyBlendOp op, gyBlend srcA, gyBlend dstA, gyBlendOp opA, uint8_t rtwm)
  {
    gyBits::PackByte(blend, 0, (uint8_t)op);
    gyBits::PackByte(blend, 1, (uint8_t)dst);
    gyBits::PackByte(blend, 2, (uint8_t)src);
    gyBits::PackByte(blend, 3, rtwm);
    gyBits::PackByte(blendA, 0, (uint8_t)opA);
    gyBits::PackByte(blendA, 1, (uint8_t)dstA);
    gyBits::PackByte(blendA, 2, (uint8_t)srcA);
  }

  gyBlend   Src() const { return (gyBlend)gyBits::UnpackByte(blend,2);}
  gyBlend   Dst() const { return (gyBlend)gyBits::UnpackByte(blend,1);}
  gyBlendOp Op()  const { return (gyBlendOp)gyBits::UnpackByte(blend,0);}
  gyBlend   SrcA()const { return (gyBlend)gyBits::UnpackByte(blendA,2);}
  gyBlend   DstA()const { return (gyBlend)gyBits::UnpackByte(blendA,1);}
  gyBlendOp OpA() const { return (gyBlendOp)gyBits::UnpackByte(blendA,0);}
  uint8_t   RTWriteMask() const { return gyBits::UnpackByte(blend,3); }

  uint32_t blend; // rtwm | src | dst | op   (rtwm=RenderTarget write mask, by default 0x0f)
  uint32_t blendA;// 00   | src | dst | op   
};

struct gyBlendState : public gyResourceBase
{
  GY_PIMPL_CLASS();
  gyBlendState();
  ~gyBlendState();
  gy_override uint32_t ComputeHash(){ return gyComputeHash( (void*)&desc, sizeof(gyBlendStateDesc)+sizeof(bool)*2 ); }
  int32_t Init(bool alphaToCov, gyBlendStateDesc* descs, uint32_t count);

  gyBlendStateDesc desc;
  bool alphaToCoverage;
  bool independentBlending;
};

#endif