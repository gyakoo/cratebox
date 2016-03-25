#ifndef _GYVARIANT_H_
#define _GYVARIANT_H_

struct gyVarList;//forward decl
class gyVarDict;
class gyRefCounted;

// ------------------------------------------------------------------------------------
struct gyVariant
{
public:
  enum eType{ UNKNOWN, T_INT64, T_FLOAT, T_DOUBLE, T_STRING, T_WSTRING,T_CFUNC, T_VTABLE, T_VLIST, T_VPTR, T_REFCOUNTED };
  
  typedef gyVariant (*CFUNCPROTO)( gyVarList& vlist );
  gyVariant();
  ~gyVariant();

  void SetSubType(int subtype){ subtyp = subtype; }
  int subtype()const { return subtyp; }

  operator int16_t() const;
  operator int32_t() const;
  operator int64_t() const;
  operator float() const;
  operator double() const;
  operator const char*() const;
  operator const wchar_t*() const;
  operator const CFUNCPROTO() const;
  operator const gyVarList& () const;
  operator const gyRefCounted*() const;

  const gyVariant& operator =( int16_t s);
  const gyVariant& operator =( int32_t i );
  const gyVariant& operator =( int64_t ll);
  const gyVariant& operator =( float f);
  const gyVariant& operator =( double d);
  const gyVariant& operator =( const char* s);
  const gyVariant& operator =( const wchar_t* s);
  const gyVariant& operator =( CFUNCPROTO fp );
  const gyVariant& operator =( const gyVarDict& vt);
  const gyVariant& operator =( const gyVarList& vl );
  const gyVariant& operator =( const gyVariant& o);
  const gyVariant& operator =( gyVariant* o);
  const gyVariant& operator =( const gyVariant* o);
  const gyVariant& operator =( const gyRefCounted* rc );

  gyVariant(const gyVariant& o);

  gyVariant( int16_t s);
  gyVariant( int32_t i );
  gyVariant( int64_t ll);
  gyVariant( float f);
  gyVariant( double d);
  gyVariant( const char* s);
  gyVariant( const wchar_t* s);
  gyVariant( CFUNCPROTO fp );
  gyVariant( const gyVarDict& vt );
  gyVariant( const gyVarList& vl );
  gyVariant( gyVariant* v );
  gyVariant( const gyVariant* v );
  gyVariant( const gyRefCounted* rc );

  eType GuessType();
  eType Type() const;
  bool IsValid() const;
  void Invalidate();
  int TypeSize( ) const;

  const char* ToString(std::string& outStr) const;

  int64_t GetI() const;
  float GetF() const;
  double GetD() const;
  const char* GetS() const;
  const wchar_t* GetW() const;
  CFUNCPROTO GetCF() const;
  const gyVarDict& GetVT() const;
  gyVarDict& GetVT();
  const gyVarList& GetVL() const;
  gyVarList& GetVL();
  const gyVariant* GetVPtr() const;
  gyVariant* GetVPtr();
  const gyRefCounted* GetRC() const;
  gyRefCounted* GetRC();

  gyVariant& To(eType typ);

  void Set(int16_t s);
  void Set(int32_t i);
  void Set(int64_t ll);
  void Set(float f);
  void Set(double d);
  void Set(const gyVariant& o);
  void Set(const char* s);
  void Set(const wchar_t* s );
  void Set(CFUNCPROTO fp);
  void Set( const gyVarDict& vt );
  void Set( const gyVarList& vl );
  void Set( gyVariant* v );
  void Set( const gyVariant* v );
  void Set( const gyRefCounted* rc );

  bool operator ==( const gyVariant& o )const;
  bool operator !=( const gyVariant& o )const;
  bool operator <(  const gyVariant& o ) const;
  bool operator >(  const gyVariant& o ) const;
  bool operator <=( const gyVariant& o ) const;
  bool operator >=( const gyVariant& o ) const;
  bool operator &&( const gyVariant& o ) const;
  bool operator ||( const gyVariant& o ) const;
  bool IsZero()const;

  static gyVariant& INVALID();

protected:
  void Clear();

  union
  {
    int64_t            asInt64;
    float            asFloat;
    double           asDouble;
    char*            asCharPtr;
    wchar_t*         asWCharPtr;
    void*            asObject;
  };
  eType typ;
  int   subtyp;
};


// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
struct gyVarList
{
  gyVarList();
  gyVarList( const gyVarList& o );
  gyVarList& Add( const gyVariant& v );
  gyVariant&  Get(int ndx);
  const gyVariant& Get(int ndx)const;
  void Set(int ndx, const gyVariant& v);
  int Size()const;
  void Reserve(int count);
  void Resize (int count);
  void Clear();
  unsigned long SizeInBytes()const;

  bool operator ==( const gyVarList& b ) const;
  bool operator <(const gyVarList& b ) const;
  bool operator >(const gyVarList& b ) const;
  bool operator <=(const gyVarList& b ) const;
  bool operator >=(const gyVarList& b ) const;

protected:
  std::vector< gyVariant >    params;
};

template<class T0>
gyVarList GYVARLIST( T0 _0 ){ gyVarList p; p.Add(_0); return p; }
template<class T0,class T1>
gyVarList GYVARLIST( T0 _0, T1 _1 ){ return GYVARLIST(_0).Add(_1); }
template<class T0,class T1,class T2>
gyVarList GYVARLIST( T0 _0, T1 _1, T2 _2 ){ return GYVARLIST(_0,_1).Add(_2); }
template<class T0,class T1,class T2,class T3>
gyVarList GYVARLIST( T0 _0, T1 _1, T2 _2, T3 _3 ){ return GYVARLIST(_0,_1,_2).Add(_3); }
template<class T0,class T1,class T2,class T3,class T4>
gyVarList GYVARLIST( T0 _0, T1 _1, T2 _2, T3 _3, T4 _4 ){ return GYVARLIST(_0,_1,_2,_3).Add(_4); }
template<class T0,class T1,class T2,class T3,class T4,class T5>
gyVarList GYVARLIST( T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5 ){ return GYVARLIST(_0,_1,_2,_3,_4).Add(_5); }
template<class T0,class T1,class T2,class T3,class T4,class T5,class T6>
gyVarList GYVARLIST( T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6 ){ return GYVARLIST(_0,_1,_2,_3,_4,_5).Add(_6); }
template<class T0,class T1,class T2,class T3,class T4,class T5,class T6, class T7>
gyVarList GYVARLIST( T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7 ){ return GYVARLIST(_0,_1,_2,_3,_4,_5,_6).Add(_7); }


// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
class gyVarDict
{
public:
  gyVarDict();
  gyVarDict( const gyVarDict& vt );
  gyVarDict( const gyVarList& vl );
  gyVariant& operator[](const gyVariant& key);
  const gyVariant& operator[](const gyVariant& key)const;
  gyVariant& Get(const gyVariant& key);
  const gyVariant& Get( const gyVariant& key, const gyVariant& def);
  // CAUTION: experimental, use discrete
  gyVariant GetXPath(const char* path);
  // CAUTION: experimental, use discrete
  gyVariant GetXPath(const char* path, const gyVariant& def);
  void Remove( const gyVariant& k );
  const gyVariant& Get( const gyVariant& key)const;
  const gyVariant& Get( const gyVariant& key, const gyVariant& def)const;
  gyVariant GetXPath( const char* path )const;
  gyVariant GetXPath( const char* path, const gyVariant& def )const;
  bool Has( const gyVariant& k ) const;
  bool Has( const gyVariant& k, gyVariant::eType typ) const;
  bool IsType(const gyVariant& k, gyVariant::eType typ) const;
  void Set( const gyVariant& key, const gyVariant& value );
  uint32_t Size() const;
  void Clear();

  template< typename OP2 >
  void Traverse( OP2& op )
  {
    TTable::iterator it = mMap.begin();
    for ( ; it != mMap.end(); ++it )
      op( it->first, it->second );
  }

  template< typename OP2 >
  void Traverse( OP2& op ) const
  {
    TTable::const_iterator it = mMap.begin();
    for ( ; it != mMap.end(); ++it )
      op( it->first, it->second );
  }

  bool operator ==( const gyVarDict& b ) const;
  bool operator <(const gyVarDict& b ) const;
  bool operator >(const gyVarDict& b ) const;
  bool operator <=(const gyVarDict& b ) const;
  bool operator >=(const gyVarDict& b ) const;

protected:
  typedef std::map< gyVariant, gyVariant >    TTable;

  TTable mMap; ///< Contains the dictionary
}; // class


#endif