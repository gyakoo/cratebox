#include <stdafx.h>
#include <engine/common/callback.h>

gyCallback::~gyCallback()
{
  Release();
}

void gyCallback::Release()
{
//   SuscriptorList::iterator it = suscriptors.begin();
//   for ( ; it != suscriptors.end(); ++it )
//     (*it) = NULL;

  suscriptors.clear();
}

void gyCallback::Subscribe( gyCallbackHandler* handler)
{
  SuscriptorList::iterator it = std::find( suscriptors.begin(), suscriptors.end(), handler);
  if ( it == suscriptors.end() )
  {
    suscriptors.push_back( handler );
  }
}

void gyCallback::Unsubscribe( gyCallbackHandler* handler )
{
  SuscriptorList::iterator it = std::find( suscriptors.begin(), suscriptors.end(), handler);
  if ( it != suscriptors.end() )
    suscriptors.erase(it);
}

void gyCallback::Trigger( gyCallbackData* data )
{
  SuscriptorList::iterator it = suscriptors.begin();
  for ( ; it != suscriptors.end(); ++it )
  {
    if ( (*it) )
      (*it)->OnCallback(data);
  }
}