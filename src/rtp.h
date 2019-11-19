/**
 * @file rtp.h
 * @author SWH
 * @brief Rtp class without library
 * @version 0.1
 * @date 2019-08-08
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __MY_RTP_H__
#define __MY_RTP_H__
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "codec.h"
#include "logger.h"


struct RtpHeader {
#if __BYTE_ORDER == __BIG_ENDIAN
  uint16_t v : 2;  /**< packet type/version	    */
  uint16_t p : 1;  /**< padding flag		    */
  uint16_t x : 1;  /**< extension flag		    */
  uint16_t cc : 4; /**< CSRC count			    */
  uint16_t m : 1;  /**< marker bit			    */
  uint16_t pt : 7; /**< payload type		    */
#else
  uint16_t cc : 4; /**< CSRC count			    */
  uint16_t x : 1;  /**< extension flag		    */
  uint16_t p : 1;  /**< padding flag		    */
  uint16_t v : 2;  /**< packet type/version	    */
  uint16_t pt : 7; /**< payload type		    */
  uint16_t m : 1;  /**< marker bit			    */
#endif
  uint16_t sequence_num;
  uint32_t timestamp;
  uint32_t ssrc;
};
static const int kRtpHeaderSize = 12;

const int kDefaultMast = 0;

class RtpSession {
 public:
  RtpSession() = default;
  RtpSession(const RtpSession&) = delete;
  RtpSession& operator=(const RtpSession&) = delete;
  /**
   * Init rtp session
   *
   * @param parm media param
   * @return int 0 if success
   */
  int Init(const MediaParam& param);

  /**
   * Generate rtp header
   *
   * @param buff pointer to buffer
   * @param length the length of buff
   * @param frame_num number of frames
   * @return int header length when return > 0, false when <= 0
   */
  int GenerateRtpHeader(uint8_t* buff, const unsigned length,
                        const int frame_num = 1);

  /**
   * Decode rtp header. Caution! SSRC conflict not handled, so each session
   * should use a unique port
   *
   * @param pkt Pointer to packet recived
   * @param pkt_len Packet length
   * @param lost_samples
   * @param recv_frame
   * @return int The rtp header length if >0 or decode false
   */
  int DecodeRtpHeader(uint8_t* pkt, const unsigned pkt_len, int& lost_samples,
                      int& recv_frame);

 private:
  uint32_t SSRCGenerator();
  MediaParam param_;
  RtpHeader send_header_, recv_header_;
  bool decode_first_packet_ = true;  /// For decode
};

#endif  // __MY_RTP_H__