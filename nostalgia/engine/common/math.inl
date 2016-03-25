template<typename T> 
inline T gyMath::Max(const T& a, const T& b){ return a>=b?a:b; }

template<typename T> 
inline T gyMath::Min(const T& a, const T& b){ return a<=b?a:b; }

template<typename T>
inline T gyMath::Clamp(const T& value, const T& minVal, const T& maxVal )
{
  if ( value <= minVal ) return minVal;
  if ( value >= maxVal ) return maxVal;
  return value;
}

template<typename T> 
inline void gyMath::Swap(T& a, T& b)
{
  T tmp=a;
  a=b;
  b=tmp;
}


inline bool gyMath::Equal(double a, double b)
{
  return Abs(a-b) <= Max( gyMath::GYDBL_ABSTOLERANCE, gyMath::GYDBL_RELTOLERANCE*Max(Abs(a), Abs(b)) );
}

template<typename T>
inline T gyMath::Saturate(const T& value){ return Clamp(value,T(0),T(1)); }

template<typename T>
inline T gyMath::Abs(const T& val)
{
  return (val<0) ? -val : val;
}

template<typename T>
inline bool gyMath::IsRealEqual (T a, T b, T epsilon/*=T(0)*/)
{
  FAIL_IF(epsilon < T(0), "epsilon must be larger than zero" );
  if (epsilon == T(0)) return (a == b);
  // make sure that NaN's will also return 'false'
  return ((a >= b - epsilon) && (a <= b + epsilon));
}

template<typename T>
inline bool gyMath::IsZero (T a, T epsilon/*=T(0.00001)*/)
{
  return IsRealEqual(a, T(0), epsilon);
}

void gyMath::VecMul( gyv3* out, const gyv3& v, gymathtype f )
{
  out->x = v.x * f;
  out->y = v.y * f;
  out->z = v.z * f;
}
void gyMath::VecAdd( gyv3* out, const gyv3& a, const gyv3& b )
{
  out->x = a.x + b.x;
  out->y = a.y + b.y;
  out->z = a.z + b.z;
}
void gyMath::VecSub( gyv3* out, const gyv3& a, const gyv3& b )
{
  out->x = a.x - b.x;
  out->y = a.y - b.y;
  out->z = a.z - b.z;
}
gymathtype gyMath::VecLength( const gyv3& v )
{
  return sqrt( VecDot(v,v) );
}
gymathtype gyMath::VecDot( const gyv3& a, const gyv3& b )
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}
void gyMath::VecCross( gyv3* out, const gyv3& p, const gyv3& q )
{
  out->x = p.y*q.z - p.z*q.y;
  out->y = p.z*q.x - p.x*q.z;
  out->z = p.x*q.y - p.y*q.x;
}
void gyMath::VecNormalize( gyv3* out, const gyv3& v )
{
  const gymathtype invlen = gymathtype(1) / VecLength( v );
  VecMul( out, v, invlen );
}
void gyMath::VecNormalize( gyv3* inout )
{
  const gymathtype invlen = gymathtype(1) / VecLength(*inout);
  inout->x*=invlen;
  inout->y*=invlen;
  inout->z*=invlen;
}

void gyMath::MatrixPerspectiveProjection( gymat44* out, const gyrad& fov, gymathtype aspect, gymathtype zNear, gymathtype zFar )
{
  const gymathtype fovby2 = fov.valueInRadians*0.5f;
  gymathtype yScale = cosf(fovby2) / sinf(fovby2);
  gymathtype xScale = yScale / aspect;
  out->_00 = xScale;  out->_01 = 0;    out->_02 = 0; out->_03 = 0;
  out->_10 = 0;    out->_11 = yScale;  out->_12 = 0; out->_13 = 0;
  out->_20 = 0;    out->_21 = 0;    out->_22 = zFar/(zFar-zNear); out->_23 = -zNear*zFar/(zFar-zNear);
  out->_30 = 0;    out->_31 = 0;    out->_32 = gymathtype(1); out->_33 = 0;

  MatrixToPlatform(out); // prepare platform specific layout
}

void gyMath::MatrixOrthographicProjection( gymat44* out, gymathtype left, gymathtype right, gymathtype top, gymathtype bottom, gymathtype zNear, gymathtype zFar)
{
  const gymathtype iw = gymathtype(1)/(right-left);
  const gymathtype ih = gymathtype(1)/(top-bottom);
  const gymathtype id = gymathtype(1)/(zFar-zNear);
  out->_00 = gymathtype(2)*iw; out->_01 = 0;    out->_02 = 0;    out->_03 = -(right+left)*iw;
  out->_10 = 0;    out->_11 = gymathtype(2)*ih; out->_12 = 0;    out->_13 = -(top+bottom)*ih;
  out->_20 = 0;    out->_21 = 0;    out->_22 = gymathtype(2)*id; out->_23 = (zFar+zNear)*id;
  out->_30 = 0;    out->_31 = 0;    out->_32 = gymathtype(1);    out->_33 = gymathtype(1);
  MatrixToPlatform(out);
}

void gyMath::MatrixLookAt( gymat44* out, const gyv3& eye, const gyv3& at, const gyv3& up/*=gyv3(0,1,0)*/ )
{
  gyv3 zaxis; VecSub(&zaxis,at,eye); VecNormalize(&zaxis);
  gyv3 xaxis; VecCross(&xaxis,up,zaxis); VecNormalize(&xaxis);
  gyv3 yaxis; VecCross(&yaxis,zaxis,xaxis);
  const gyv3 trans(-VecDot(xaxis,eye), -VecDot(yaxis,eye), -VecDot(zaxis,eye));

  out->_00 = xaxis.x;  out->_01 = xaxis.y;  out->_02 = xaxis.z; out->_03 = trans.x;
  out->_10 = yaxis.x;  out->_11 = yaxis.y;  out->_12 = yaxis.z; out->_13 = trans.y;
  out->_20 = zaxis.x;  out->_21 = zaxis.y;  out->_22 = zaxis.z; out->_23 = trans.z;
  out->_30 = 0;     out->_31 = 0;     out->_32 = 0;    out->_33 = gymathtype(1);

  MatrixToPlatform(out); // prepare platform specific layout
}
void gyMath::MatrixTranspose( gymat44* out )
{
  const gymat44 t = *out;
  out->_01 = t._10;  out->_02 = t._20; out->_03 = t._30;
  out->_10 = t._01;                     out->_12 = t._21; out->_13 = t._31;
  out->_20 = t._02;  out->_21 = t._12;                    out->_23 = t._32;
  out->_30 = t._03;  out->_31 = t._13;  out->_32 = t._23; 
}
void gyMath::MatrixTranspose( gymat33* out )
{
  const gymat33 m = *out;
  out->_01 = m._10; out->_02 = m._20; 
  out->_10 = m._01;                   out->_12 = m._21; 
  out->_20 = m._02; out->_21 = m._12; 
}
void gyMath::MatrixToPlatform( gymat44* out ) // prepare platform specific layout
{
#ifdef GY_RENDER_D3D11
  // do nothing - matrices in this engine share same layout as DX11
  out=out;
#endif
}
void gyMath::MatrixRotX( gymat33* out, const gyrad& ang )
{
  const gymathtype ca = cosf(ang.valueInRadians);
  const gymathtype sa = sinf(ang.valueInRadians);
  out->_00 = gymathtype(1); out->_01 = 0; out->_02 = 0;
  out->_10 = 0; out->_11 = ca  ; out->_12 = sa ;
  out->_20 = 0; out->_21 = -sa ; out->_22 = ca  ;
}
void gyMath::MatrixRotY( gymat33* out, const gyrad& ang )
{
  const gymathtype ca = cosf(ang.valueInRadians);
  const gymathtype sa = sinf(ang.valueInRadians);
  out->_00 = ca  ; out->_01 = 0; out->_02 = -sa ;
  out->_10 = 0; out->_11 = gymathtype(1); out->_12 = 0;
  out->_20 = sa  ; out->_21 = 0; out->_22 = ca  ;
}
void gyMath::MatrixRotZ( gymat33* out, const gyrad& ang )
{
  const gymathtype ca = cosf(ang.valueInRadians);
  const gymathtype sa = sinf(ang.valueInRadians);
  out->_00 = ca  ; out->_01 =  sa ; out->_02 = 0;
  out->_10 = -sa ; out->_11 = ca  ; out->_12 = 0;
  out->_20 = 0; out->_21 = 0; out->_22 = gymathtype(1);
}
void gyMath::MatrixRotX( gymat44* out, const gyrad& ang )
{
  // lot of unneeded gymatht movements? copy&paste 3x3 code here?
  gymat33 m; MatrixRotX(&m,ang);
  MatrixConvert( out, m );
}
void gyMath::MatrixRotY( gymat44* out, const gyrad& ang )
{
  // lot of unneeded gymatht movements? copy&paste 3x3 code here?
  gymat33 m; MatrixRotY(&m,ang);
  MatrixConvert( out, m );
}
void gyMath::MatrixRotZ( gymat44* out, const gyrad& ang )
{
  // lot of unneeded gymatht movements? copy&paste 3x3 code here?
  gymat33 m; MatrixRotZ(&m,ang);
  MatrixConvert( out, m );
}
void gyMath::MatrixRotXY( gymat44* out, const gyrad& angX, const gyrad& angY )
{
  gymat44 mX,mY;
  MatrixRotX(&mX,angX);
  MatrixRotY(&mY,angY);
  MatrixMult(out,mY,mX);
  //   const gymatht cx = cosf(angX.valueInRadians), sx =sinf(angX.valueInRadians);
  //   const gymatht cy = cosf(angY.valueInRadians), sy =sinf(angY.valueInRadians);
  //   out->_00 = cy    ; out->_01 = 0; out->_02 = sy   ; out->_03 = 0;
  //   out->_10 = -sx*sy; out->_11 = cx  ; out->_12 = sx*cy; out->_13 = 0;
  //   out->_20 = -cx*sy; out->_21 = -sx  ; out->_22 =  cx*cy; out->_23 = 0;
  //   out->_30 = 0;  out->_31 = 0;  out->_32 = 0; out->_33 = gymathtype(1);
}
void gyMath::MatrixTranslation( gymat44* out, const gyv3& t )
{
  out->_00 = gymathtype(1);  out->_01 = 0;  out->_02 = 0; out->_03 = t.x;
  out->_10 = 0;  out->_11 = gymathtype(1);  out->_12 = 0; out->_13 = t.y;
  out->_20 = 0;  out->_21 = 0;  out->_22 = gymathtype(1); out->_23 = t.z;
  out->_30 = 0;  out->_31 = 0;  out->_32 = 0; out->_33 = gymathtype(1);
}

void gyMath::MatrixTranslation( gymat44* out, const gyv2& t )
{
  MatrixTranslation(out, gyv3(t.x,t.y,0) );
}


void gyMath::MatrixMult( gymat44* out, const gymat44& a, const gymat44& b )
{
  for ( int i = 0; i < 4; ++i )
  {
    out->col[i][0] = a.col[0][0]*b.col[i][0] + a.col[1][0]*b.col[i][1] + a.col[2][0]*b.col[i][2] + a.col[3][0]*b.col[i][3];
    out->col[i][1] = a.col[0][1]*b.col[i][0] + a.col[1][1]*b.col[i][1] + a.col[2][1]*b.col[i][2] + a.col[3][1]*b.col[i][3];
    out->col[i][2] = a.col[0][2]*b.col[i][0] + a.col[1][2]*b.col[i][1] + a.col[2][2]*b.col[i][2] + a.col[3][2]*b.col[i][3];
    out->col[i][3] = a.col[0][3]*b.col[i][0] + a.col[1][3]*b.col[i][1] + a.col[2][3]*b.col[i][2] + a.col[3][3]*b.col[i][3];
  }
}
void gyMath::MatrixMult( gymat33* out, const gymat33& a, const gymat33& b )
{
  for ( int i = 0; i < 3; ++i )
  {
    out->col[i][0] = a.col[0][0]*b.col[i][0] + a.col[1][0]*b.col[i][1] + a.col[2][0]*b.col[i][2];
    out->col[i][1] = a.col[0][1]*b.col[i][0] + a.col[1][1]*b.col[i][1] + a.col[2][1]*b.col[i][2];
    out->col[i][2] = a.col[0][2]*b.col[i][0] + a.col[1][2]*b.col[i][1] + a.col[2][2]*b.col[i][2];
  }
}
void gyMath::MatrixConvert( gymat44* out, const gymat33& m )
{
  out->_00 = m._00; out->_01 = m._01; out->_02 = m._02; out->_03 = 0;
  out->_10 = m._10; out->_11 = m._11; out->_12 = m._12; out->_13 = 0;
  out->_20 = m._20; out->_21 = m._21; out->_22 = m._22; out->_23 = 0;
  out->_30 = 0;  out->_31 = 0;  out->_32 = 0;  out->_33 = gymathtype(1);
}
void gyMath::MatrixConvert( gymat33* out, const gymat44& m )
{
  out->_00 = m._00; out->_01 = m._01; out->_02 = m._02; 
  out->_10 = m._10; out->_11 = m._11; out->_12 = m._12; 
  out->_20 = m._20; out->_21 = m._21; out->_22 = m._22;     
}
void gyMath::VecMultAdd( gyv3* out, const gyv3& multv, gymathtype t, const gyv3& addv )
{
  out->x = multv.x*t + addv.x;
  out->y = multv.y*t + addv.y;
  out->z = multv.z*t + addv.z;
}
void gyMath::ExtractEulerAngles( const gymat44& m, gymathtype* x, gymathtype* y, gymathtype* z )
{
  // Extracting Euler Angles from a Rotation Matrix
  // Mike Day, Insommniac Games 
  *x = atan2(m._12,m._22);
  const gymathtype c2 = sqrt( m._00*m._00 + m._01*m._01 );
  *y = atan2(-m._02,c2);
  const gymathtype s1 = sin(*x), c1 = cos(*x);
  *z = atan2(s1*m._20 - c1*m._10, c1*m._11 - s1*m._21 );
}
template<typename T>
T gyMath::SmoothStep(T edge0, T edge1, T x)
{
  T t;
  t = Clamp<T>( (x-edge0)/(edge1-edge0), 0, 1 );
  return t * t * (3 - 2 * t );
}

