#include <stdafx.h>
#include <engine/net/netmgr.h>

#include GY_PLATINC_NETMGR

void gyNetManager::FillCreateTemplate(gyVarDict& outTemplate)
{
  
}

gyNetManager::gyNetManager()
{
  impl = new Impl();
}

gyNetManager::~gyNetManager()
{
  Destroy();
  SafeDelete(impl);
}

void gyNetManager::Destroy()
{
  sockets.ReleaseAll();
  impl->Destroy();
}

gyIDSocket gyNetManager::SocketCreate( gySocketType stype )
{
  gySharedPtr<gySocket> sock = impl->SocketCreate(stype);
  if ( !sock )
    return gyIDSocket::INVALID();
  return sockets.Add(sock);
}

int gyNetManager::SocketListen(gyIDSocket idSock, uint16_t port)
{
  gySocket* sock = sockets.Get(idSock);
  if ( !sock )
    return -1;
  return impl->SocketListen(sock,port);
}

int gyNetManager::SocketSend(gyIDSocket idSock, const void* data, uint32_t size)
{
  gySocket* sock = sockets.Get(idSock);
  if ( !sock )
    return -1;
  return impl->SocketSend(sock, data, size);
}

int gyNetManager::SocketRecv(gyIDSocket idSock, void* data, uint32_t maxSize)
{
  gySocket* sock = sockets.Get(idSock);
  if ( !sock )
    return -1;
  return impl->SocketRecv(sock,data,maxSize);
}

int gyNetManager::SocketClose(gyIDSocket& idSock)
{
  return sockets.Release(idSock) ? R_OK : -1;
}