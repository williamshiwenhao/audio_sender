/*
@ Create on July 7 2019
@ Author SWH
 */

#ifndef __UDP_H__
#define __UDP_H__
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdint>

class UdpSocket {
 public:
  UdpSocket(const UdpSocket &) = delete;
  UdpSocket() = default;
  ~UdpSocket();
  /**
   * @Init the socket
   *
   * @return int Non zero on error
   */
  int Init();

  /**
   * @Bind socket to port
   *
   * @param port local port
   * @return int Non zero on error
   */
  int Bind(const uint16_t port);

  /**
   * @Send a udp packet to somewhere
   *
   * @param ip Send to IP
   * @param port Send to port
   * @param buff Send data
   * @param length Data length
   * @return int Length of data sent or negative on error
   */
  int SendTo(const char *ip, uint16_t port, const char *buff, const int length);

  /**
   * @Receive packet from socket, which is block type
   *
   * @param buff Receive buff
   * @param length Length of receive buff
   * @param addr Source address, NULL if don't need it
   * @return int Packet length or <= 0 on error
   */
  int RecvFrom(char *buff, const int length, sockaddr_in *addr = nullptr);

  /**
   * @Send to set address, SetSendIp must call SetSendIp first
   *
   * @param buff Send data
   * @param length Data length
   * @return int Length of data sent or negative on error
   */
  int Send(const char *buff, const int length);

  /**
   * @Set Send address
   *
   * @param ip
   * @param port
   */
  void SetSendIp(const char *ip, const uint16_t port);
  void Close();

 private:
  int fd_ = -1;
  bool remote_set_ = false;
  sockaddr_in remote_addr_;
};

#endif