#define _WINSOCKAPI_
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
/////////////////////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////////////////////
struct gySocketWinsock : public gySocket
{
  gySocketWinsock():handle(INVALID_SOCKET)
  {
  }
  ~gySocketWinsock(){ Close(); }
  void Close()
  {
    ::closesocket(handle);
    handle = INVALID_SOCKET;
  }

  SOCKET handle;
};

/////////////////////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////////////////////
class gyNetManager::Impl
{
public:
  Impl();
  ~Impl(){ Destroy(); }
  void Destroy();
  gySocket* SocketCreate( gySocketType stype );
  int SocketListen(gySocket* sock, uint16_t port);
  int SocketSend(gySocket* sock, const void* data, uint32_t size);
  int SocketRecv(gySocket* sock, void* data, uint32_t maxDataSize);
  int SocketClose(gySocket* sock);
private: 
  WSADATA wsadata;
};

gyNetManager::Impl::Impl()
{
  ZeroMemory(&wsadata, sizeof(WSADATA));
  if ( WSAStartup(MAKEWORD(2,2),&wsadata) == SOCKET_ERROR )
    ZeroMemory(&wsadata,sizeof(WSADATA));
}

void gyNetManager::Impl::Destroy()
{
  if ( wsadata.wVersion != 0 )
  {
    ZeroMemory(&wsadata, sizeof(WSADATA));
    WSACleanup();
  }
}

gySocket* gyNetManager::Impl::SocketCreate( gySocketType stype )
{

  bool inherit = !gyWinIs7SP1OrGreater();
  DWORD flags = WSA_FLAG_OVERLAPPED;
  if ( !inherit )
    flags |= 0x80; //WSA_FLAG_NO_HANDLE_INHERIT
  
  SOCKET handle=INVALID_SOCKET;
  switch ( stype )
  {
  case GY_SOCKET_TCP:
    handle = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, flags);
    break;
  case GY_SOCKET_UDP:
    handle = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, flags);
    break;
  }

  // If WSA_FLAG_NO_HANDLE_INHERIT is not supported, manually change the handle information. This is subject to a race condition if 
  // a new process is created before the handle has been changed, but it's the best we can do.
  if ( inherit && handle != INVALID_SOCKET)
    SetHandleInformation((HANDLE)handle, HANDLE_FLAG_INHERIT, 0);

  gySocketWinsock* sock = NULL;
  if ( handle != INVALID_SOCKET )
  {
    sock = new gySocketWinsock;
    sock->handle = handle;
  }
  return sock;
}

int gyNetManager::Impl::SocketListen(gySocket* sock, uint16_t port)
{
  gySocketWinsock* winsock = (gySocketWinsock*)sock;
  if ( winsock->handle == INVALID_SOCKET )
    return -1;

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if ( ::bind(winsock->handle, (sockaddr*)&address, sizeof(address)) != 0 )
    return -1;

  if ( ::listen(winsock->handle, SOMAXCONN) != 0 )
    return -1;
  return R_OK;
}

int gyNetManager::Impl::SocketSend(gySocket* sock, const void* data, uint32_t size)
{
  gySocketWinsock* winsock = (gySocketWinsock*)sock;
  if ( winsock->handle == INVALID_SOCKET || !data )
    return -1;
  
  if ( !size ) 
    return 0;
  
  // ensures it sends all bytes
  uint32_t sent=0;
  const char* sendPtr = reinterpret_cast<const char*>(data);
  while ( sent < size )
  {
    int retSent = ::send(winsock->handle, sendPtr, size-sent, 0);
    if ( retSent < 0 )
      return -1;
    sent += retSent;
    sendPtr += retSent;
  }
  return sent;
}

int gyNetManager::Impl::SocketRecv(gySocket* sock, void* data, uint32_t maxDataSize)
{
  gySocketWinsock* winsock = (gySocketWinsock*)sock;
  if ( winsock->handle == INVALID_SOCKET || !data )
    return -1;
  
  if ( !maxDataSize )
    return 0;

  char* rcvPtr = reinterpret_cast<char*>(data);
  int retRecv = ::recv(winsock->handle, rcvPtr, (int)maxDataSize, 0);
  return retRecv;
}


int gyNetManager::Impl::SocketClose(gySocket* sock)
{
  gySocketWinsock* winsock = (gySocketWinsock*)sock;
  if ( winsock->handle == INVALID_SOCKET )
    return -1;
  winsock->Close();  
  return R_OK;
}