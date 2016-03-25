#ifndef _GYMATH_H_
#define _GYMATH_H_

#ifdef GY_DOUBLE_PRECISION
typedef double gymathtype;
#else
typedef float gymathtype;
#endif
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------  
template<typename T>
struct gyv2t
{
  union
  {
    struct { T x,y; };
    struct { T u,v; };
    T data[2];
  };

  inline gyv2t(){}
  inline gyv2t(const T& x, const T& y ):x(x),y(y){}
  inline explicit gyv2t(const T& val):x(val),y(val){}

  inline static const gyv2t& ZERO(){ static gyv2t ze(0); return ze; }
  inline void Set( const T& _x, const T& _y ){ x=_x; y=_y; }
  inline void Set( const T* v ){ x=v[0]; y=v[1]; }
};
typedef gyv2t<int> gyv2i;
typedef gyv2t<float> gyv2f;
typedef gyv2t<double> gyv2d;
#ifdef GY_DOUBLE_PRECISION
typedef gyv2d gyv2;
#else
typedef gyv2f gyv2;
#endif
// ------------------------------------------------------------------------------------  
// ------------------------------------------------------------------------------------  
template<typename T>
struct gyv3t
{
  union
  {
    struct { T x,y,z; };
    struct { T r,g,b; };
    struct { T u,v,w; };
    T data[3];
  };

  inline gyv3t(){}
  inline gyv3t(const T& x, const T& y, const T& z ):x(x),y(y),z(z){}
  inline explicit gyv3t(const T& val):x(val),y(val),z(val){}
  inline void Set( const T& _x, const T& _y, const T& _z ){ x=_x; y=_y; z=_z; }
  inline void Set( const T* v ){ x=v[0]; y=v[1]; z=v[2]; }
  inline const gyv3t operator -(){ return gyv3t(-x,-y,-z); }
  inline static const gyv3t& ZERO(){ static gyv3t ze(0); return ze; }
};
typedef gyv3t<int> gyv3i;
typedef gyv3t<float> gyv3f;
typedef gyv3t<double> gyv3d;
#ifdef GY_DOUBLE_PRECISION
typedef gyv3d gyv3;
#else
typedef gyv3f gyv3;
#endif
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
template<typename T>
struct gyv4t
{
  union
  {
    struct { T x,y,z,w; };
    struct { T r,g,b,a; };
    T data[4];
  };

  inline gyv4t(){}
  inline gyv4t(const T& x, const T& y, const T& z, const T& w ):x(x),y(y),z(z),w(w){}
  inline explicit gyv4t(const T& val):x(val),y(val),z(val),w(val){}

  inline static const gyv4t& ZERO(){ static gyv4t ze(0.0f); return ze; }
};
typedef gyv4t<int> gyv4i;
typedef gyv4t<float> gyv4f;
typedef gyv4t<double> gyv4d;
#ifdef GY_DOUBLE_PRECISION
typedef gyv4d gyv4;
#else
typedef gyv4f gyv4;
#endif

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
template<typename T>
struct gyquatt
{
  union
  {
    struct { T x,y,z,w; };
    T data[4];
  };

  inline gyquatt(){}
  inline gyquatt(const T& x, const T& y, const T& z, const T& w ):x(x),y(y),z(z),w(w){}

  inline static const gyquatt& IDENTITY(){ static gyquatt i(0,0,0,1); return i; }
};

typedef gyquatt<float> gyquatf;
typedef gyquatt<double> gyquatd;
#ifdef GY_DOUBLE_PRECISION
typedef gyquatd gyquat;
#else
typedef gyquatf gyquat;
#endif

// ------------------------------------------------------------------------------------
// column-major, LH
// ------------------------------------------------------------------------------------
template<typename T>
struct gymat44t
{
  union
  {
    struct { T _00,_01,_02,_03,_10,_11,_12,_13,_20,_21,_22,_23,_30,_31,_32,_33; };
    T data[16];
    T col[4][4];
  };
  inline gymat44t(){}
  inline gymat44t(T _00,T _01,T _02,T _03,T _10,T _11,T _12,T _13,
    T _20,T _21,T _22,T _23,T _30,T _31,T _32,T _33 )
    : _00(_00),_01(_01),_02(_02),_03(_03)
    , _10(_10),_11(_11),_12(_12),_13(_13)
    , _20(_20),_21(_21),_22(_22),_23(_23)
    , _30(_30),_31(_31),_32(_32),_33(_33){}
  inline static const gymat44t& IDENTITY()
  {
    static gymat44t i(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
    return i;
  }
  inline void GetXAxis( gyv3* xaxis )const{ xaxis->x = _00; xaxis->y = _01; xaxis->z = _02; }
  inline void GetYAxis( gyv3* yaxis )const{ yaxis->x = _10; yaxis->y = _11; yaxis->z = _12; }
  inline void GetZAxis( gyv3* zaxis )const{ zaxis->x = _20; zaxis->y = _21; zaxis->z = _22; }
  inline void GetTranslation( gyv3* t ){ t->x = _03; t->y = _13; t->z = _23; }
};

typedef gymat44t<float> gymat44f;
typedef gymat44t<double> gymat44d;
#ifdef GY_DOUBLE_PRECISION
typedef gymat44d gymat44;
#else
typedef gymat44f gymat44;
#endif

// ------------------------------------------------------------------------------------
// column-major, LH
// ------------------------------------------------------------------------------------
template<typename T>
struct gymat33t
{
  union
  {
    struct { T _00,_01,_02, _10,_11,_12, _20,_21,_22; };
    T data[9];
    T col[3][3];
  };
  inline gymat33t(){}
  inline gymat33t(T _00,T _01,T _02,T _10,T _11,T _12,T _20,T _21,T _22)
    : _00(_00),_01(_01),_02(_02)
    , _10(_10),_11(_11),_12(_12)
    , _20(_20),_21(_21),_22(_22){}
  inline static const gymat33t& IDENTITY()
  {
    static gymat33t i(1,0,0, 0,1,0, 0,0,1);
    return i;
  }
  inline void GetXAxis( gyv3* xaxis ){ xaxis->x = _00; xaxis->y = _01; xaxis->z = _02; }
  inline void GetYAxis( gyv3* yaxis ){ yaxis->x = _10; yaxis->y = _11; yaxis->z = _12; }
  inline void GetZAxis( gyv3* zaxis ){ zaxis->x = _20; zaxis->y = _21; zaxis->z = _22; }
};
typedef gymat33t<float> gymat33f;
typedef gymat33t<double> gymat33d;
#ifdef GY_DOUBLE_PRECISION
typedef gymat33d gymat33;
#else
typedef gymat33f gymat33;
#endif

// ------------------------------------------------------------------------------------
// Degrees <-> Radians
// ------------------------------------------------------------------------------------
template<typename T> struct gydegt;
template<typename T>
struct gyradt
{
  gyradt(){}
  explicit gyradt(T radvalue):valueInRadians(radvalue){}
  gyradt( const gydegt<T>& degvalue ):valueInRadians(degvalue.valueInDegrees*T(DEG2RADFACTORf)){}
  T GetDegrees(){ return valueInRadians*T(RAD2DEGFACTORf); }

  T valueInRadians;
};

template<typename T>
struct gydegt
{
  gydegt(){}
  explicit gydegt(float degvalue):valueInDegrees(degvalue){}
  gydegt( const gyradt<T>& radvalue):valueInDegrees(radvalue.valueInRadians*T(RAD2DEGFACTORf)){}
  T GetRadians(){ return valueInDegrees*T(DEG2RADFACTORf); }

  T valueInDegrees;
};

typedef gyradt<float> gyradf;
typedef gyradt<double> gyradd;
typedef gydegt<float> gydegf;
typedef gydegt<double> gydegd;
#ifdef GY_DOUBLE_PRECISION
typedef gyradd gyrad;
typedef gydegd gydeg;
#else
typedef gyradf gyrad;
typedef gydegf gyreg;
#endif


class gyMath
{
public:

  static const gymathtype PI;
  static const gymathtype RAD2DEGFACTOR;
  static const gymathtype DEG2RADFACTOR;
  static const float GYFLT_MAX;
  static const double GYDBL_MAX;
  static const double GYDBL_ABSTOLERANCE;
  static const double GYDBL_RELTOLERANCE;

  // ------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------
  // -- Scalar operations
  template<typename T> static inline T Max(const T& a, const T& b);
  template<typename T> static inline T Min(const T& a, const T& b);
  template<typename T> static inline T Clamp(const T& value, const T& minVal, const T& maxVal );
  template<typename T> static inline void Swap(T& a, T& b);
  template<typename T> static inline T Saturate(const T& value);
  template<typename T> static inline T Abs(const T& val);
  template<typename T> static inline bool IsRealEqual (T a, T b, T epsilon=T(0));
  template<typename T> static inline bool IsZero (T a, T epsilon=T(0.00001));
  static inline bool Equal(double a, double b);
  static inline void ExtractEulerAngles( const gymat44& m, gymathtype* x, gymathtype* y, gymathtype* z );
  template<typename T> static inline T SmoothStep( T edge0, T edge1, T x);

  // -- Vector operations
  static inline gymathtype VecLength( const gyv3& v );
  static inline gymathtype VecDot( const gyv3& a, const gyv3& b );
  static inline void VecCross( gyv3* out, const gyv3& a, const gyv3& b );
  static inline void VecMul( gyv3* out, const gyv3& v, gymathtype f );
  static inline void VecAdd( gyv3* out, const gyv3& a, const gyv3& b );
  static inline void VecSub( gyv3* out, const gyv3& a, const gyv3& b );
  static inline void VecMultAdd( gyv3* out, const gyv3& multv, gymathtype t, const gyv3& addv );
  static inline void VecNormalize( gyv3* out, const gyv3& v );
  static inline void VecNormalize( gyv3* inout );
  static inline void MatrixPerspectiveProjection( gymat44* out, const gyrad& fov, gymathtype aspect, gymathtype zNear, gymathtype zFar );
  static inline void MatrixOrthographicProjection( gymat44* out, gymathtype left, gymathtype right, gymathtype top, gymathtype bottom, gymathtype zNear, gymathtype zFar);
  static inline void MatrixLookAt( gymat44* out, const gyv3& eye, const gyv3& at, const gyv3& up=gyv3(0,1,0) );
  static inline void MatrixTranspose( gymat44* out );
  static inline void MatrixTranspose( gymat33* out );
  static inline void MatrixToPlatform( gymat44* out );
  static inline void MatrixRotX( gymat33* out, const gyrad& ang );
  static inline void MatrixRotY( gymat33* out, const gyrad& ang );
  static inline void MatrixRotZ( gymat33* out, const gyrad& ang );
  static inline void MatrixRotXY( gymat44* out, const gyrad& angX, const gyrad& angY );
  static inline void MatrixRotX( gymat44* out, const gyrad& ang );
  static inline void MatrixRotY( gymat44* out, const gyrad& ang );
  static inline void MatrixRotZ( gymat44* out, const gyrad& ang );
  static inline void MatrixTranslation( gymat44* out, const gyv3& t );
  static inline void MatrixTranslation( gymat44* out, const gyv2& t );
  static inline void MatrixMult( gymat44* out, const gymat44& a, const gymat44& b );
  static inline void MatrixMult( gymat33* out, const gymat33& a, const gymat33& b );
  static inline void MatrixConvert( gymat44* out, const gymat33& m );
  static inline void MatrixConvert( gymat33* out, const gymat44& m );
  static void ComputeTangentSpaceVectors( gyv3* tangent, gyv3* binormal, gyv3* normal,
    const gyv3& p0, const gyv3& p1, const gyv3& p2,
    const gyv2& t0, const gyv2& t1, const gyv2& t2 );
  

  // -- Triangulations
  // create two triangles clock wise, outData will have room for 6 points with two coordinates of type gymath (i.e. 6*sizeof(float))
  static void TriangulateRect(const gyv2& lefttop, const gyv2& widthheight, void* outData, int32_t stride=sizeof(gymathtype)*2, bool ccw=false);
};

#include <engine/common/math.inl>
#endif