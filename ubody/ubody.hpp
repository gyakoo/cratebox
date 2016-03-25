//
//
//
//
//
//
//
#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#ifndef UB_REAL
#define UB_REAL float
#endif

#if defined(__x86_64__) || defined(_M_X64)  ||  defined(__aarch64__)   || defined(__64BIT__) || \
	defined(__mips64)     || defined(__powerpc64__) || defined(__ppc64__)
#	define UB_PLATFORM_64
# define UB_ALIGNMENT 16
#else
#	define UB_PLATFORM_32
# define UB_ALIGNMENT 8
#endif //

#if defined(_DEBUG) || defined(DEBUG)
#ifdef _MSC_VER
#define UB_BREAK { __debugbreak(); }
#else
#define UB_BREAK { raise(SIGTRAP); }
#endif

#define UB_ASSERT(c) { if (!(c)){ UB_BREAK; } }
#else
#define UB_BREAK {(void*)0;}
#define UB_ASSERT(c)
#endif

#ifdef _MSC_VER
#define UB_BREAK_ALWAYS { __debugbreak(); }
#else
#define UB_BREAK_ALWAYS { raise(SIGTRAP); }
#endif

#if defined(WIN32) || defined(_WIN32) || (defined(sgi) && defined(unix) && defined(_MIPSEL)) || (defined(sun) && defined(unix) && !defined(_BIG_ENDIAN)) || (defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || (defined(__APPLE__) && defined(__LITTLE_ENDIAN__)) || (defined( _PowerMAXOS ) && (BYTE_ORDER == LITTLE_ENDIAN ))
#define UB_LITTLE_ENDIAN
#elif (defined(sgi) && defined(unix) && defined(_MIPSEB)) || (defined(sun) && defined(unix) && defined(_BIG_ENDIAN)) || defined(vxw) || (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)) || ( defined(__APPLE__) && defined(__BIG_ENDIAN__)) || (defined( _PowerMAXOS ) && (BYTE_ORDER == BIG_ENDIAN) )
#define UB_BIG_ENDIAN
#else
#  error unknown endian type
#endif

#ifdef UB_LITTLE_ENDIAN
void ub_swap16(void* d)
{ 
	unsigned char* b=(unsigned char*)d;
	const unsigned char t = b[0];
	b[0]=b[1]; b[1]=t;
}

void ub_swap32(void* d)
{ 
	unsigned char* b=(unsigned char*)d;
	const unsigned char t0 = b[0], t1 = b[1];
	b[0]=b[3]; b[1]=b[2];
	b[3]=t0;   b[2]=t1;
}

void ub_swap64(void* d)
{
	unsigned char* b=(unsigned char*)d;
	const unsigned char t0=b[0], t1=b[1], t2=b[2], t3=b[3];
	b[0]=b[7]; b[1]=b[6]; 
	b[2]=b[5]; b[3]=b[4];
	b[4]=t3; b[5]=t2;   
	b[6]=t1; b[7]=t0;   
}
#else
# define ub_swap16(d)
# define ub_swap32(d)
# define ub_swap64(d)
#endif

#define ub_offsetto(n,t)  ( (fltu8*)&((t*)(0))->n - (fltu8*)(0) )

// Memory functions override
#if defined(ub_malloc) && defined(ub_free) && defined(ub_calloc) && defined(ub_realloc)
// ok, all defined
#elif !defined(ub_malloc) && !defined(ub_free) && !defined(ub_calloc) && !defined(ub_realloc)
// ok, none defined
#else
#error "Must define all or none of ub_malloc, ub_free, ub_calloc, ub_realloc"
#endif

#ifdef UB_ALIGNED
#   ifndef ub_malloc
#     define ub_malloc(sz) _aligned_malloc(sz,UB_ALIGNMENT)
#   endif
#   ifndef ub_free
#     define ub_free(p) _aligned_free(p)
#   endif
#   ifndef ub_calloc
#     define ub_calloc(count,size) ub_aligned_calloc(count,size,UB_ALIGNMENT)
#   endif
#   ifndef ub_realloc
#     define ub_realloc(p,sz) _aligned_realloc(p,sz,UB_ALIGNMENT)
#   endif
#else
#   ifndef ub_malloc
#     define ub_malloc(sz) malloc(sz)
#   endif
#   ifndef ub_free
#     define ub_free(p) free(p)
#   endif
#   ifndef ub_calloc
#     define ub_calloc(count,size) calloc(count,size)
#   endif
#   ifndef ub_realloc
#     define ub_realloc(p,sz) realloc(p,sz)
#   endif
#endif
#define ub_strdup(s) ub_strdup_internal(s)
#define ub_safefree(p) { if (p){ ub_free(p); (p)=0;} }

////////////////////////////////////////////////////////////////////////////////////////////////
#define UB_FALSE 0
#define UB_TRUE 1
#ifndef UB_ARRAY_INITCAP
#	define UB_ARRAY_INITCAP 8
#endif
////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations of functions
////////////////////////////////////////////////////////////////////////////////////////////////
char* ub_strdup_internal(const char* str);
#ifdef UB_ALIGNED
void* ub_aligned_calloc(size_t nelem, size_t elsize, size_t alignment);
#endif

// -- Array
#define ub_array_type void*
struct ub_array;
typedef void (*ub_array_growpolicy)(ub_array*);
typedef struct ub_array
{
	ub_array_type* data;
	int size;
	int capacity;
	ub_array_growpolicy growpolicy;
}ub_array;
int ub_array_create(ub_array** arr, int capacity, ub_array_growpolicy grow);
void ub_array_destroy(ub_array** arr);
void ub_array_push_back(ub_array* arr, ub_array_type elem);
ub_array_type ub_array_pop_back(ub_array* arr);
ub_array_type ub_array_at(ub_array* arr, int index);
void ub_array_clear(ub_array* arr);
void ub_array_grow_double(ub_array* arr);
void ub_array_ensure(ub_array* arr, int count_new_elements); // makes sure there's room for new elms

#define UB_HANDLE_INVALID 0xffffffff
#define UB_CREATE_HANDLE(name) typedef struct UB_##name { unsigned int handle; } UB_##name; \
	static const UB_##name UB_NULL_##name = { UB_HANDLE_INVALID }
UB_CREATE_HANDLE(BODY);
UB_CREATE_HANDLE(WORLD);
UB_CREATE_HANDLE(SHAPE);

typedef struct ubody
{
	ub_array* worlds;
}ubody;

typedef struct ub_world_desc
{
	XMFLOAT3 gravity;
}ub_world_desc;

typedef struct ub_mass_desc
{
	XMFLOAT3X3 inertia;
	XMFLOAT3 com; // center of mass
	UB_REAL mass;
}ub_mass_desc;

typedef struct ub_body_desc
{
	ub_mass_desc mass_props;
	XMFLOAT3 position;	
	UB_SHAPE shape;
}ub_body_desc;

int ub_create(ubody** ub, int flags);
void ub_destroy(ubody** ub);
void ub_step(ubody* ub, UB_REAL step_time_s);

UB_WORLD ub_world_create(ubody* ub, ub_world_desc* wdesc );
void ub_world_destroy(ubody* ub, UB_WORLD hworld);
void ub_world_add_body(ubody* ub, UB_WORLD hworld, UB_BODY hbody);
void ub_world_remove_body(ubody* ub, UB_WORLD hworld, UB_BODY hbody);

UB_SHAPE ub_shape_create_sphere(ubody* ub, UB_REAL radius);
void ub_shape_destroy(ubody* ub, UB_SHAPE hshape);

UB_BODY ub_body_create(ubody* ub, ub_body_desc* bdesc);
void ub_body_destroy(ubody* ub, UB_BODY hbody);

void ub_util_compute_sphere_mass_desc(UB_REAL radius, UB_REAL mass, ub_mass_desc* massprops);
void ub_util_compute_box_mass_desc(XMFLOAT3* halfxtents, UB_REAL mass, ub_mass_desc* massprops);

#if defined(UB_IMPLEMENTATION)

typedef struct ub_world
{

}ub_world;

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
int ub_create(ubody** ub, int flags)
{
	*ub = (ubody*)ub_malloc(sizeof(ubody));
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
void ub_destroy(ubody** ub)
{
	assert( (*ub) != NULL );
	ub_safefree( *ub );
}


////////////////////////////////////////////////////////////////////////////////////////////////
// custom strdup implementation making use of optionally user defined malloc
////////////////////////////////////////////////////////////////////////////////////////////////
char* ub_strdup_internal(const char* str)
{
	const int len=(int)strlen(str);
	char* outstr=(char*)ub_malloc(len+1);
	memcpy(outstr, str, len+1);
	return outstr;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef UB_ALIGNED
void* ub_aligned_calloc(size_t nelem, size_t elsize, size_t alignment)
{
	void* mem;
	elsize *= nelem;
	mem=_aligned_malloc(elsize,alignment);
	if (mem)
		memset(mem,0,elsize);
	return mem;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// Array
////////////////////////////////////////////////////////////////////////////////////////////////
int ub_array_create(ub_array** arr, int capacity, ub_array_growpolicy grow)
{
	*arr = (ub_array*)ub_calloc(1,sizeof(ub_array));
	if ( !*arr ) return UB_FALSE;
	(*arr)->capacity = capacity>0?capacity:UB_ARRAY_INITCAP;
	(*arr)->size = 0;
	(*arr)->data = (ub_array_type*)ub_malloc(sizeof(ub_array_type)*capacity);
	if ( !(*arr)->data ) { ub_array_destroy(arr); return UB_FALSE; }
	if ( !grow ) grow = ub_array_grow_double;
	(*arr)->growpolicy = grow;
	return UB_TRUE;
}

void ub_array_destroy(ub_array** arr)
{
	if ( !arr || !*arr ) return;
	ub_safefree((*arr)->data);
	ub_free(*arr);
	*arr=NULL;
}

void ub_array_grow_double(ub_array* arr)
{
	int newcap = arr->capacity*2;
	arr->data = (ub_array_type*)ub_realloc(arr->data, sizeof(ub_array_type)*newcap);
	if( !arr->data ) { UB_BREAK; return; }
	arr->capacity = newcap;
}

void ub_array_push_back(ub_array* arr, ub_array_type elem)
{
	if (arr->size>=arr->capacity)
		arr->growpolicy(arr);    
	arr->data[arr->size++]=elem;
}

ub_array_type ub_array_pop_back(ub_array* arr)
{
	if ( arr->size==0 ) return NULL;
	return arr->data[--arr->size];
}

ub_array_type ub_array_at(ub_array* arr, int index)
{
	return index >= 0 && index < arr->size ? arr->data[index] : NULL;
}

void ub_array_clear(ub_array* arr)
{
	arr->size=0;
}

void ub_array_ensure(ub_array* arr, int cout_new_elements)
{
	const int newcap = arr->size+cout_new_elements;
	if ( newcap <= arr->capacity ) return;
	arr->capacity = newcap;
	ub_array_grow_double(arr); // ensure makes use directly of grow_double policy
}

#endif