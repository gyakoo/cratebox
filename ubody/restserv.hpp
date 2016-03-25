#pragma once
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

// Websocket specs: https://tools.ietf.org/html/rfc6455
// Websocket 101: http://lucumr.pocoo.org/2012/9/24/websockets-101/
/*
The handshake from the client looks as follows:

        GET /chat HTTP/1.1
        Host: server.example.com
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
        Origin: http://example.com
        Sec-WebSocket-Protocol: chat, superchat
        Sec-WebSocket-Version: 13

   The handshake from the server looks as follows:

        HTTP/1.1 101 Switching Protocols
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
        Sec-WebSocket-Protocol: chat
*/
#pragma warning(disable:4996)
template<int MAXC>
class RestServerJSON;
class RestServerClientConnectionJSON;
class RestServerCallbacksJSON;
#define SHA1HashSize 20
/*
*  This structure will hold context information for the SHA-1
*  hashing operation
*/
struct SHA1Context
{
  uint32_t Intermediate_Hash[SHA1HashSize / 4];		/* Message Digest  */
  uint32_t Length_Low;							/* Message length in bits      */
  uint32_t Length_High;							/* Message length in bits      */
  int_least16_t Message_Block_Index;				/* Index into message block array   */
  uint8_t Message_Block[64];						/* 512-bit message blocks      */
  int Computed;									/* Is the digest computed?         */
  int Corrupted;									/* Is the message digest corrupted? */
};

/*
*  Function Prototypes
*/

int SHA1Reset(SHA1Context*);
int SHA1Input(SHA1Context*, const uint8_t*, unsigned int);
int SHA1Result(SHA1Context*, uint8_t Message_Digest[SHA1HashSize]);
static void base64encode(const char* buf, int len, std::string& strOut);

class RestServerCallbacksJSON
{
public:
	virtual void onClientDataReceived(RestServerClientConnectionJSON& ) {  }
};

struct RestServerHeaders
{
	void clear()
	{
		m_headers.clear();
	}

	void serialize(std::string& data)
	{
		for (auto kv : m_headers)
		{
			data.append(kv.first);
			data.append(": ");
			data.append(kv.second);
			data.append("\r\n");
		}
	}

	void set(const std::string& key, const std::string& value)
	{
		m_headers[key] = value;
	}

	void setIfNew(const std::string& key, const std::string& value)
	{
		if (!hasKey(key))
			set(key, value);
	}

	void setIfExists(const std::string& key, const std::string& value)
	{
		if (hasKey(key))
			set(key, value);
	}

	bool hasKey(const std::string& key) const
	{
		return m_headers.find(key) != m_headers.end();
	}

	bool hasKeyValueCaseIns(const std::string& key, const std::string& value) const
	{
		auto it = m_headers.find(key);
		if (it == m_headers.end()) return false;
		auto valLower = it->second;
		std::transform(valLower.begin(), valLower.end(), valLower.begin(), ::tolower);
		auto rhsValLower = value;
		std::transform(rhsValLower.begin(), rhsValLower.end(), rhsValLower.begin(), ::tolower);
		return valLower == rhsValLower;
	}

	std::string get(const std::string& key) const
	{
		auto it = m_headers.find(key);
		return it != m_headers.end() ? it->second : "";
	}

	std::unordered_map<std::string, std::string> m_headers;
};

class RestServerClientConnectionJSON
{
public:
	typedef std::pair<std::string,std::string> UriParam;

	enum { BUFFLEN = 4<<10 };
	enum eMethod
	{
		METHOD_UNKNOWN= 0,
		METHOD_GET,
		METHOD_POST,
		METHOD_DELETE,
		METHOD_PUT,
		METHOD_TRACE,
		METHOD_OPTIONS,
		METHOD_HEAD
	};

	class Response
	{
	public:
		void clear()
		{
			m_body.clear();
			m_headers.clear();
			setStatusCode(200, "OK");
		}

		void setStatusCode(int statusCode, const std::string& statusText)
		{
			m_statusCode = std::to_string(statusCode);
			m_statusText = statusText;
		}

		void addToBody(const std::string& text)
		{
			m_body.append(text);
		}

		RestServerHeaders& getHeaders()
		{
			return m_headers;
		}

	private:
		friend class RestServerClientConnectionJSON;
		std::string compose(const RestServerClientConnectionJSON& client)
		{
			if ( client.isWebsocket())
			{
				setStatusCode(101, "Switching Protocols");
				m_headers.set("Upgrade", "websocket");
				m_headers.set("Connection", "Upgrade");
				m_headers.set("Sec-WebSocket-Accept", client.calculateWebsocketHandshakeKey());
			}
			else
			{
				std::time_t result = std::time(nullptr);
				char datestr[128];
				strftime(datestr, 128, "%A %c", std::localtime(&result));			
				m_headers.set("Date", datestr);
				m_headers.set("Last-Modified", datestr);
				m_headers.setIfNew("Server", "ubody");
				m_headers.setIfNew("Connection", "Keep-Alive");
				m_headers.set("Content-Length", std::to_string(m_body.length()) );
			}

			// status code
			std::string data; data.reserve(512);
			data.append("HTTP/1.1 ").append(m_statusCode).append(" ").append(m_statusText).append("\r\n");

			// headers
			m_headers.serialize(data);

			// content
			data.append("\r\n");
			data.append(m_body);
			return data;
		}

  private:
		RestServerHeaders m_headers;
		std::string m_body;
		std::string m_statusCode;
		std::string m_statusText;
	};

	RestServerClientConnectionJSON(int clientId=-1, SOCKET s=INVALID_SOCKET) 
		: m_clientId(clientId), m_socket(s), m_buffLen(0)
	{
	}

	bool isValid() const
	{
		return m_socket != INVALID_SOCKET;
	}

	void quit()
	{
		if ( isValid() )
		{
			closesocket(m_socket);
			m_socket=INVALID_SOCKET;
		}
		m_buffLen=0;
	}

	bool stepRecv(RestServerCallbacksJSON* callbacks)
	{
		bool recvOk = true;
		int recvBytes = ::recv(m_socket, m_buff, BUFFLEN, 0);
		if ( recvBytes == 0 )
		{
			recvOk = false;
		}else if ( recvBytes == SOCKET_ERROR )
		{
			// it's a non-blocking so we can time out but client is ok yet
			int lastError = WSAGetLastError();
			if ( lastError != WSAEWOULDBLOCK )
				recvOk = false;
		}
		else
		{
			// received data (recvBytes > 0)
			m_buffLen = recvBytes;
			if ( recvBytes >= BUFFLEN ) --recvBytes;
			m_buff[recvBytes]=0;
			if (!isWebsocket())
			{
				parseData();
				if (callbacks)
					callbacks->onClientDataReceived(*this);
			}
			else
			{
				printf("control frame\n");
			}

		}
		return recvOk;
	}

	int getClientId() const
	{
		return m_clientId;
	}

	eMethod extractMethod() const
	{
		eMethod retMethod=METHOD_UNKNOWN;
		if ( m_buffLen >= 3 ) 
		{
			if		(!_strnicmp(m_buff,"GET",3))  return METHOD_GET;
			else if (!_strnicmp(m_buff,"POST",4)) return METHOD_POST;
			else if (!_strnicmp(m_buff,"DELETE",6)) return METHOD_DELETE;
			else if (!_strnicmp(m_buff,"PUT",3)) return METHOD_PUT;
			else if (!_strnicmp(m_buff,"TRACE",5)) return METHOD_TRACE;
			else if (!_strnicmp(m_buff,"OPTIONS",7)) return METHOD_OPTIONS;
			else if (!_strnicmp(m_buff,"HEAD",4)) return METHOD_HEAD;
		}
		return retMethod;
	}

	std::string extractUri() const
	{
		return getUriInternal();
	}

	std::string extractUriBase() const
	{
		// /citiesJSON?north=44.1&south=-9.9&east=-22.4&west=55.2&lang=de&username=demo
		return getUriInternal("?");
	}

	void extractUriParameters(std::vector<UriParam>& outUriParams) const
	{
		if ( m_buffLen < 3 ) 
			return;

		char* str = strstr( const_cast<char*>(m_firstHeader.c_str()),"?");
		if ( !str )
			return;
		++str;
		while (str && *str && *str!=' ')
		{
			char* next = strstr(str,"&");
			if ( !next ) next = strstr(str," ");
			std::string key;
			while ( *str != '=' )
			{
				key+=*str;
				++str;
			}

			std::string value;
			++str;
			while (str != next)
			{
				value += *str;
				++str;
			}
			outUriParams.push_back( UriParam(key,value) );
			if ( *str != ' ' ) ++str;
		}
	}

	const std::string& getBody()
	{
		return m_lastBody;
	}

	int sendResponse(Response& r)
	{
		std::string msg = r.compose(*this);
		return sendData(msg.c_str(), msg.length());
	}

	bool isWebsocket() const 
	{
		return m_headers.hasKeyValueCaseIns("Upgrade", "Websocket");
	}

	RestServerHeaders& getHeaders()
	{
		return m_headers;
	}

  std::string calculateWebsocketHandshakeKey() const
  {
    return _calculateWebsocketHandshakeKey(m_headers.get("Sec-WebSocket-Key"));
  }

	static std::string _calculateWebsocketHandshakeKey(const std::string& key)
	{
		// SHA-1
    static const char* magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const std::string concat = key + magic;
    uint8_t digest[SHA1HashSize];
    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha, (const uint8_t*)concat.c_str(), concat.length());
    SHA1Result(&sha, &digest[0]);

    // Base64
    std::string acceptKey;
    base64encode((const char*)digest, SHA1HashSize, acceptKey);		
    return acceptKey;
	}

private:
	int sendData(const char* data, int length)
	{
		int leftBytes = length;
		while ( leftBytes>0 )
		{
			int sent = ::send(m_socket, data+length-leftBytes, leftBytes, 0);
			if ( sent != SOCKET_ERROR )
			{
				leftBytes -= sent;
			}
			else 
			{
				if ( WSAGetLastError() == WSAEWOULDBLOCK )
					leftBytes = 0;
				else
					return 1; // if any other error with socket, next step will catch&close it
			}
		}	
		return 0;
	}

	std::string getUriInternal(const char* secondStop=NULL) const
	{
		if ( m_buffLen < 3 || m_firstHeader.empty() ) 
			return "";
		char* str = strstr( const_cast<char*>(m_firstHeader.c_str())," ");
		if ( !str )
			return "";  
		++str;
		char* beginUri = str;
		if ( secondStop )
			str = strstr(beginUri, secondStop);
		if ( !secondStop || !str ) // no params
			str = strstr(beginUri, " ");
		if (!str) return beginUri;
		std::string uri; uri.reserve(128);
		return uri.assign(beginUri, (str-beginUri));
	}

	void parseData()
	{
		char* str = m_buff;
		int len = m_buffLen;

		// caching the header lines
		std::string line; line.reserve(256);
		m_headers.clear();
		m_firstHeader.clear();
		char* next = strstr(str, "\r\n");
		bool addedFirst=false;
		while ( next && next != str)
		{
			*next = 0;				
			line.reserve(next-str);
			line = str;
			if ( !addedFirst )
			{
				m_firstHeader = line;
				addedFirst = true;
			}
			else
			{
				// extract key:value
				auto colon = line.find(':');
				if ( colon > 0 )
				{
					auto val = line.find_first_not_of(" ", colon+1);
					if ( val >= colon )
					{
						std::string key = line.substr(0,colon);
						while (key.back()==' ') key.pop_back();
						std::string value = line.substr(val,line.length()-val);
						while (value.back()==' ') value.pop_back();
						m_headers.set(key, value);
					}
				}
			}
			next += 2;
			len -= (int)(next-str);
			str = next;
			next = strstr(str, "\r\n");
		}

		// caching body
		m_lastBody.clear();
		if ( next && *next && next == str )
		{
			next += 2; len -= 2;
			if ( len > 0 )
			{
				next[len]=0;
				m_lastBody.reserve(len);
				m_lastBody = next;
			}
		}
	}

private:
	char m_buff[BUFFLEN];
	std::string m_firstHeader;
	RestServerHeaders m_headers;
	std::string m_lastBody;
	SOCKET m_socket;
	int m_buffLen;
	int m_clientId;
};


// Single threaded, Synchronous Http Rest server
// Up to MAXC simultaneous connections
// BUFFLEN is the temporary recv buffer length in bytes
template<int MAXC>
class RestServerJSON
{
public:
	RestServerJSON(std::shared_ptr<RestServerCallbacksJSON> cbInterface=NULL)
		: m_port(0)
		, m_listenSocket(INVALID_SOCKET)
		, m_addrInfo(NULL)
		, m_callbacks(std::move(cbInterface))
	{
		WSADATA wsaData;
		// Initialize Winsock
		WSAStartup(MAKEWORD(2, 2), &wsaData);		
	}

	~RestServerJSON()
	{
		quit();
		WSACleanup();
	}

	int init(int port)
	{
		m_port = port;		

		addrinfo params = {0};
		params.ai_family = AF_INET;
		params.ai_socktype = SOCK_STREAM;
		params.ai_protocol = IPPROTO_TCP;
		params.ai_flags = AI_PASSIVE;

		//resolve the IP address and port to used by the server
		char portstr[128]={0};
		sprintf_s(portstr, "%d", m_port);
		if ( getaddrinfo(NULL, portstr, &params, &m_addrInfo) != 0) 
			return 1;

		// creating socket
		m_listenSocket = socket(m_addrInfo->ai_family, m_addrInfo->ai_socktype, m_addrInfo->ai_protocol);
		if (m_listenSocket == INVALID_SOCKET) 
			return 1;

		if ( !setBlocking(m_listenSocket,false) )
			return 1;

		// Setup the socket for HTTP
		if ( bind(m_listenSocket, m_addrInfo->ai_addr, (int)m_addrInfo->ai_addrlen) == SOCKET_ERROR )
			return 1;


		if ( ::listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR ) 
			return 1;

		return 0;
	}

	void stepListen()
	{
		// step for client sockets
		int firstAvail=-1;
		int clientCount = stepClients(&firstAvail);

		// accept incoming connections if room for it
		if ( clientCount != MAXC )
		{
			SOCKET clientSocket=INVALID_SOCKET;
			clientSocket = accept(m_listenSocket, NULL, NULL);
			if (clientSocket != INVALID_SOCKET && firstAvail >= 0 && firstAvail < MAXC ) 
			{
				RestServerClientConnectionJSON client(firstAvail, clientSocket);

				// add a new client, put it as non-blocking
				setBlocking(clientSocket, false);
				m_clients[firstAvail] = client;
				stepClients(&firstAvail);
			}
		}
	}

	void quit()
	{
		for ( int i = 0; i < MAXC; ++i )
			m_clients[i].quit();

		if ( m_addrInfo )
		{
			freeaddrinfo(m_addrInfo);
			m_addrInfo = NULL;
		}

		if ( m_listenSocket != INVALID_SOCKET )
		{
			closesocket(m_listenSocket);
			m_listenSocket = INVALID_SOCKET;
		}

		m_callbacks = NULL;		
	}

private:
	int stepClients(int* out1stAvailSlot)
	{
		int clientCount = 0;
		*out1stAvailSlot = -1;
		for ( int i=0; i < MAXC; ++i )
		{
			if ( m_clients[i].isValid() )
			{
				// any problem with the client socket?
				if ( !m_clients[i].stepRecv(m_callbacks.get()) )
					m_clients[i].quit();					
				else
					++clientCount;
			}
			else if ( *out1stAvailSlot == -1 ) // the slot is empty for this socket
			{
				*out1stAvailSlot = i;
			}
		}
		return clientCount;
	}

	bool setBlocking( SOCKET s, bool blocking )
	{
		u_long iMode = blocking ? 0 : 1;
		int err = ioctlsocket(s, FIONBIO, &iMode);
		return err == 0;
	}

private:
	std::shared_ptr<RestServerCallbacksJSON> m_callbacks;
	std::array<RestServerClientConnectionJSON,MAXC> m_clients;
	int m_port;
	SOCKET m_listenSocket;
	addrinfo* m_addrInfo;	
};


#ifndef _SHA_enum_
#define _SHA_enum_
enum
{
  shaSuccess = 0,
  shaNull,            /* Null pointer parameter */
  shaInputTooLong,    /* input data too long */
  shaStateError       /* called Input after Result */
};
#endif


/*
*  Define the SHA1 circular left shift macro
*/
#define SHA1CircularShift(bits,word) \
                (((word) << (bits)) | ((word) >> (32-(bits))))

/* Local Function Prototypes */
void SHA1PadMessage(SHA1Context *);
void SHA1ProcessMessageBlock(SHA1Context *);

/*
*  SHA1Reset
*
*  Description:
*      This function will initialize the SHA1Context in preparation
*      for computing a new SHA1 message digest.
*
*  Parameters:
*      context: [in/out]
*          The context to reset.
*
*  Returns:
*      sha Error Code.
*
*/
int SHA1Reset(SHA1Context *context)
{
  if (!context)
  {
    return shaNull;
  }

  context->Length_Low = 0;
  context->Length_High = 0;
  context->Message_Block_Index = 0;

  context->Intermediate_Hash[0] = 0x67452301;
  context->Intermediate_Hash[1] = 0xEFCDAB89;
  context->Intermediate_Hash[2] = 0x98BADCFE;
  context->Intermediate_Hash[3] = 0x10325476;
  context->Intermediate_Hash[4] = 0xC3D2E1F0;

  context->Computed = 0;
  context->Corrupted = 0;

  return shaSuccess;
}

/*
*  SHA1Result
*
*  Description:
*      This function will return the 160-bit message digest into the
*      Message_Digest array  provided by the caller.
*      NOTE: The first octet of hash is stored in the 0th element,
*            the last octet of hash in the 19th element.
*
*  Parameters:
*      context: [in/out]
*          The context to use to calculate the SHA-1 hash.
*      Message_Digest: [out]
*          Where the digest is returned.
*
*  Returns:
*      sha Error Code.
*
*/
int SHA1Result(SHA1Context *context, uint8_t Message_Digest[SHA1HashSize])
{
  int i;

  if (!context || !Message_Digest)
  {
    return shaNull;
  }

  if (context->Corrupted)
  {
    return context->Corrupted;
  }

  if (!context->Computed)
  {
    SHA1PadMessage(context);
    for (i = 0; i<64; ++i)
    {
      /* message may be sensitive, clear it out */
      context->Message_Block[i] = 0;
    }
    context->Length_Low = 0;    /* and clear length */
    context->Length_High = 0;
    context->Computed = 1;
  }

  for (i = 0; i < SHA1HashSize; ++i)
  {
    Message_Digest[i] = (uint8_t)(context->Intermediate_Hash[i >> 2] >> 8 * (3 - (i & 0x03)));
  }

  return shaSuccess;
}

/*
*  SHA1Input
*
*  Description:
*      This function accepts an array of octets as the next portion
*      of the message.
*
*  Parameters:
*      context: [in/out]
*          The SHA context to update
*      message_array: [in]
*          An array of characters representing the next portion of
*          the message.
*      length: [in]
*          The length of the message in message_array
*
*  Returns:
*      sha Error Code.
*
*/
int SHA1Input(SHA1Context    *context,
  const uint8_t  *message_array,
  unsigned       length)
{
  if (!length)
  {
    return shaSuccess;
  }

  if (!context || !message_array)
  {
    return shaNull;
  }

  if (context->Computed)
  {
    context->Corrupted = shaStateError;
    return shaStateError;
  }

  if (context->Corrupted)
  {
    return context->Corrupted;
  }
  while (length-- && !context->Corrupted)
  {
    context->Message_Block[context->Message_Block_Index++] =
      (*message_array & 0xFF);

    context->Length_Low += 8;
    if (context->Length_Low == 0)
    {
      context->Length_High++;
      if (context->Length_High == 0)
      {
        /* Message is too long */
        context->Corrupted = 1;
      }
    }

    if (context->Message_Block_Index == 64)
    {
      SHA1ProcessMessageBlock(context);
    }

    message_array++;
  }

  return shaSuccess;
}

/*
*  SHA1ProcessMessageBlock
*
*  Description:
*      This function will process the next 512 bits of the message
*      stored in the Message_Block array.
*
*  Parameters:
*      None.
*
*  Returns:
*      Nothing.
*
*  Comments:
*      Many of the variable names in this code, especially the
*      single character names, were used because those were the
*      names used in the publication.
*
*
*/
void SHA1ProcessMessageBlock(SHA1Context *context)
{
  const uint32_t K[] = {       /* Constants defined in SHA-1   */
    0x5A827999,
    0x6ED9EBA1,
    0x8F1BBCDC,
    0xCA62C1D6
  };
  int           t;                 /* Loop counter                */
  uint32_t      temp;              /* Temporary word value        */
  uint32_t      W[80];             /* Word sequence               */
  uint32_t      A, B, C, D, E;     /* Word buffers                */

                                   /*
                                   *  Initialize the first 16 words in the array W
                                   */
  for (t = 0; t < 16; t++)
  {
    W[t] = context->Message_Block[t * 4] << 24;
    W[t] |= context->Message_Block[t * 4 + 1] << 16;
    W[t] |= context->Message_Block[t * 4 + 2] << 8;
    W[t] |= context->Message_Block[t * 4 + 3];
  }

  for (t = 16; t < 80; t++)
  {
    W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
  }

  A = context->Intermediate_Hash[0];
  B = context->Intermediate_Hash[1];
  C = context->Intermediate_Hash[2];
  D = context->Intermediate_Hash[3];
  E = context->Intermediate_Hash[4];

  for (t = 0; t < 20; t++)
  {
    temp = SHA1CircularShift(5, A) +
      ((B & C) | ((~B) & D)) + E + W[t] + K[0];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  for (t = 20; t < 40; t++)
  {
    temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  for (t = 40; t < 60; t++)
  {
    temp = SHA1CircularShift(5, A) +
      ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  for (t = 60; t < 80; t++)
  {
    temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  context->Intermediate_Hash[0] += A;
  context->Intermediate_Hash[1] += B;
  context->Intermediate_Hash[2] += C;
  context->Intermediate_Hash[3] += D;
  context->Intermediate_Hash[4] += E;

  context->Message_Block_Index = 0;
}


/*
*  SHA1PadMessage
*
*  Description:
*      According to the standard, the message must be padded to an even
*      512 bits.  The first padding bit must be a '1'.  The last 64
*      bits represent the length of the original message.  All bits in
*      between should be 0.  This function will pad the message
*      according to those rules by filling the Message_Block array
*      accordingly.  It will also call the ProcessMessageBlock function
*      provided appropriately.  When it returns, it can be assumed that
*      the message digest has been computed.
*
*  Parameters:
*      context: [in/out]
*          The context to pad
*      ProcessMessageBlock: [in]
*          The appropriate SHA*ProcessMessageBlock function
*  Returns:
*      Nothing.
*
*/

void SHA1PadMessage(SHA1Context *context)
{
  /*
  *  Check to see if the current message block is too small to hold
  *  the initial padding bits and length.  If so, we will pad the
  *  block, process it, and then continue padding into a second
  *  block.
  */
  if (context->Message_Block_Index > 55)
  {
    context->Message_Block[context->Message_Block_Index++] = 0x80;
    while (context->Message_Block_Index < 64)
    {
      context->Message_Block[context->Message_Block_Index++] = 0;
    }

    SHA1ProcessMessageBlock(context);

    while (context->Message_Block_Index < 56)
    {
      context->Message_Block[context->Message_Block_Index++] = 0;
    }
  }
  else
  {
    context->Message_Block[context->Message_Block_Index++] = 0x80;
    while (context->Message_Block_Index < 56)
    {
      context->Message_Block[context->Message_Block_Index++] = 0;
    }
  }

  /*
  *  Store the message length as the last 8 octets
  */
  context->Message_Block[56] = (uint8_t)(context->Length_High >> 24);
  context->Message_Block[57] = (uint8_t)(context->Length_High >> 16);
  context->Message_Block[58] = (uint8_t)(context->Length_High >> 8);
  context->Message_Block[59] = (uint8_t)(context->Length_High);
  context->Message_Block[60] = (uint8_t)(context->Length_Low >> 24);
  context->Message_Block[61] = (uint8_t)(context->Length_Low >> 16);
  context->Message_Block[62] = (uint8_t)(context->Length_Low >> 8);
  context->Message_Block[63] = (uint8_t)(context->Length_Low);

  SHA1ProcessMessageBlock(context);
}

static void base64encode(const char* buf, int len, std::string& strOut)
{
  const char* bin2ascii = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  strOut.clear();

  const char* cur = buf;
  while (len > 0)
  {
    unsigned char ibuf[3] = { 0 };
    int ibuflen = len >= 3 ? 3 : len;
    memcpy(ibuf, cur, ibuflen);
    cur += ibuflen;
    len -= ibuflen;

    unsigned char obuf[4];
    obuf[0] = bin2ascii[(ibuf[0] >> 2)];
    obuf[1] = bin2ascii[((ibuf[0] << 4) & 0x30) | (ibuf[1] >> 4)];
    obuf[2] = bin2ascii[((ibuf[1] << 2) & 0x3c) | (ibuf[2] >> 6)];
    obuf[3] = bin2ascii[(ibuf[2] & 0x3f)];

    if (ibuflen >= 3)
    {
      strOut.append((char*)obuf, 4);
    }
    else // need to pad up to 4
    {
      switch (ibuflen)
      {
      case 1:
        obuf[2] = obuf[3] = '=';
        strOut.append((char*)obuf, 4);
        break;
      case 2:
        obuf[3] = '=';
        strOut.append((char*)obuf, 4);
      }
      break;
    }
  }
}

#pragma warning(default:4996)


