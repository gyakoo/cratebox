#ifndef _RENDERERTYPES_H_
#define _RENDERERTYPES_H_

// (!) NOTE: Don't modify the order of the enumerations (!)

// ------------------------------------------------------------
// forward decls
// 
class gyRendererFactory;
class gyRenderer;
struct gyScreenQuad;
class gyRenderCommandBuffer;
class gyRenderContext;
class gyRenderTarget;
struct gyVertexElement;
class gyVertexLayout;
class gyMeshBuffer;
struct gyShaderConstant;
class gyShaderConstantBuffer;
struct gyShaderSourceDesc;
class gyShaderByteCode;
class gyShader;
class gyShaderTechnique;
class gyTexture;
struct gySamplerState;
struct gyRasterState;
struct gyDepthStencilState;
struct gyBlendState;
struct gyBlendStateDesc;

// ------------------------------------------------------------
// ------------------------------------------------------------
enum gyRendererTypes
{
  // misc
  TYPE_UNKNOW = 0,
  MAIN_RENDER_THREAD = 0,

  // RT types
  RT_CREATE_TEXTURE_2D      = 1<<0,
  RT_USED_AS_SHADER_TEXTURE = 1<<1,
  RT_CREATE_DEPTH_STENCIL   = 1<<2,
  RT_ALLFLAGS = (RT_CREATE_TEXTURE_2D|RT_USED_AS_SHADER_TEXTURE|RT_CREATE_DEPTH_STENCIL),

#ifdef BX_PLATFORM_WINDOWS
  MAX_RENDER_THREADS = 1, // at least 1 rendering thread (main)
  MAX_IA_VERTEXBUFFERS = 16,
  MAX_PS_TEXTUREUNITS = 128,
  MAX_PS_SAMPLERS = 16,
  MAX_OM_RENDERTARGETS = 8,
#else
#error Define these for this platform!
#endif
  // vertex elements (semantics...)
  MAX_VERTEXELEMENT_COUNT = 100,  
//   VSTREAM0 = 0,
//   VSTREAM1,
};
enum gyPrimitiveTopology
{
  PT_NONE=0,
  PT_TRILIST,
  PT_TRISTRIP,
  PT_MAX
};
enum gyIndexType
{
  INDEX_NONE=0,
  INDEX_16,
  INDEX_32,
  INDEX_MAX
};
enum gyVtxElmSemantic
{
  VES_UNKNOWN=0,
  VES_POSITION=1,
  VES_POSITIONTRANSFORMED,
  VES_NORMAL,
  VES_COLOR,
  VES_BINORMAL,
  VES_TANGENT,
  VES_TEXCOORD,
  VES_MAX,
};

enum gyComponentFormat
{
  FMT_UNKNOW=0,
  FMT_R32_F=1,              // 32bpc floats (F)
  FMT_R32G32_F,
  FMT_R32G32B32_F,
  FMT_R32G32B32A32_F,
  FMT_R32_SI,               // 32bpc signed-ints (SI)
  FMT_R32G32_SI,
  FMT_R32G32B32_SI,
  FMT_R32G32B32A32_SI,
  FMT_R32_UI,              // 32bpc unsigned-ints (UI)
  FMT_R32G32_UI,
  FMT_R32G32B32_UI,
  FMT_R32G32B32A32_UI,
  FMT_R8,                   // 8bpc
  FMT_R8G8,
  FMT_R8G8B8,
  FMT_R8G8B8A8,

  FMT_R24G8_UI,             // depth 24, stencil 8
  // << Update Renderer::GetComponentFormatSize() and others conversion methods when adding a new one here >>
  FMT_MAX, 
};

enum gyShaderModel
{
  SM_NONE = -1,
  SM_11 = 0, SM_12, SM_13, SM_14,                   // SM1
  SM_20, SM_2a, SM_2b,                              // SM2
  SM_30,                                            // SM3
  SM_40, SM_41, SM_40_FEATLEV_91, SM_40_FEATLEV_93, // SM4
  SM_50,                                            // SM5
  SM_MAX
};
enum gyShaderType
{
  ST_NONE=-1,
  ST_FIRSTSTAGE = 0,
  ST_VERTEXSHADER = 0,
# ifdef BX_PLATFORM_WINDOWS
  ST_HULLSHADER=1,
  ST_DOMAINSHADER=2,
  ST_GEOMETRYSHADER=3,
# endif
  ST_PIXELSHADER=4,
# ifdef BX_PLATFORM_WINDOWS
  ST_COMPUTESHADER=5,
# endif
  ST_MAXSTAGES
};

enum gyTextureFlags
{
  TF_NONE = 0,
  TF_RAWCREATION=1<<0,
};

enum gyTextureType
{
  TT_NONE=-1,
  TT_TEXTURE2D = 1<<0,
  TT_TEXTURE3D = 1<<1,
  TT_CUBEMAP   = 1<<2,
  TT_RENDERABLE = 1<<3,
  TT_TEXTURE2DARRAY = 1<<4
};

enum gyShaderConstanType 
{
  // !! DO NOT MODIFY THE ORDER !!
  SCT_NONE=-1,
  SCT_STRING,                                                 // string
  SCT_FLOAT, SCT_FLOAT2, SCT_FLOAT3, SCT_FLOAT4,              // vector floats
  SCT_FLOAT33, SCT_FLOAT44, SCT_FLOAT43,                      // matrices
  SCT_TEXTURE, SCT_CUBEMAP, SCT_TEXTURE3D, SCT_TEXTURE2DARRAY,// textures
  SCT_BOOL, SCT_BOOL2, SCT_BOOL3, SCT_BOOL4,                  // vector bools
  SCT_INT, SCT_INT2, SCT_INT3, SCT_INT4, SCT_UINT,            // vector ints
  SCT_SAMPLER,
};

enum gyRendererQueries
{
  QRY_NONE = 0,
  QRY_BACKBUFFER_DIM,         // dimensions of backbuffer (output=uint32*, two unsigned integer)
  QRY_SIMULTANEOUS_RT_COUNT,  // simultaneous render target count
};

enum gyClearStates
{
  // Clear render target flags
  CS_CLEAR_COLOR = 1<<0,
  CS_CLEAR_DEPTH = 1<<1,
  CS_CLEAR_STENCIL = 1<<2,
  CS_CLEAR_ALL = (CS_CLEAR_COLOR|CS_CLEAR_DEPTH|CS_CLEAR_STENCIL),
};

enum gyFilterType
{
  FT_NONE=0,
  FT_MIN_MAG_MIP_POINT,
  FT_MIN_MAG_POINT_MIP_LINEAR,
  FT_MIN_POINT_MAG_LINEAR_MIP_POINT,
  FT_MIN_POINT_MAG_MIP_LINEAR,
  FT_MIN_LINEAR_MAG_MIP_POINT,
  FT_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
  FT_MIN_MAG_LINEAR_MIP_POINT,
  FT_MIN_MAG_MIP_LINEAR,
  FT_ANISOTROPIC,
  FT_MAX,
};

enum gyAddress
{
  AD_NONE=0,
  AD_WRAP,
  AD_MIRROR,
  AD_CLAMP,
  AD_BORDER,
  AD_MIRRORONCE,
  AD_MAX,
};

enum gyFillMode
{
  FM_WIREFRAME=2,
  FM_SOLID=3
};

enum gyCullMode
{
  CULL_NONE=1,
  CULL_FRONT=2,
  CULL_BACK=3
};

enum gyRasterFlags
{
  RASTER_NONE=0,
  RASTER_COUNTERCLOCKWISE=1,
  RASTER_DEPTHCLIP_ENABLED=2,
  RASTER_SCISSOR_ENABLED=4,
  RASTER_MULTISAMPLE_ENABLED=8,
  RASTER_AALINE_ENABLED=16
};

enum gyComparisonFunc
{
  COMP_NONE=0,
  COMP_NEVER,
  COMP_LESS,
  COMP__EQUAL,
  COMP_LESS_EQUAL,
  COMP_GREATER,
  COMP_NOT_EQUAL,
  COMP_GREATER_EQUAL,
  COMP_ALWAYS,
  COMP_MAX
};

enum gyStencilOp
{
  SOP_NONE=0,
  SOP_KEEP,       // Keep the existing stencil data
  SOP_ZERO,       // Set the stencil data to 0
  SOP_REPLACE,    // Set the stencil data to the reference value in Renderer::SetDepthStencilState()
  SOP_INC_SAT,    // Increment the stencil value by 1, and clamp the result
  DOP_DEC_SAT,    // Decrement the stencil value by 1, and clamp the result
  SOP_INVERT,     // Invert the stencil data
  SOP_INC,        // Increment the stencil value by 1, and wrap the result if necessary
  SOP_DEC,        // Increment the stencil value by 1, and wrap the result if necessary
  SOP_MAX
};

enum gyBlend
{
  BLEND_ZERO               = 1,
  BLEND_ONE                = 2,
  BLEND_SRC_COLOR          = 3,
  BLEND_INV_SRC_COLOR      = 4,
  BLEND_SRC_ALPHA          = 5,
  BLEND_INV_SRC_ALPHA      = 6,
  BLEND_DEST_ALPHA         = 7,
  BLEND_INV_DEST_ALPHA     = 8,
  BLEND_DEST_COLOR         = 9,
  BLEND_INV_DEST_COLOR     = 10,
  BLEND_SRC_ALPHA_SAT      = 11,
  BLEND_BLEND_FACTOR       = 14,
  BLEND_INV_BLEND_FACTOR   = 15,
  BLEND_SRC1_COLOR         = 16,
  BLEND_INV_SRC1_COLOR     = 17,
  BLEND_SRC1_ALPHA         = 18,
  BLEND_INV_SRC1_ALPHA     = 19 
};

enum gyBlendOp
{
  BLENDOP_ADD            = 1,
  BLENDOP_SUBTRACT       = 2,
  BLENDOP_REVERSE_SUBTRACT = 3,
  BLENDOP_MIN            = 4,
  BLENDOP_MAX            = 5 
};

int gyGetComponentFormatSize(gyComponentFormat fmt);

#endif