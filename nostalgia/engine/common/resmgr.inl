
template<typename RES, int RESTYPE> 
gyResourceMgr<RES,RESTYPE>::~gyResourceMgr()
{
  ReleaseAll();
}

template<typename RES, int RESTYPE> 
void gyResourceMgr<RES,RESTYPE>::ReleaseAll(bool deallocate)
{
  ResourceList::iterator it = resources.begin();
  for ( ; it != resources.end(); ++it )
    it->Destroy();
  
  if ( deallocate )
    resources.resize(0);
}

template<typename RES, int RESTYPE> 
void gyResourceMgr<RES,RESTYPE>::ResourceInfo::Destroy()
{
  resource = NULL;
  hash = 0;
}

template<typename RES, int RESTYPE> 
bool gyResourceMgr<RES,RESTYPE>::ResourceInfo::IsAvailable() const
{
  if ( resource.GetPtr() )
    return false;
  return true; // todo: to return true only when usedCount >= threshold
}

template<typename RES, int RESTYPE> 
typename gyResourceMgr<RES,RESTYPE>::IDResource gyResourceMgr<RES,RESTYPE>::FindByHash(uint32_t hash)
{
  for ( size_t i = 0; i < resources.size(); ++i )
    if ( resources[i].hash == hash )
      return IDResource((uint32_t)i);
  return IDResource::INVALID();
}


template<typename RES, int RESTYPE> 
typename gyResourceMgr<RES,RESTYPE>::IDResource gyResourceMgr<RES,RESTYPE>::AddUnique(RES* res, bool* exists/*=0*/)
{
  if ( !res ) 
    return IDResource::INVALID();
  // search for other with same hash
  const uint32_t newHash = res->ComputeHash();
  const size_t count = resources.size();
  size_t firstAvailable=count; // keep track of first available slot
  for ( size_t i=0; i < count; ++i )
  {
    const ResourceInfo& rinfo = resources[i];
    if ( rinfo.IsAvailable() && firstAvailable == count )
      firstAvailable = i;
    if (rinfo.resource && rinfo.hash == newHash )
    {
      if ( exists ) *exists = true;
      return IDResource((uint32_t)i);
    }
  }

  // we're here, no other was found with same hash  
  if ( firstAvailable == count )
  {
    // push back
    ResourceInfo rinfo;
    rinfo.resource = res;
    rinfo.hash = newHash;
    resources.push_back(rinfo);
  }
  else
  {
    // insert at avail pos
    ResourceInfo& rinfo = resources[firstAvailable];
    rinfo.resource = res;
    rinfo.hash = newHash;
  }
  if ( exists ) *exists = false;
  return IDResource((uint32_t)firstAvailable);
}

template<typename RES, int RESTYPE> template<typename _pred>
typename gyResourceMgr<RES,RESTYPE>::IDResource gyResourceMgr<RES,RESTYPE>::AddUnique(RES* res, _pred pred, bool* exists/*=0*/)
{
  if ( !res ) 
    return IDResource::INVALID();
  // search for other with same hash  
  const size_t count = resources.size();
  size_t firstAvailable=count; // keep track of first available slot
  for ( size_t i=0; i < count; ++i )
  {
    const ResourceInfo& rinfo = resources[i];    
    if ( rinfo.IsAvailable() && firstAvailable == count )
      firstAvailable = i;
    if ( pred(rinfo) )
    {
      if ( exists ) *exists = true;
      return IDResource((uint32_t)i);
    }
  }

  const uint32_t newHash = res->ComputeHash();
  // we're here, no other was found with same hash  
  if ( firstAvailable == count )
  {
    // push back
    ResourceInfo rinfo;
    rinfo.resource = res;
    rinfo.hash = newHash;
    resources.push_back(rinfo);
  }
  else
  {
    // insert at avail pos
    ResourceInfo& rinfo = resources[firstAvailable];
    rinfo.resource = res;
    rinfo.hash = newHash;
  }
  if ( exists ) *exists = false;
  return IDResource((uint32_t)firstAvailable);
}


template<typename RES, int RESTYPE> 
typename gyResourceMgr<RES,RESTYPE>::IDResource gyResourceMgr<RES,RESTYPE>::Add(RES* res)
{
  if ( !res ) 
    return IDResource::INVALID();
  const size_t count = resources.size();
  size_t firstAvailable=count; // keep track of first available slot
  for ( size_t i=0; i < count; ++i )
  {
    const ResourceInfo& rinfo = resources[i];
    if ( rinfo.IsAvailable() && firstAvailable == count )
    {
      firstAvailable = i;
      break;
    }
  }

  const uint32_t newHash = res->ComputeHash();
  if ( firstAvailable == count )
  {
    ResourceInfo rinfo;
    rinfo.resource = res;
    rinfo.hash = newHash;
    resources.push_back(rinfo);
  }
  else
  {
    ResourceInfo& rinfo = resources[firstAvailable];
    rinfo.resource = res;
    rinfo.hash = newHash;
  }
  return IDResource((uint32_t)firstAvailable);
}

template<typename RES, int RESTYPE> 
typename gyResourceMgr<RES,RESTYPE>::ResourceType* gyResourceMgr<RES,RESTYPE>::Get(IDResource idres)
{
  if ( !idres.IsValid() || idres.Number() >= resources.size() )
    return NULL;
  return resources[idres.Number()].resource;
}

template<typename RES, int RESTYPE> 
bool gyResourceMgr<RES,RESTYPE>::Release(IDResource& idres)
{
  if ( !idres.IsValid() || idres.Number() >= resources.size() )
  {
    idres.Invalidate();
    return false;
  }

  resources[idres.Number()].Destroy();
  idres.Invalidate();
  return true;
}

template<typename RES, int RESTYPE> 
int gyResourceMgr<RES,RESTYPE>::GetValidElementCount() const
{
  register int c = 0;
  for (auto& r : resources )
    if ( !r.IsAvailable() )
      ++c;
  return c;
}
