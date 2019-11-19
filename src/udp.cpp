#include "udp.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

int UdpSocket::Init() {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ < 0) {
    return -1;
  }
  return 0;
}

int UdpSocket::Bind(const uint16_t port) {
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  int ret = bind(fd_, (struct sockaddr*)&server_addr, sizeof(server_addr));
  return ret;
}

void UdpSocket::Close() {
  if (fd_ > 0) close(fd_);
  fd_ = -1;
}

UdpSocket::~UdpSocket() { Close(); }

int UdpSocket::SendTo(const char* ip, const uint16_t port, const char* buff,
                      const int length) {
  struct sockaddr_in remote_addr;
  memset(&remote_addr, 0, sizeof(remote_addr));
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_addr.s_addr = inet_addr(ip);
  remote_addr.sin_port = htons(port);
  int ret = sendto(fd_, buff, length, 0, (struct sockaddr*)&remote_addr,
                   sizeof(remote_addr));
  return ret;
}

int UdpSocket::Send(const char* buff, const int length) {
  if (!remote_set_ || fd_ <= 0) return -1;
  int ret = sendto(fd_, buff, length, 0, (struct sockaddr*)&remote_addr_,
                   sizeof(remote_addr_));
  return ret;
}

int UdpSocket::RecvFrom(char* buff, const int length, sockaddr_in* addr) {
  sockaddr_in unused_addr;
  if (addr == nullptr) addr = &unused_addr;
  socklen_t sock_len = sizeof(sockaddr_in);
  int ret = recvfrom(fd_, buff, length, 0, (struct sockaddr*)addr, &sock_len);
  return ret;
}

void UdpSocket::SetSendIp(const char* ip, const uint16_t port) {
  if (ip == nullptr) return;
  memset(&remote_addr_, 0, sizeof(remote_addr_));
  remote_addr_.sin_family = AF_INET;
  remote_addr_.sin_addr.s_addr = inet_addr(ip);
  remote_addr_.sin_port = htons(port);
  remote_set_ = true;
}