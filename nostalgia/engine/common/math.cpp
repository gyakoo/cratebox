#include <stdafx.h>
#include <engine/common/math.h>

#ifdef GY_DOUBLE_PRECISION
const double gyMath::PI = 3.1415926535897932384626433832795;
const double gyMath::RAD2DEGFACTOR= 180.0 / gyMath::PI;
const double gyMath::DEG2RADFACTOR = gyMath::PI / 180.0;
#else
const float gyMath::PI = 3.1415926535897932384626433832795f;
const float gyMath::RAD2DEGFACTOR= 180.0f / gyMath::PI;
const float gyMath::DEG2RADFACTOR = gyMath::PI / 180.0f;
#endif

const double gyMath::GYDBL_MAX = 1.7976931348623158e+308;
const float gyMath::GYFLT_MAX = 3.402823466e+38F;
const double gyMath::GYDBL_ABSTOLERANCE = 0.000001;
const double gyMath::GYDBL_RELTOLERANCE = 0.000001;

void gyMath::TriangulateRect(const gyv2& lefttop, const gyv2& size, void* outData, int32_t stride,bool ccw/*=false*/)
{
# define T_ADVANCE(v,s) reinterpret_cast<gymathtype*>( reinterpret_cast<uint8_t*>(v)+s);
  if ( !outData || stride <= 0 ) return;

  // todo: handle this better
#ifdef GY_RENDER_D3D11 
  gymathtype _v[]={ lefttop.x, lefttop.y, lefttop.x+size.x, lefttop.y, lefttop.x+size.x, lefttop.y-size.y, lefttop.x, lefttop.y-size.y };
#endif
  if ( ccw )
  {
    Swap(_v[2], _v[6]);
    Swap(_v[3], _v[7]);
  }
  gymathtype* v = reinterpret_cast<gymathtype*>(outData);
  *v = _v[0]; *(v+1) = _v[1]; v = T_ADVANCE(v,stride);
  *v = _v[2]; *(v+1) = _v[3]; v = T_ADVANCE(v,stride);
  *v = _v[6]; *(v+1) = _v[7]; v = T_ADVANCE(v,stride);

  *v = _v[2]; *(v+1) = _v[3]; v = T_ADVANCE(v,stride);
  *v = _v[4]; *(v+1) = _v[5]; v = T_ADVANCE(v,stride);
  *v = _v[6]; *(v+1) = _v[7];
# undef T_ADVANCE
}
