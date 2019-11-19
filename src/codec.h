/**
 * @file codec.h
 * @author SWH
 * @brief
 * @version 0.1
 * @date 2019-08-05
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __CODEC_H__
#define __CODEC_H__

#include <cstdint>

#include "udp.h"

#define __USE_AMRWB__

const int kPcmFrameLength = 320;  // Byte
const int kPcmFrameSample = 160;
const int kCodec2FrameLength = 6;   // Byte
const int kAmrWbFrameSample = 320;  // 320 sample = 20ms
const int16_t kAmrAllowDtx = 1;     // allow dtx
const int16_t kAmrEncodeMode = 8;

struct MediaParam {
  int payload_type;
  uint32_t clock_rate;
  uint32_t samples_pre_frames;
};

/**
 * @brief  Basic Class for codec
 *
 */
class Coder {
 public:
  Coder() = default;
  Coder(const Coder &) = delete;
  Coder &operator=(const Coder &) = delete;
  virtual ~Coder(){};

  /**
   * @brief Init the coder
   *
   * @return int 0 if succeed
   */
  virtual int Init() = 0;

  /**
   * @brief code
   *
   * @param input input code
   * @param length input code length
   * @param output output code
   * @param output_length Input the output buff length
   * @return int The length of packet when > 0 or error when < 0
   */
  virtual int Codec(uint8_t *input, const int length, uint8_t *output,
                    int output_length) = 0;

  /**
   * @brief Get white frame, in cases some frames are lost
   *
   * @param output White frame
   * @param length output buff length
   * @param frames need frames, 1 frame have 160 samples
   * @return int The length of packet when > 0 or error when < 0
   */
  virtual int GetWhite(uint8_t *output, const int length, const int frames) = 0;
};

// PCM
class PcmUEnCoder : public Coder {
 public:
  virtual int Init() { return 0; }
  virtual int Codec(uint8_t *input, const int length, uint8_t *output,
                    int output_length);
  virtual int GetWhite(uint8_t *output, const int length, const int frames);
};

class PcmUDeCoder : public Coder {
 public:
  virtual int Init() { return 0; }
  virtual int Codec(uint8_t *input, const int length, uint8_t *output,
                    int output_length);
  virtual int GetWhite(uint8_t *output, const int length, const int frames);
};

class PcmAEnCoder : public Coder {
 public:
  virtual int Init() { return 0; }
  virtual int Codec(uint8_t *input, const int length, uint8_t *output,
                    int output_length);
  virtual int GetWhite(uint8_t *output, const int length, const int frames);
};

class PcmADeCoder : public Coder {
 public:
  virtual int Init() { return 0; }
  virtual int Codec(uint8_t *input, const int length, uint8_t *output,
                    int output_length);
  virtual int GetWhite(uint8_t *output, const int length, const int frames);
};

#endif  //__CODEC_H__