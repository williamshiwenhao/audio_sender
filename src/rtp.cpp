#include <arpa/inet.h>
#include <sys/time.h>    /* gettimeofday() */
#include <sys/types.h>   /* u_long */
#include <sys/utsname.h> /* uname() */
#include <unistd.h>      /* get..() */
#include <ctime>         /* clock() */
#include <limits>

#include "md5.h"
#include "rtp.h"

int RtpSession::Init(const MediaParam& param) {
  if (sizeof(RtpHeader) != kRtpHeaderSize) {
    PrintLog(
        "[Fatal Error] Wrong rtp header size!!! Check compiler memory "
        "alignment");
    return -1;
  }
  param_ = param;
  send_header_.v = 2;
  send_header_.p = 0;
  send_header_.x = 0;
  send_header_.cc = 0;
  send_header_.m = 0;
  send_header_.pt = param.payload_type;
  srand(time(NULL));
  send_header_.sequence_num = uint16_t(rand() & 0xffff);
  send_header_.timestamp = rand();
  send_header_.ssrc = RtpSession::SSRCGenerator();
  decode_first_packet_ = true;
  return 0;
}

int RtpSession::GenerateRtpHeader(uint8_t* buff, const unsigned length,
                                  const int frame_num) {
  send_header_.sequence_num++;
  send_header_.timestamp += frame_num * param_.samples_pre_frames;
  if (length < sizeof(RtpHeader)) {
    PrintLog("[Error] No enough memory for rtp header");
    return -1;
  }
  static const int no_order_head = 2;  // Value less than 1byte length
  memcpy(buff, &send_header_, no_order_head);
  // Change to network order
  RtpHeader* now = (RtpHeader*)buff;
  now->sequence_num = htons(send_header_.sequence_num);
  now->timestamp = htonl(send_header_.timestamp);
  now->ssrc = htonl(send_header_.ssrc);
  return sizeof(RtpHeader);
}

int RtpSession::DecodeRtpHeader(uint8_t* pkt, const unsigned pkt_len,
                                int& lost_samples, int& recv_frame) {
  lost_samples = 0;
  if (pkt_len < sizeof(RtpHeader)) {
    PrintLog("[Warning] Packet's length less than rtp header length");
    return -1;
  }
  RtpHeader* now_head = (RtpHeader*)pkt;
  if (now_head->v != 2 || now_head->x != 0 || now_head->cc != 0 ||
      now_head->m != 0) {
    PrintLog("[Error] Cannot parse rtp head");
    return -1;
  }
  // Change to host order
  now_head->sequence_num = ntohs(now_head->sequence_num);
  now_head->timestamp = ntohl(now_head->timestamp);
  now_head->ssrc = ntohl(now_head->ssrc);
  // Though ssrc is just a id, I change it to host order for safe
  if (decode_first_packet_) {
    decode_first_packet_ = false;
    recv_header_ = *now_head;
  } else {
    if (now_head->pt != recv_header_.pt) {
      PrintLog("[Error] Payload type changed, cannot handle it");
      return -1;
    }
    uint16_t diff = now_head->timestamp - recv_header_.timestamp;
    if (diff > std::numeric_limits<uint16_t>::max() >> 1) {
      // old packet
      lost_samples = diff - std::numeric_limits<uint16_t>::max();
      return -1;
    } else {
      lost_samples = diff;
      recv_header_ = *now_head;
    }
    return sizeof(RtpHeader);
  }
  // recv_frame = (pkt_len - sizeof(RtpHeader)) / param_.byte_pre_frame;
  // recv_frame must >= 0, so change it to uin32_t directly
  // recv_header_.timestamp += (uint32_t)(recv_frame)*param_.samples_pre_frames;
  return sizeof(RtpHeader);
}

uint32_t RtpSession::SSRCGenerator() {
  uint8_t buff[16];  // md5 result
  // Hash source
  struct {
    struct timeval tv;
    clock_t cpu;
    pid_t pid;
    u_long hid;
    uid_t uid;
    gid_t gid;
    struct utsname name;
  } s;

  gettimeofday(&s.tv, 0);
  uname(&s.name);
  s.cpu = clock();
  s.pid = getpid();
  s.hid = gethostid();
  s.uid = getuid();
  s.gid = getgid();

  // Calculate md5 of s
  MD5_CTX md5;
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)&s, sizeof(s));
  MD5Final((unsigned char*)buff, &md5);
  uint32_t* p32 = (uint32_t*)buff;
  uint32_t SSRC = *p32;
  p32++;
  for (int i = 0; i < 3; ++i) {
    SSRC ^= *p32;
    p32++;
  }
  return SSRC;
}
