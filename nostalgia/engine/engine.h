#ifndef _ENGINE_H_
#define _ENGINE_H_

//#define GY_DOUBLE_PRECISION
#include <engine/enginedefs.h>
#ifndef BX_PLATFORM_WINDOWS
# error Platform not implemented
#endif
#include <stdint.h>

#define GY_ENGINE_NAME "Nostalgia"
#define GY_ENGINE_VERSION "0.1"
#define GY_ENGINE_FULLNAME GY_ENGINE_NAME GY_ENGINE_VERSION

#ifdef BX_PLATFORM_WINDOWS
#include <windows.h>
#define GY_RENDER_D3D11
#endif

#ifdef BX_ARCH_64BIT
# define BX_PLATFORM_BITS 64
  typedef int64_t ptrType;
#else
# define BX_PLATFORM_BITS 32
  typedef int32_t ptrType;
#endif

#if defined(BX_PLATFORM_WINDOWS)
# define MAIN_FUNCTION int WINAPI wWinMain( HINSTANCE , HINSTANCE , LPWSTR , int )
#else

#endif

// *************************************************************************************************
// Macro helpers
// *************************************************************************************************
#define gy_override 
#define GYUNUSED(var) var=var;
#define SafeRelease(ptr) { if ( ptr ){ (ptr)->Release(); (ptr) = 0; } }
#define SafeDelete(ptr)  { if ( ptr ){ delete (ptr); (ptr) = 0; } }
#define SafeDestroyDelete(ptr){ if (ptr){ (ptr)->Destroy(); delete (ptr); } }
#define SafeFree(ptr)    { if ( ptr ){ free(ptr); (ptr)=0; } }
#define SafeDeleteA(ptr) { if ( ptr ){ delete[] (ptr); (ptr) = 0; } }
#define FlagIsSet(flags,f) ((flags)&(f))!=0
#define PtrSize (sizeof(void*))
#define gyColor_RGBAToPackedRGBA(r,g,b,a)  ( (r<<24)|((g&0xff0000)<<16)|((b&0xff00)<<8)|(a&0xff) );
#define gyColor_FloatRGBAToPackedRGBA(r,g,b,a) gyColor_RGBAToPackedRGBA( uint8_t(r*255), uint8_t(g*255), uint8_t(b*255), uint8_t(a*255) )
#define gyColor_PackedRGBAToRGBA(c,r,g,b,a) { r=(c&0xff000000)>>24; g=(c&0xff0000)>>16; b=(c&0xff00)>>8; a=(c&0xff); }
inline void gyColor_PackedRGBAToFloatRGBA(uint32_t pkColor, float* dstRGBA)
{
  uint8_t r,g,b,a; 
  gyColor_PackedRGBAToRGBA(pkColor,r,g,b,a); 
  dstRGBA[0]=r/255.0f; 
  dstRGBA[1]=g/255.0f; 
  dstRGBA[2]=b/255.0f; 
  dstRGBA[3]=a/255.0f;
}
#define R_OK (0)
#define GY_HASH_SEED 37
#define GY_PIMPL_CLASS() public: class Impl; protected: friend class Impl; Impl* impl; public: Impl* GetImpl(){ return impl; }

#define GYINFO(msg,...) gyGetLogManager().Message(GY_LOG_INFO,msg,__VA_ARGS__)
#define GYWARNING(msg,...) gyGetLogManager().Message(GY_LOG_WARNING,msg,__VA_ARGS__)
#define GYERROR(msg,...) gyGetLogManager().Message(GY_LOG_ERROR,msg,__VA_ARGS__)
#define GYDEBUG(msg,...) gyGetLogManager().Message(GY_LOG_DEBUG,msg,__VA_ARGS__)

#define GYDEBUGRET(str,val) { GYDEBUG(str); return val; }

void gyAssert( bool cond, const char* file, int line, const char* condstr, const char* msg );
#ifdef _DEBUG
# define ASSERT_MSG(c,m) gyAssert( c, __FILE__, __LINE__, #c, m )
# define FAIL_IF(c,m) ASSERT_MSG( !(c), m )
# define FAIL(m) FAIL_IF(true,m)
# define R_FAIL_IF(c,m) FAIL_IF(c,m)
# define RV_FAIL_IF(c,m) FAIL_IF(c,m)
# define FAIL_ALWAYS(m) FAIL_IF(true,m)
#else
# define ASSERT_MSG(c,m) 
# define FAIL_IF(c,m) 
# define FAIL(m) 
# define R_FAIL_IF(c,m) { if ( c ) { return -1; } }
# define RV_FAIL_IF(c,m) { if ( c ) { return; } }
# define FAIL_ALWAYS(m) gyAssert( false, __FILE__, __LINE__, "Failing always", m )
#endif
#define IMPLEMENT(m) FAIL_ALWAYS("IMPLEMENT! "#m)

// *************************************************************************************************
// GPU Markers and profilers
// *************************************************************************************************
//GPU markers
void gyPerfMarkerBegin( const char* name );
void gyPerfMarkerEnd();
#ifdef _DEBUG
struct _gyScopedMarker{ _gyScopedMarker(const char* n){ gyPerfMarkerBegin(n); } ~_gyScopedMarker(){ gyPerfMarkerEnd(); } };
#define GY_PERFMARKER_BEGIN(name) gyPerfMarkerBegin(name)
#define GY_PERFMARKER_END() gyPerfMarkerEnd()
#define GY_PERFMARKER_SCOPE(name) _gyScopedMarker _marker_(name);
#else
#define GY_PERFMARKER_BEGIN(name)
#define GY_PERFMARKER_END()
#define GY_PERFMARKER_SCOPE(name) 
#endif

// GPU profilers
void gyProfileGPUBegin( const char* name );
void gyProfileGPUEnd();
#if defined(_DEBUG) || defined(PROFILE)
struct _gyScopedGPUProfiler{ _gyScopedGPUProfiler(const char* n){ gyProfileGPUBegin(n); } ~_gyScopedGPUProfiler(){ gyProfileGPUEnd(); } };
#define GY_PROFILE_GPU_BEGIN(name) gyProfileGPUBegin(name)
#define GY_PROFILE_GPU_END()
#define GY_PROFILE_GPU_SCOPE(name) _gyScopedGPUProfiler _profgpu__(name);
#else
#define GY_PROFILE_GPU_BEGIN(name)
#define GY_PROFILE_GPU_END()
#define GY_PROFILE_GPU_SCOPE(name)
#endif

// *************************************************************************************************
// Misc functions
// *************************************************************************************************
void gySleep(int ms);
uint32_t gyComputeHash( const void* data, int32_t len, uint32_t seed=GY_HASH_SEED );
uint8_t* gyGetBigStackChunkST();
uint64_t gyGetBigStackChunkSTSize();
int gyAtomicIncrement(int32_t &i32);
int gyAtomicDecrement(int32_t &i32);
void gyAtomicAdd(int32_t& dest, int32_t value);
void gyAtomicAdd(int64_t& dest, int64_t value);
template<int N> const char* _gy_t_StringFormatST( const char* format, ... )
{
  static char tmp[N];
  va_list arglist;
  va_start (arglist, format);
  vsnprintf_s(tmp, N, format, arglist);
  va_end (arglist);
  return tmp;
}
#define gyStrFormat64 _gy_t_StringFormatST<64>
#define gyStrFormat256 _gy_t_StringFormatST<256>
#define gyStrFormat512 _gy_t_StringFormatST<512>
int gyGetCommandLine(char**& argv);
void gyFreeCommandLine();

#ifdef BX_PLATFORM_WINDOWS
bool gyWinIsVersionOrGreater(uint16_t major, uint16_t minor, uint16_t spmajor);
bool gyWinIsVistaOrGreater();
bool gyWinIs7SP1OrGreater();
#endif

// *************************************************************************************************
// gyResourceID
// *************************************************************************************************
template<int N>
struct gyResourceID
{
  enum { TYPEMASK = 0xff000000, TYPESHIFT=24, MAXNUMB=~TYPEMASK }; // 8 bits type, 24 bits number
  gyResourceID(){ Invalidate(); }
  explicit gyResourceID(uint32_t number){ Invalidate(); Number(number); }

  static inline gyResourceID<N> CreateFrom( uint32_t n ){gyResourceID<N> t;t.id=n;return t; }
  inline uint8_t   Type()const { return  (id&TYPEMASK)>>TYPESHIFT;}
  inline uint32_t  Number()const { return (id&(~TYPEMASK));}
  inline void    Number(uint32_t n){ id= (id&TYPEMASK)|(n&(~TYPEMASK));}
  inline bool    IsValid()const { return (id&(~TYPEMASK))!=(~TYPEMASK); }
  inline void    Invalidate(){ id = (N<<TYPESHIFT)|(~TYPEMASK); }
  inline bool    operator ==(const gyResourceID<N>& o ){ return id==o.id; }
  inline operator uint32_t(){ return id; }
  static gyResourceID<N> INVALID(){ return gyResourceID<N>();} // an invalid id has the 24 lsb to 1 
  static uint32_t ExtractType( uint32_t resId ){ return (resId&TYPEMASK)>>TYPESHIFT; }
  static uint32_t ExtractNumber(uint32_t resId){ return (resId&(~TYPEMASK)); }
private:
  uint32_t  id;
};

enum gyBaseResourceTypes
{
  // BASE
  ID_GENERIC = 0,  
  ID_FILE,
  ID_JSON,
  ID_LOGWRITER,

  // RENDERER
  ID_RENDERTARGET,
  ID_VERTEXLAYOUT,
  ID_MESHBUFFER,
  ID_TEXTURE,
  ID_SHADER,
  ID_SAMPLERSTATE,
  ID_DEPTHSTENCILSTATE,
  ID_RASTERSTATE,
  ID_BLENDSTATE,

  // INPUT
  ID_INPUTTRIGGER,

  // JOB MANAGER
  ID_JOB,

  // SOUND MANAGER
  ID_SOUND,

  // DYNAMIC LIBRARIES
  ID_DYNLIB,

  // NETWORK
  ID_SOCKET,

  // PHYSICS

  ID_BASERESOURCEID_MAX
};

typedef gyResourceID<ID_GENERIC>            gyIDGeneric;
typedef gyResourceID<ID_FILE>               gyIDFile;
typedef gyResourceID<ID_JSON>               gyIDJson;
typedef gyResourceID<ID_LOGWRITER>          gyIDLogWriter;
typedef gyResourceID<ID_RENDERTARGET>       gyIDRenderTarget;
typedef gyResourceID<ID_VERTEXLAYOUT>       gyIDVertexLayout;
typedef gyResourceID<ID_MESHBUFFER>         gyIDMeshBuffer;
typedef gyResourceID<ID_TEXTURE>            gyIDTexture;
typedef gyResourceID<ID_SHADER>             gyIDShader;
typedef gyResourceID<ID_SAMPLERSTATE>       gyIDSamplerState;
typedef gyResourceID<ID_DEPTHSTENCILSTATE>  gyIDDepthStencilState;
typedef gyResourceID<ID_RASTERSTATE>        gyIDRasterState;
typedef gyResourceID<ID_BLENDSTATE>         gyIDBlendState;
typedef gyResourceID<ID_INPUTTRIGGER>       gyIDInputTrigger;
typedef gyResourceID<ID_JOB>                gyIDJob;
typedef gyResourceID<ID_SOUND>              gyIDSound;
typedef gyResourceID<ID_DYNLIB>             gyIDDynLib;
typedef gyResourceID<ID_SOCKET>             gyIDSocket;


// *************************************************************************************************
// Shared ptr
// *************************************************************************************************
class gyRefCounted
{
public:
  gyRefCounted() : refCount(0){ }
  gyRefCounted( const gyRefCounted &other ) : refCount(0) { }  
  virtual ~gyRefCounted() { FAIL_IF(refCount!=0 , "Tried to delete a reference counted object with refcount!=0"); }
  gyRefCounted& operator=( const gyRefCounted& other){ return *this; }
  /*virtual*/ void DeleteThis() { delete this; }
  inline unsigned int AddRef() { return gyAtomicIncrement(refCount); }
  inline int GetRefCount() const { return refCount; }
  
  inline virtual int Release()
  {
    int iRefCount = DecRef();
    if (iRefCount == 0)
      DeleteThis();
    return iRefCount;
  }

  inline int DecRef()
  {
    FAIL_IF(refCount<=0, "Invalid refcount");
    return gyAtomicDecrement(refCount);
  }

protected:
  int refCount;
};

template<class C> class gySharedPtr
{
public:
  inline gySharedPtr() : ptr(NULL) { }
  inline gySharedPtr(C* pPtr) : ptr(NULL) { Set(pPtr); }
  inline gySharedPtr(const gySharedPtr<C>& other) : ptr(NULL) { Set(other.GetPtr()); }
  inline ~gySharedPtr() { SafeRelease(ptr); }
  inline gySharedPtr<C>& operator=(const gySharedPtr<C> &other) { Set(other.GetPtr()); return *this; }
  inline gySharedPtr<C>& operator=(C *pPtr) { Set(pPtr); return *this; }
  inline void Set(C *pPtr)
  {
    if (ptr == pPtr)
      return;
    C* pOldPtr = ptr;
    ptr = pPtr;
    if (pPtr)
      pPtr->AddRef();
    SafeRelease(pOldPtr);
  }

  inline operator C*() const { return ptr; }
  inline C* operator->() const { return ptr; }
  inline C* GetPtr() const { return ptr; }

  C* ptr;  ///< pointer to the object
};

// *************************************************************************************************
class gyRenderer;
class gyRendererFactory;
class gyFileSystem;
class gyJobManager;
class gyInput;
class gyApp;
class gyTimer;
class gyVarDict;
class gyReflect;
class gyVarDict;
class gySoundManager;
class gyDynLibManager;
class gyNetManager;
class gyPhysicsManager2D;
class gyLogManager;

// *************************************************************************************************
// *************************************************************************************************
class gyEngine
{
public:
  static gyEngine& GetInstance();
  gyEngine();
  ~gyEngine();

  int Create(const gyVarDict& params);
  void Release();
  int Run();

  gyRenderer& GetRenderer();
  gyFileSystem& GetFileSystem();
  gyJobManager& GetJobManager();
  gyInput& GetInputSystem();
  gyApp& GetApplication();
  gyTimer& GetTimer();
  gyReflect& GetReflection();
  gyVarDict& GetCommonDict();
  gySoundManager& GetSoundManager();
  gyDynLibManager& GetDynLibManager();
  gyNetManager& GetNetManager();
  gyPhysicsManager2D& GetPhysicsManager2D();
  gyLogManager& GetLogManager();

private:
  int PrepareCreateParams(gyVarDict& params);
  int CreateModules(gyVarDict& params);

  gyRenderer* renderer;
  gyFileSystem* fileSystem;
  gyJobManager* jobManager;
  gyInput* inputSystem;
  gyApp* application;
  gyTimer* timer;
  gyReflect* reflect;
  gyVarDict* commonDict;
  gySoundManager* soundManager;
  gyDynLibManager* dynLibManager;
  gyNetManager* netManager;
  gyPhysicsManager2D* physicsManager2D;
  gyLogManager* logManager;
};

inline gyRenderer& gyGetRenderer(){ return gyEngine::GetInstance().GetRenderer(); }
#define gyGetRendererFactory() gyGetRenderer().GetFactory()
inline gyFileSystem& gyGetFileSystem(){ return gyEngine::GetInstance().GetFileSystem(); }
inline gyJobManager& gyGetJobManager(){ return gyEngine::GetInstance().GetJobManager(); }
inline gyInput& gyGetInputSystem(){ return gyEngine::GetInstance().GetInputSystem(); }
inline gyApp& gyGetApplication(){ return gyEngine::GetInstance().GetApplication(); }
inline gyTimer& gyGetTimer(){ return gyEngine::GetInstance().GetTimer(); }
inline gyReflect& gyGetReflect(){ return gyEngine::GetInstance().GetReflection(); }
inline gyVarDict& gyGetCommonDict(){ return gyEngine::GetInstance().GetCommonDict(); }
inline gySoundManager& gyGetSoundManager(){ return gyEngine::GetInstance().GetSoundManager(); }
inline gyDynLibManager& gyGetDynLibManager(){ return gyEngine::GetInstance().GetDynLibManager(); }
inline gyNetManager& gyGetNetManager(){ return gyEngine::GetInstance().GetNetManager(); }
inline gyPhysicsManager2D& gyGetPhysicsManager2D(){ return gyEngine::GetInstance().GetPhysicsManager2D(); }
inline gyLogManager& gyGetLogManager(){ return gyEngine::GetInstance().GetLogManager(); }
#endif // #ifndef _engine-H-