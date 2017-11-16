#include <iostream>

#include "Socket.h"

Socket::Socket() {
  Init();
}

Socket::~Socket() {
  CleanUp();
}

void Socket::Init() {
#if PLATFORM == PLATFORM_WINDOWS
  WSADATA WsaData;
  if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) {
    std::cout << "WSAStartup Failed." << std::endl;
  }
#else
#endif
}

void Socket::CleanUp() {
#if PLATFORM == PLATFORM_WINDOWS
  WSACleanup();
#endif
}


bool Socket::Open() {
  handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (handle <= 0)
  {
    std::cout << "failed to create socket\n";
    return false;
  }

  return true;
}

bool Socket::Bind(unsigned short port) {
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(handle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0) {
    std::cout << "failed to bind socket\n";
    return false;
}

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

  int nonBlocking = 1;
  if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
    printf("failed to set non-blocking\n");
    return false;
  }

#elif PLATFORM == PLATFORM_WINDOWS

  DWORD nonBlocking = 1;
  if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {
    printf("failed to set non-blocking\n");
    return false;
  }

#endif

  return true;
}

void Socket::Close() {
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close(handle);
#elif PLATFORM == PLATFORM_WINDOWS
  closesocket(handle);
#endif
}

bool Socket::IsOpen() const {
  return true;
}

bool Socket::Send(const Address & destination, const void * data, int size) {

  unsigned int address = destination.GetAddress();
  unsigned short port = destination.GetPort();

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(address);
  addr.sin_port = htons(port);

  int sent_bytes = sendto(handle, (const char*)data, size, 0, (sockaddr*)&addr, sizeof(sockaddr_in));

  if (sent_bytes != size)
  {
    std::cout << "failed to send packet\n" << WSAGetLastError();
    return false;
  }

  return true;
}

int Socket::Receive(Address & sender, void * data, int size) {

#if PLATFORM == PLATFORM_WINDOWS
  typedef int socklen_t;
#endif

  sockaddr_in from;
  socklen_t fromLength = sizeof(from);

  int bytes = recvfrom(handle, (char*)data, size, 0, (sockaddr*)&from, &fromLength);

  if (bytes <= 0) {
    //std::cout << "failed to recieve packet: " << WSAGetLastError() << std::endl;
    return 0;
  }

  unsigned int from_address = ntohl(from.sin_addr.s_addr);

  unsigned int from_port = ntohs(from.sin_port);

  sender.SetAddress(from_address, from_port);

  return bytes;
}