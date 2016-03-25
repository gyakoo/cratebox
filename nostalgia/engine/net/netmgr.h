#ifndef _GY_NETMANAGER_H_
#define _GY_NETMANAGER_H_

class gyEngine;

enum gySocketType
{
  GY_SOCKET_TCP,
  GY_SOCKET_UDP
};

//////////////////////////////////////////////////////////////////////////
struct gySocket : public gyResourceBase
{
  gySocket()
  {}
};

//////////////////////////////////////////////////////////////////////////
class gyNetManager : public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyNetManager);
public:
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  // -- Manager control
  void Destroy();
  
  // -- sockets low level API
  gyIDSocket SocketCreate( gySocketType stype );
  int SocketListen(gyIDSocket idSock, uint16_t port);
  int SocketSend(gyIDSocket idSock, const void* data, uint32_t size);
  int SocketRecv(gyIDSocket idSock, void* data, uint32_t maxSize);
  int SocketClose(gyIDSocket& idSock);

private:
  friend class gyEngine;
  gyNetManager();
  ~gyNetManager();

private:
  gyResourceMgr<gySocket, ID_SOCKET>  sockets;
};

#endif