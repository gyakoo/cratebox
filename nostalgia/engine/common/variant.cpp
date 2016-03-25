#include <stdafx.h>
#include <engine/common/variant.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#pragma region Misc
static gyVariant Concat( const char* a, const char* b )
{
  const unsigned int la = gyStringUtil::StrLen(a), lb = gyStringUtil::StrLen(b);
  char* tmp = (char*)malloc( la + lb + 1 );
  if ( tmp == NULL ) return gyVariant::INVALID();
  strcpy_s( tmp, la+1, a );
  strcpy_s( tmp+la, lb+1, b );
  tmp[la+lb] = 0;
  gyVariant ret(tmp); // one copy
  free(tmp);
  return ret; // second copy :( bad performance, bad fragmentation
}

static gyVariant Concat( const wchar_t* a, const wchar_t* b )
{
  const unsigned int la = gyStringUtil::StrLen(a), lb = gyStringUtil::StrLen(b);
  wchar_t* tmp = (wchar_t*)malloc( (la+lb+1)*sizeof(wchar_t) );
  if ( tmp == NULL ) return gyVariant::INVALID();
  wcscpy_s( tmp, la+1, a );
  wcscpy_s( tmp+la, lb+1, b);
  tmp[la+lb] = 0;
  gyVariant ret(tmp);
  free(tmp);
  return ret;
}

static void BuildVL( const std::vector<const char*>& stdList, gyVarList& outList)
{
  for ( int i = 0; i < (int)stdList.size(); ++i )
    outList.Add(stdList[i]);
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#pragma region gyVariant

gyVariant& gyVariant::INVALID()
{ 
  static gyVariant gInvalid;
  gInvalid.Invalidate();
  return gInvalid; 
}

gyVariant::gyVariant():typ(UNKNOWN), asInt64(0), subtyp(0)
{

}


gyVariant::~gyVariant()
{
  Clear();
}

gyVariant::operator int16_t() const   { return (int16_t)GetI(); }
gyVariant::operator int32_t() const        { return (int32_t)GetI(); }
gyVariant::operator int64_t() const        { return GetI(); }
gyVariant::operator float() const        { return GetF(); }
gyVariant::operator double() const        { return GetD(); }
gyVariant::operator const char*() const    { return GetS(); }
gyVariant::operator const wchar_t*() const{ return GetW(); }
gyVariant::operator const gyVarList&() const{ return GetVL(); }
gyVariant::operator const gyVariant::CFUNCPROTO() const{ return GetCF(); }
//gyVariant::operator const RefCounted*(){ return getRC(); }

const gyVariant& gyVariant::operator =( int16_t s)        { Set(s); return *this;}
const gyVariant& gyVariant::operator =( int32_t i )        { Set(i); return *this;}
const gyVariant& gyVariant::operator =( int64_t ll)        { Set(ll);return *this;}
const gyVariant& gyVariant::operator =( float f)        { Set(f); return *this;}
const gyVariant& gyVariant::operator =( double d)        { Set(d); return *this;}
const gyVariant& gyVariant::operator =( const char* s)    { Set(s); return *this;}
const gyVariant& gyVariant::operator =( const wchar_t* s){ Set(s); return *this;}
const gyVariant& gyVariant::operator =( CFUNCPROTO fp ){ Set(fp); return *this; }
const gyVariant& gyVariant::operator =( const gyVarDict& vt){ Set(vt); return *this; }
const gyVariant& gyVariant::operator =( const gyVariant& o){ Set(o); return *this;}
const gyVariant& gyVariant::operator =( const gyVarList& vl){ Set(vl); return *this; }
const gyVariant& gyVariant::operator =( gyVariant* o ){ Set(o); return *this; }
const gyVariant& gyVariant::operator =( const gyVariant* o ){ Set(o); return *this; }
const gyVariant& gyVariant::operator =( const gyRefCounted* rc){ Set(rc); return *this; }

gyVariant::gyVariant(const gyVariant& o):typ(UNKNOWN), asInt64(0)
{
  if        ( o.typ == T_STRING  ) this->Set( o.asCharPtr );
  else if ( o.typ == T_WSTRING )   this->Set( o.asWCharPtr );
  else if ( o.typ == T_VTABLE )    this->Set( o.GetVT() );
  else if ( o.typ == T_VLIST )     this->Set( o.GetVL() );
  else if ( o.typ == T_REFCOUNTED) this->Set( o.GetRC() );
  else                             this->Set( o );
  subtyp = o.subtyp;
}

gyVariant::gyVariant( int16_t s):typ(UNKNOWN), asInt64(0),subtyp(0)                   { Set(s); }
gyVariant::gyVariant( int32_t i ):typ(UNKNOWN), asInt64(0),subtyp(0)                  { Set(i); }
gyVariant::gyVariant( int64_t ll):typ(UNKNOWN), asInt64(0),subtyp(0)                  { Set(ll); }
gyVariant::gyVariant( float f):typ(UNKNOWN), asInt64(0),subtyp(0)                   { Set(f); }
gyVariant::gyVariant( double d):typ(UNKNOWN), asInt64(0),subtyp(0)                  { Set(d); }
gyVariant::gyVariant( const char* s):typ(UNKNOWN), asInt64(0),subtyp(0)             { Set(s); }
gyVariant::gyVariant( const wchar_t* s):typ(UNKNOWN), asInt64(0),subtyp(0)          { Set(s); }
gyVariant::gyVariant( CFUNCPROTO fp ):typ(UNKNOWN), asInt64(0),subtyp(0)            { Set(fp);}
gyVariant::gyVariant( const gyVarDict& vt ):typ(UNKNOWN), asInt64(0),subtyp(0) { Set(vt); }
gyVariant::gyVariant( const gyVarList& vl ):typ(UNKNOWN), asInt64(0),subtyp(0)  { Set(vl); }
gyVariant::gyVariant( gyVariant* v ):typ(UNKNOWN), asInt64(0),subtyp(0)             { Set(v); }
gyVariant::gyVariant( const gyVariant* v ):typ(UNKNOWN), asInt64(0),subtyp(0)       { Set(v); }
gyVariant::gyVariant( const gyRefCounted* rc):typ(UNKNOWN), asInt64(0),subtyp(0)    { Set(rc); }

gyVariant::eType gyVariant::GuessType()
{
  if ( typ == T_STRING )
  {
    const int32_t gt = gyStringUtil::GuessType( asCharPtr );
    // String(0), Char(1), Number(2), Float(3), Double(4), List(5)
    switch ( gt )
    {
    case 1: Set( *asCharPtr ); break;
    case 2: Set( gyStringUtil::ParseNumber(asCharPtr) ); break;
    case 3: Set( gyStringUtil::ParseFloat(asCharPtr) ); break;
    case 4: Set( gyStringUtil::ParseDouble(asCharPtr) ); break;
    case 5:
      {
        std::vector<const char*> tokens;
        if ( gyStringUtil::Split( asCharPtr, tokens ) > 0 )
        {
          gyVarList vl;
          BuildVL(tokens,vl);
          Set( vl );
        }
      }break;
    }
  }else if ( typ == T_WSTRING )
  {
    IMPLEMENT("No WString support");
  }
  return typ;
}

/// \brief Returns the type represented
gyVariant::eType gyVariant::Type() const { return typ; }
bool gyVariant::IsValid()const
{
  return typ != UNKNOWN;
}

void gyVariant::Invalidate()
{
  Clear();
  this->typ = UNKNOWN;
  this->asInt64 =0;
  this->subtyp = 0;
}

/// \brief Return size of type represented in bytes
int gyVariant::TypeSize( ) const
{
  switch ( typ )
  {
  case T_INT64    : return sizeof(int64_t);
  case T_FLOAT    : return sizeof(float);
  case T_DOUBLE    : return sizeof(double);
  case T_STRING    : return (1+gyStringUtil::StrLen(asCharPtr))*sizeof(char);
  case T_WSTRING    : return(1+gyStringUtil::StrLen(asWCharPtr))*sizeof(wchar_t);
  case T_CFUNC    : return sizeof(CFUNCPROTO);
  case T_VTABLE    :
    {
      struct CountBytesVT
      {
        uint32_t bytes;
        CountBytesVT():bytes(0){}
        void operator()( const gyVariant& a, const gyVariant& b )
        {
          bytes += a.TypeSize() + b.TypeSize();
        }
      }cbvt;
      const gyVarDict& vt = GetVT();
      vt.Traverse( cbvt );
      return cbvt.bytes;
    };
  case T_VLIST    : return GetVL().SizeInBytes();
  case T_VPTR        : return GetVPtr()->TypeSize();
  case T_REFCOUNTED: FAIL_IF(true,"Unknown size"); break;
  }
  return 0;
}

const char* gyVariant::ToString(std::string& outStr) const
{
  switch ( typ )
  {
  case T_INT64    : outStr = gyStrFormat64( "%ld",asInt64); break;
  case T_FLOAT    : outStr = gyStrFormat64( "%f", asFloat ); break;
  case T_DOUBLE   : outStr = gyStrFormat64( "%lf",asDouble ); break;
  case T_STRING   : outStr = asCharPtr; break;
  case T_WSTRING  : gyStringUtil::ToAString(asWCharPtr,outStr); break;
  case T_CFUNC    : outStr = gyStrFormat64( "cfunc(0x%xld)",asInt64); break;
  case T_VTABLE   : outStr = "<vtable>"; break;
  case T_VLIST    : outStr = "<vlist>"; break;
  case T_VPTR     : outStr = "<vptr>"; break;
  case T_REFCOUNTED:outStr = "<refcounted>"; break;
  default         : outStr = "<unknown>"; break;
  }
  return outStr.c_str();
}

int64_t gyVariant::GetI()    const { return asInt64; }
float gyVariant::GetF()        const { return asFloat; }
double gyVariant::GetD()        const { return asDouble; }
const char*    gyVariant::GetS()    const { return asCharPtr; }
const wchar_t* gyVariant::GetW()const { return asWCharPtr; }
gyVariant::CFUNCPROTO gyVariant::GetCF()const{ return reinterpret_cast<CFUNCPROTO>(asInt64); }
const gyVarDict& gyVariant::GetVT() const { return *reinterpret_cast<const gyVarDict*>(asObject);}
gyVarDict& gyVariant::GetVT() { return *reinterpret_cast<gyVarDict*>(asObject);}
const gyVarList& gyVariant::GetVL() const  { return *reinterpret_cast<const gyVarList*>(asObject); }
gyVarList& gyVariant::GetVL() { return *reinterpret_cast<gyVarList*>(asObject); }
gyVariant* gyVariant::GetVPtr(){ return reinterpret_cast<gyVariant*>(asObject); }
const gyVariant* gyVariant::GetVPtr()const{ return reinterpret_cast<const gyVariant*>(asObject); }
const gyRefCounted* gyVariant::GetRC() const{ return (const gyRefCounted*)asObject; }
gyRefCounted* gyVariant::GetRC(){ return (gyRefCounted*)asObject; }

gyVariant& gyVariant::To(eType typ)
{
  if ( Type() == typ )
    return *this;

  switch ( Type() ) // src
  {
    case T_INT64: 
      switch(typ) // dst
      {
      case T_FLOAT    : Set( (float)GetI() ); break;
      case T_DOUBLE   : Set( (double)GetI() ); break;
      case T_STRING   : Set( gyStrFormat64("%ld",GetI()) ); break;
      case T_CFUNC    : Set( (CFUNCPROTO)GetI()); break;
      case T_VPTR     : Set( (gyVariant*)GetI() ); break;
      case T_REFCOUNTED:Set( (gyRefCounted*)GetI() ); break;
      default         : IMPLEMENT("From INT64 to this case");
      }
    break;

    case T_FLOAT: 
      switch(typ) // dst
      {
      case T_INT64    : Set( (int64_t)GetF() ); break;
      case T_DOUBLE   : Set( (double)GetF() ); break;
      case T_STRING   : Set( gyStrFormat64("%f",GetF()) ); break;
      default         : IMPLEMENT("From FLOAT to this case");
      }    
    break;

    case T_DOUBLE: 
      switch(typ) // dst
      {
      case T_INT64    : Set( (int64_t)GetD() ); break;
      case T_FLOAT    : Set( (double)GetD() ); break;
      case T_STRING   : Set( gyStrFormat64("%lf",GetD()) ); break;
      default         : IMPLEMENT("From DOUBLE this case");
      }
    break;
    case T_STRING   : 
      switch(typ) // dst
      {
      case T_INT64    : Set( gyStringUtil::ParseNumber(GetS()) ); break;
      case T_FLOAT    : Set( gyStringUtil::ParseFloat(GetS()) ); break;
      case T_DOUBLE   : Set( gyStringUtil::ParseDouble(GetS()) ); break;
      default         : IMPLEMENT("From STRING this case");
      }
    break;  
    case T_CFUNC    : 
      switch(typ) // dst
      {
      case T_INT64    : Set( (int64_t)GetCF()); break;
      default         : IMPLEMENT("From CFUNC to this case");
      }
    break;  
    case T_VPTR     : 
      switch(typ) // dst
      {
      case T_INT64    : Set( (int64_t)GetVPtr()); break;
      default         : IMPLEMENT("From VPTR to this case");
      } 
    break;
    case T_REFCOUNTED:
      switch(typ) // dst
      {
      case T_INT64    : Set( (int64_t)GetRC()); break;
      default         : IMPLEMENT("From RC to this case");
      } 
    break;
    default: IMPLEMENT("This src case");
  }
  return *this;
}

void gyVariant::Set(int16_t s)            { Clear(); asInt64 = s; typ=T_INT64; }
void gyVariant::Set(int32_t i)            { Clear(); asInt64 = i; typ=T_INT64; }
void gyVariant::Set(int64_t ll)           { Clear(); asInt64 =ll; typ=T_INT64; }
void gyVariant::Set(float f)            { Clear(); asFloat = f; typ=T_FLOAT; }
void gyVariant::Set(double d)           { Clear(); asDouble= d; typ=T_DOUBLE; }
void gyVariant::Set( gyVariant* v )     { Clear(); asObject = (void*)v; typ=T_VPTR; }
void gyVariant::Set( const gyVariant* v){ Clear(); asObject = (void*)v; typ=T_VPTR; }
void gyVariant::Set(const gyVariant& o)
{
  switch ( o.typ )
  {
  case T_INT64    : Set( o.GetI() ); break;
  case T_FLOAT    : Set( o.GetF() ); break;
  case T_DOUBLE   : Set( o.GetD() ); break;
  case T_STRING   : Set( o.GetS() ); break;
  case T_WSTRING  : Set( o.GetW() ); break;
  case T_CFUNC    : Set( o.GetCF()); break;
  case T_VTABLE   : Set( o.GetVT() ); break;
  case T_VLIST    : Set( o.GetVL() ); break;
  case T_VPTR     : Set( o.GetVPtr() ); break;
  case T_REFCOUNTED:Set( o.GetRC() ); break;
  }
  subtyp = o.subtyp;
}

void gyVariant::Set(const char* s)
{
  Clear();
  const int l = gyStringUtil::StrLen(s);
  asCharPtr = new char[ l+1 ];
  strcpy_s( asCharPtr, l+1, s );
  asCharPtr[l] = '\0';
  typ= T_STRING;
}

void gyVariant::Set(const wchar_t* s )
{
  Clear();
  const int l = gyStringUtil::StrLen(s);
  asWCharPtr = new wchar_t[ l+1 ];
  wcscpy_s( asWCharPtr, l+1, s );
  asWCharPtr[l] = L'\0';
  typ=T_WSTRING;
}
void gyVariant::Set(CFUNCPROTO fp)
{
  Clear();
  asInt64 = (int64_t)fp;
  typ = T_CFUNC;
}
void gyVariant::Set( const gyVarDict& vt )
{
  Clear();
  asObject = new gyVarDict( vt );
  typ = T_VTABLE;
}
void gyVariant::Set( const gyVarList& vl )
{
  Clear();
  asObject = new gyVarList( vl );
  typ = T_VLIST;
}
void gyVariant::Set( const gyRefCounted* rc )
{
  Clear();
  asObject = (void*)const_cast<gyRefCounted*>( rc );
  reinterpret_cast<gyRefCounted*>(asObject)->AddRef();
  typ = T_REFCOUNTED;
}
bool gyVariant::operator !=(const gyVariant& o) const
{
  return !( (*this) == o );
}
bool gyVariant::operator ==( const gyVariant& o )const
{
  switch ( this->typ )
  {
  case T_INT64    :
    switch ( o.Type() )
    {
    case T_INT64: return asInt64 == o.asInt64;
    case T_FLOAT: return gyMath::Equal( (double)asInt64, (double)asFloat );
    case T_DOUBLE:return gyMath::Equal( (double)asInt64, o.asDouble );
    }break;
  case T_FLOAT    :
    switch ( o.Type() )
    {
    case T_INT64: return gyMath::Equal( (double)asFloat, (double)o.asInt64 );
    case T_FLOAT: return gyMath::Equal( (double)asFloat, (double)o.asFloat );
    case T_DOUBLE:return gyMath::Equal( (double)asFloat, o.asDouble );
    }break;
  case T_DOUBLE    :
    switch ( o.Type() )
    {
    case T_INT64: return gyMath::Equal( asDouble, (double)o.asInt64 );
    case T_FLOAT: return gyMath::Equal( asDouble, (double)o.asFloat );
    case T_DOUBLE:return gyMath::Equal( asDouble, o.asDouble );
    }break;
  case T_STRING    :
    switch ( o.Type() )
    {
    case T_STRING : return gyStringUtil::StrCmp(asCharPtr, o.asCharPtr) == 0;
    case T_WSTRING: return gyStringUtil::StrCmp(asCharPtr, o.asWCharPtr) == 0;
    }break;
  case T_WSTRING  :
    switch ( o.Type() )
    {
    case T_STRING : return gyStringUtil::StrCmp(o.asCharPtr, asWCharPtr) == 0;
    case T_WSTRING: return gyStringUtil::StrCmp(asWCharPtr, o.asWCharPtr) == 0;
    }break;
  case T_VTABLE   : return (o.Type() == T_VTABLE && GetVT() == o.GetVT() ); break;
  case T_VLIST    : return ( o.Type() == T_VLIST && GetVL() == o.GetVL() ); break;
  case T_CFUNC :
  case T_VPTR  :
  case T_REFCOUNTED: return asObject == o.asObject;
  }
  return false;
}
bool gyVariant::operator <( const gyVariant& o ) const
{
  switch ( this->typ )
  {
  case T_INT64    :
    switch ( o.Type() )
    {
    case T_INT64: return asInt64 < o.asInt64;
    case T_FLOAT: return (float)asInt64 < o.asFloat;
    case T_DOUBLE:return (double)asInt64< o.asDouble;
    }break;
  case T_FLOAT    :
    switch ( o.Type() )
    {
    case T_INT64: return asFloat < (float)o.asInt64;
    case T_FLOAT: return asFloat < o.asFloat;
    case T_DOUBLE:return (double)asFloat < o.asDouble;
    }break;
  case T_DOUBLE    :
    switch ( o.Type() )
    {
    case T_INT64: return asDouble < (double)o.asInt64;
    case T_FLOAT: return asDouble < (double)o.asFloat;
    case T_DOUBLE:return asDouble < o.asDouble;
    }break;
  case T_STRING    :
    switch ( o.Type() )
    {
    case T_STRING: return gyStringUtil::StrCmp( asCharPtr, o.asCharPtr ) < 0;
    case T_WSTRING: return gyStringUtil::StrCmp( asCharPtr, o.asWCharPtr ) < 0;
    }break;
  case T_WSTRING    :
    switch ( o.Type() )
    {
    case T_WSTRING: return gyStringUtil::StrCmp( asWCharPtr, o.asWCharPtr ) < 0;
    case T_STRING : return gyStringUtil::StrCmp(  o.asCharPtr, asWCharPtr ) > 0;
    }break;
  case T_VTABLE    : return ( o.Type() == T_VTABLE && GetVT() < o.GetVT() );
  case T_VLIST    : return ( o.Type() == T_VLIST && GetVL() < o.GetVL() );
  }
  return false;
}
bool gyVariant::operator >( const gyVariant& o ) const
{
  switch ( this->typ )
  {
  case T_INT64    :
    switch ( o.Type() )
    {
    case T_INT64: return asInt64 > o.asInt64;
    case T_FLOAT: return (float)asInt64 > o.asFloat;
    case T_DOUBLE:return (double)asInt64> o.asDouble;
    }break;
  case T_FLOAT    :
    switch ( o.Type() )
    {
    case T_INT64: return asFloat > (float)o.asInt64;
    case T_FLOAT: return asFloat > o.asFloat;
    case T_DOUBLE:return (double)asFloat > o.asDouble;
    }break;
  case T_DOUBLE    :
    switch ( o.Type() )
    {
    case T_INT64: return asDouble > (double)o.asInt64;
    case T_FLOAT: return asDouble > (double)o.asFloat;
    case T_DOUBLE:return asDouble > o.asDouble;
    }break;
  case T_STRING    :
    switch ( o.Type() )
    {
    case T_STRING: return gyStringUtil::StrCmp( asCharPtr, o.asCharPtr ) > 0;
    case T_WSTRING: return gyStringUtil::StrCmp( asCharPtr, o.asWCharPtr ) > 0;
    }break;
  case T_WSTRING    :
    switch ( o.Type() )
    {
    case T_WSTRING: return gyStringUtil::StrCmp( asWCharPtr, o.asWCharPtr ) > 0;
    case T_STRING : return gyStringUtil::StrCmp( o.asCharPtr, asWCharPtr ) < 0;
    }break;
  case T_VTABLE    : return ( o.Type() == T_VTABLE && GetVT() > o.GetVT() );
  case T_VLIST    : return ( o.Type() == T_VLIST && GetVL() > o.GetVL() );
  }
  return false;
}
bool gyVariant::operator <=( const gyVariant& o ) const
{
  switch ( this->typ )
  {
  case T_INT64    :
    switch ( o.Type() )
    {
    case T_INT64: return asInt64 <= o.asInt64;
    case T_FLOAT: return (float)asInt64 <= o.asFloat;
    case T_DOUBLE:return (double)asInt64<= o.asDouble;
    }break;
  case T_FLOAT    :
    switch ( o.Type() )
    {
    case T_INT64: return asFloat <= (float)o.asInt64;
    case T_FLOAT: return asFloat <= o.asFloat;
    case T_DOUBLE:return (double)asFloat <= o.asDouble;
    }break;
  case T_DOUBLE    :
    switch ( o.Type() )
    {
    case T_INT64: return asDouble <= (double)o.asInt64;
    case T_FLOAT: return asDouble <= (double)o.asFloat;
    case T_DOUBLE:return asDouble <= o.asDouble;
    }break;
  case T_STRING    :
    switch ( o.Type() )
    {
    case T_STRING: return gyStringUtil::StrCmp( asCharPtr, o.asCharPtr ) <= 0;
    case T_WSTRING: return gyStringUtil::StrCmp( asCharPtr, o.asWCharPtr ) <= 0;
    }break;
  case T_WSTRING    :
    switch ( o.Type() )
    {
    case T_WSTRING: return gyStringUtil::StrCmp( asWCharPtr, o.asWCharPtr ) <= 0;
    case T_STRING : return gyStringUtil::StrCmp( o.asCharPtr, asWCharPtr ) > 0;
    }break;
  case T_VTABLE    : return ( o.Type() == T_VTABLE && GetVT() <= o.GetVT() );
  case T_VLIST    : return ( o.Type() == T_VLIST && GetVL() <= o.GetVL() );
  }
  return false;
}
bool gyVariant::operator >=( const gyVariant& o ) const
{
  switch ( this->typ )
  {
  case T_INT64    :
    switch ( o.Type() )
    {
    case T_INT64: return asInt64 <= o.asInt64;
    case T_FLOAT: return (float)asInt64 <= o.asFloat;
    case T_DOUBLE:return (double)asInt64<= o.asDouble;
    }break;
  case T_FLOAT    :
    switch ( o.Type() )
    {
    case T_INT64: return asFloat <= (float)o.asInt64;
    case T_FLOAT: return asFloat <= o.asFloat;
    case T_DOUBLE:return (double)asFloat <= o.asDouble;
    }break;
  case T_DOUBLE    :
    switch ( o.Type() )
    {
    case T_INT64: return asDouble <= (double)o.asInt64;
    case T_FLOAT: return asDouble <= (double)o.asFloat;
    case T_DOUBLE:return asDouble <= o.asDouble;
    }break;
  case T_STRING    :
    switch ( o.Type() )
    {
    case T_STRING: return gyStringUtil::StrCmp( asCharPtr, o.asCharPtr ) <= 0;
    case T_WSTRING: return gyStringUtil::StrCmp( asCharPtr, o.asWCharPtr ) <= 0;
    }break;
  case T_WSTRING    :
    switch ( o.Type() )
    {
    case T_WSTRING: return gyStringUtil::StrCmp( asWCharPtr, o.asWCharPtr ) <= 0;
    case T_STRING : return gyStringUtil::StrCmp( o.asCharPtr, asWCharPtr) > 0;
    }break;
  case T_VTABLE    : return ( o.Type() == T_VTABLE && GetVT() >= o.GetVT() );
  case T_VLIST    : return ( o.Type() == T_VLIST && GetVL() >= o.GetVL() );
  }
  return false;
}
bool gyVariant::operator &&( const gyVariant& o ) const
{
  return !( IsZero() || o.IsZero() );
}
bool gyVariant::operator ||( const gyVariant& o ) const
{
  return !(IsZero() && IsZero());
}
void gyVariant::Clear()
{
  switch ( typ )
  {
  case T_STRING:
  case T_WSTRING:
    delete [] asCharPtr;
    break;
  case T_VTABLE:
    delete reinterpret_cast<gyVarDict*>(asObject);
    break;
  case T_VLIST:
    delete reinterpret_cast<gyVarList*>(asObject);
    break;
  case T_REFCOUNTED:
    reinterpret_cast<gyRefCounted*>(asObject)->Release();
    break;
  }
  asInt64 = 0;
  typ = UNKNOWN;
}

bool gyVariant::IsZero()const
{
  if ( !IsValid() ) return true;

  switch ( Type() )
  {
  case T_INT64  : return GetI() == 0;
  case T_FLOAT  : return gyMath::IsZero(GetF());
  case T_DOUBLE : return gyMath::IsZero(GetD());
  case T_STRING : return GetS() == NULL || *GetS()== 0;
  case T_WSTRING: return GetW() == NULL || *GetW()== 0;
  case T_VLIST  : return GetVL().Size() == 0;
  case T_VTABLE : return GetVT().Size() == 0;

  default: return asObject == NULL;
  }
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#pragma region gyVariantList
gyVarList::gyVarList()
{
}
gyVarList::gyVarList( const gyVarList& o )
{
  if ( o.Size() > 0 )
  {
    params.reserve( o.Size() );
    std::copy( o.params.begin(), o.params.end(), std::back_inserter(params) );
  }
}
gyVarList& gyVarList::Add( const gyVariant& v )
{
  params.push_back( v ); return *this;
}

gyVariant& gyVarList::Get(int ndx)
{
  return params.at(ndx);
}

const gyVariant&    gyVarList::Get(int ndx)const
{
  return params.at(ndx);
}
void gyVarList::Set(int ndx, const gyVariant& v )
{
  params[ndx] = v;
}

int    gyVarList::Size()const
{
  return (int)params.size();
}

void gyVarList::Reserve(int count)
{
  params.reserve(count);
}

void gyVarList::Resize(int count)
{
  params.resize(count);
}

void gyVarList::Clear()
{
  params.clear();
}

unsigned long gyVarList::SizeInBytes()const
{
  unsigned long s = 0;
  for ( int i = 0; i < Size(); ++i )
    s += params[i].TypeSize();
  return s;
}
bool gyVarList::operator==(const gyVarList& b ) const
{
  const gyVarList& a = *this;
  if ( a.Size() == b.Size() )
  {
    for ( int i = 0; i < a.Size(); ++i )
      if ( a.Get(i) != b.Get(i) )
        return false;
    return true;
  }
  return false;
}
bool gyVarList::operator <(const gyVarList& b ) const
{
  return Size() < b.Size();
}
bool gyVarList::operator >(const gyVarList& b ) const
{
  return Size() > b.Size();
}
bool gyVarList::operator <=(const gyVarList& b ) const
{
  return (*this) < b || (*this) == b;
}
bool gyVarList::operator >=(const gyVarList& b ) const
{
  return (*this) > b || (*this) == b;
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#pragma region gyVariantTable
gyVarDict::gyVarDict()
{
}

gyVarDict::gyVarDict( const gyVarDict& vt )
{
  TTable::const_iterator it = vt.mMap.begin();
  for ( ; it != vt.mMap.end(); ++it )
  {
    mMap[ it->first ] = it->second;
  }
}

gyVarDict::gyVarDict( const gyVarList& vl )
{
  int i = 0;
  for ( ; i < vl.Size(); ++i )
    mMap[ i ] = vl.Get(i);
}

gyVariant& gyVarDict::operator[](const gyVariant& key)
{
  return mMap[key];
}

const gyVariant&    gyVarDict::operator[](const gyVariant& key)const
{
  TTable::const_iterator it = mMap.find( key );
  return it != mMap.end() ? it->second : gyVariant::INVALID();
}

gyVariant& gyVarDict::Get( const gyVariant& key )
{
  TTable::iterator it = mMap.find( key );
  return it != mMap.end() ? it->second : gyVariant::INVALID();
}

const gyVariant& gyVarDict::Get( const gyVariant& key, const gyVariant& def )
{
  TTable::iterator it = mMap.find( key );
  return it != mMap.end() ? it->second : def;
}

gyVariant gyVarDict::GetXPath( const char* path )
{
  gyVarList sp;
  std::vector<const char*> tokens;
  std::string tmp(path);  
  if ( gyStringUtil::Split(const_cast<char*>(tmp.c_str()),tokens,'.') <= 0 )
    return gyVariant::INVALID();
  BuildVL(tokens,sp);
  
  const gyVarDict* vt = this;
  gyVariant lastv = gyVariant::INVALID();
  for ( int i = 0; i < sp.Size(); ++i )
  {
    lastv = (*vt)[ sp.Get(i) ];
    if ( !lastv.IsValid() ) return gyVariant::INVALID();
    if ( lastv.Type() == gyVariant::T_VTABLE )
      vt = &lastv.GetVT();
  }
  return lastv;
}

gyVariant gyVarDict::GetXPath( const char* path, const gyVariant& def )
{
  gyVariant v = GetXPath(path);
  if ( !v.IsValid() )
    return def;
  return v.To(def.Type());
}

void gyVarDict::Remove( const gyVariant& key )
{
  TTable::iterator it = mMap.find( key );
  if ( it != mMap.end() )
    mMap.erase( it );
}

const gyVariant& gyVarDict::Get( const gyVariant& key)const
{
  TTable::const_iterator it = mMap.find( key );
  return it != mMap.end() ? it->second : gyVariant::INVALID();
}

const gyVariant& gyVarDict::Get( const gyVariant& key, const gyVariant& def) const
{
  TTable::const_iterator it = mMap.find( key );
  return it != mMap.end() ? it->second : def;
}

gyVariant gyVarDict::GetXPath( const char* path ) const
{
  gyVarList sp;
  std::vector<const char*> tokens;
  std::string tmp(path);  
  if ( gyStringUtil::Split(const_cast<char*>(tmp.c_str()),tokens, '.') <= 0 )
    return gyVariant::INVALID();
  BuildVL(tokens,sp);

  const gyVarDict* vt = this;
  gyVariant& lastv = gyVariant::INVALID();
  for ( int i = 0; i < sp.Size(); ++i )
  {
    lastv = (*vt)[ sp.Get(i) ];
    if ( !lastv.IsValid() ) return gyVariant::INVALID();
    if ( lastv.Type() == gyVariant::T_VTABLE )
      vt = &lastv.GetVT();
  }
  return lastv;
}

gyVariant gyVarDict::GetXPath( const char* path, const gyVariant& def ) const
{
  gyVariant v = GetXPath(path);
  if ( !v.IsValid() )
    return def;
  return v.To(def.Type());
}

bool gyVarDict::Has( const gyVariant& k ) const
{
  TTable::const_iterator it = mMap.find( k );
  return it != mMap.end();
}

bool gyVarDict::Has( const gyVariant& k, gyVariant::eType typ) const
{
  TTable::const_iterator it = mMap.find( k );
  if ( it == mMap.end() ) return false;
  return it->second.Type() == typ;
}


bool gyVarDict::IsType(const gyVariant& k, gyVariant::eType typ) const
{
  TTable::const_iterator it = mMap.find(k);
  if ( it == mMap.end() )
    return false;
  bool res = (it->second.Type() == typ);
  // wstring overload
  if ( !res && typ == gyVariant::T_STRING ) res = (it->second.Type() == gyVariant::T_WSTRING);
  return res;
}

void gyVarDict::Set( const gyVariant& key, const gyVariant& value )
{
  mMap[key] = value;
}

uint32_t gyVarDict::Size() const
{
  return (uint32_t)mMap.size();
}

void gyVarDict::Clear()
{
  mMap.clear();
}

bool gyVarDict::operator ==( const gyVarDict& b ) const
{
  if ( Size() != b.Size() ) return false;
  TTable::const_iterator it = mMap.begin();
  for ( ; it != mMap.end(); ++it )
  {
    const gyVariant& o = b.Get( it->first );
    if ( ! o.IsValid() ) return false;
    if ( o != it->second ) return false;
  }
  return true;
}
bool gyVarDict::operator <(const gyVarDict& b ) const
{
  return Size() < b.Size();
}
bool gyVarDict::operator >(const gyVarDict& b ) const
{
  return Size() > b.Size();
}
bool gyVarDict::operator <=(const gyVarDict& b ) const
{
  return (*this) < b || (*this) == b;
}
bool gyVarDict::operator >=(const gyVarDict& b ) const
{
  return (*this) > b || (*this) == b;
}
#pragma endregion