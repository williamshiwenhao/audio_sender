/**
 * @file codec.cpp
 * @author SWH
 * @brief
 * @version 0.1
 * @date 2019-08-05
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <cstdio>
#include <cstring>

#include "codec.h"
#include "logger.h"

/******************************************************************* */
/*Pcm U law Codec                                                   */
/***************************************************************** */

int PcmUEnCoder::Codec(uint8_t* input, const int length, uint8_t* output,
                       int output_length) {
  if (output_length<length>> 1) {
    PrintLog("[Warning] Pcm u encoder: no enough memory");
    return -1;
  }
  static const int16_t seg_end[8] = {0xFF,  0x1FF,  0x3FF,  0x7FF,
                                     0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};
  const int kBIAS{0x84};
  int16_t* speech = (int16_t*)input;
  int frames = length >> 1;
  for (int i = 0; i < frames; ++i) {
    int mask;
    int seg = 0;
    unsigned char uval;
    int pcm_val = speech[i];

    /* Get the sign and the magnitude of the value. */
    if (pcm_val < 0) {
      pcm_val = kBIAS - pcm_val;
      mask = 0x7F;
    } else {
      pcm_val += kBIAS;
      mask = 0xFF;
    }

    /* Convert the scaled magnitude to segment number. */
    for (; seg < 8; ++seg) {
      if (pcm_val <= seg_end[seg]) {
        break;
      }
    }

    /*
     * Combine the sign, segment, quantization bits;
     * and complement the code word.
     */
    if (seg >= 8) /* out of range, return maximum value. */
      output[i] = 0x7F ^ mask;
    else {
      uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
      output[i] = uval ^ mask;
    }
  }
  return frames;
}

int PcmUEnCoder::GetWhite(uint8_t* output, const int length, const int frames) {
  if (length < frames) {
    PrintLog("[Error] Pcm u law encoder GetWhite: no enough memory");
    return -1;
  }
  memset(output, 0, frames);
  return frames;
}

int PcmUDeCoder::Codec(uint8_t* input, const int length, uint8_t* output,
                       int output_length) {
  if (output_length < length << 1) {
    PrintLog("[Warning] Pcm u decoder: no enough memory");
    return -1;
  }
  int16_t* speech = (int16_t*)output;
  static const int16_t ulaw_table[] = {
      -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956, -23932,
      -22908, -21884, -20860, -19836, -18812, -17788, -16764, -15996, -15484,
      -14972, -14460, -13948, -13436, -12924, -12412, -11900, -11388, -10876,
      -10364, -9852,  -9340,  -8828,  -8316,  -7932,  -7676,  -7420,  -7164,
      -6908,  -6652,  -6396,  -6140,  -5884,  -5628,  -5372,  -5116,  -4860,
      -4604,  -4348,  -4092,  -3900,  -3772,  -3644,  -3516,  -3388,  -3260,
      -3132,  -3004,  -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,
      -1980,  -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
      -1372,  -1308,  -1244,  -1180,  -1116,  -1052,  -988,   -924,   -876,
      -844,   -812,   -780,   -748,   -716,   -684,   -652,   -620,   -588,
      -556,   -524,   -492,   -460,   -428,   -396,   -372,   -356,   -340,
      -324,   -308,   -292,   -276,   -260,   -244,   -228,   -212,   -196,
      -180,   -164,   -148,   -132,   -120,   -112,   -104,   -96,    -88,
      -80,    -72,    -64,    -56,    -48,    -40,    -32,    -24,    -16,
      -8,     0,      32124,  31100,  30076,  29052,  28028,  27004,  25980,
      24956,  23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
      15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,  11900,
      11388,  10876,  10364,  9852,   9340,   8828,   8316,   7932,   7676,
      7420,   7164,   6908,   6652,   6396,   6140,   5884,   5628,   5372,
      5116,   4860,   4604,   4348,   4092,   3900,   3772,   3644,   3516,
      3388,   3260,   3132,   3004,   2876,   2748,   2620,   2492,   2364,
      2236,   2108,   1980,   1884,   1820,   1756,   1692,   1628,   1564,
      1500,   1436,   1372,   1308,   1244,   1180,   1116,   1052,   988,
      924,    876,    844,    812,    780,    748,    716,    684,    652,
      620,    588,    556,    524,    492,    460,    428,    396,    372,
      356,    340,    324,    308,    292,    276,    260,    244,    228,
      212,    196,    180,    164,    148,    132,    120,    112,    104,
      96,     88,     80,     72,     64,     56,     48,     40,     32,
      24,     16,     8,      0};
  for (int i = 0; i < length; ++i) {
    speech[i] = ulaw_table[input[i]];
  }
  return length << 1;
}

int PcmUDeCoder::GetWhite(uint8_t* output, const int length, const int frames) {
  if (length < frames << 1) {
    PrintLog("[Error] Pcm u law decoder GetWhite: no enough memory");
    return -1;
  }
  memset(output, 0, frames << 1);
  return frames << 1;
}

/******************************************************************* */
/*Pcm A law Codec                                                   */
/***************************************************************** */

int PcmAEnCoder::Codec(uint8_t* input, const int length, uint8_t* output,
                       int output_length) {
  if (output_length<length>> 1) {
    PrintLog("[Warning] Pcm a law encoder: no enough memory");
    return -1;
  }
  static const int16_t seg_end[8] = {0xFF,  0x1FF,  0x3FF,  0x7FF,
                                     0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};
  int16_t* speech = (int16_t*)input;
  int frames = length >> 1;
  for (int i = 0; i < frames; ++i) {
    int mask;
    int seg = 0;
    uint8_t aval;
    int16_t& pcm_val = speech[i];
    static const int kSegShift = 4;
    static const uint8_t kQuantMask = 0x0f;

    if (pcm_val >= 0) {
      mask = 0xD5; /* sign (7th) bit = 1 */
    } else {
      mask = 0x55; /* sign bit = 0 */
      pcm_val = -pcm_val - 8;

      if (pcm_val < 0) pcm_val = 0;
    }

    /* Convert the scaled magnitude to segment number. */
    for (; seg < 8; ++seg) {
      if (pcm_val <= seg_end[seg]) {
        break;
      }
    }

    /* Combine the sign, segment, and quantization bits. */

    if (seg >= 8) /* out of range, return maximum value. */
      return (0x7F ^ mask);
    else {
      aval = seg << kSegShift;
      if (seg < 2)
        aval |= (pcm_val >> 4) & kQuantMask;
      else
        aval |= (pcm_val >> (seg + 3)) & kQuantMask;
      output[i] = aval ^ mask;
    }
  }
  return frames;
}

int PcmAEnCoder::GetWhite(uint8_t* output, const int length, const int frames) {
  if (length < frames) {
    PrintLog("[Error] Pcm a law encoder GetWhite: no enough memory");
    return -1;
  }
  memset(output, 0, frames);
  return frames;
}

int PcmADeCoder::Codec(uint8_t* input, const int length, uint8_t* output,
                       int output_length) {
  if (output_length < length << 1) {
    PrintLog("[Warning] Pcm a law decoder: no enough memory");
    return -1;
  }
  static const int16_t a_law_table[] = {
      -5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736,  -7552,
      -7296,  -8064,  -7808,  -6528,  -6272,  -7040,  -6784,  -2752,  -2624,
      -3008,  -2880,  -2240,  -2112,  -2496,  -2368,  -3776,  -3648,  -4032,
      -3904,  -3264,  -3136,  -3520,  -3392,  -22016, -20992, -24064, -23040,
      -17920, -16896, -19968, -18944, -30208, -29184, -32256, -31232, -26112,
      -25088, -28160, -27136, -11008, -10496, -12032, -11520, -8960,  -8448,
      -9984,  -9472,  -15104, -14592, -16128, -15616, -13056, -12544, -14080,
      -13568, -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296,
      -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,   -88,
      -72,    -120,   -104,   -24,    -8,     -56,    -40,    -216,   -200,
      -248,   -232,   -152,   -136,   -184,   -168,   -1376,  -1312,  -1504,
      -1440,  -1120,  -1056,  -1248,  -1184,  -1888,  -1824,  -2016,  -1952,
      -1632,  -1568,  -1760,  -1696,  -688,   -656,   -752,   -720,   -560,
      -528,   -624,   -592,   -944,   -912,   -1008,  -976,   -816,   -784,
      -880,   -848,   5504,   5248,   6016,   5760,   4480,   4224,   4992,
      4736,   7552,   7296,   8064,   7808,   6528,   6272,   7040,   6784,
      2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,   3776,
      3648,   4032,   3904,   3264,   3136,   3520,   3392,   22016,  20992,
      24064,  23040,  17920,  16896,  19968,  18944,  30208,  29184,  32256,
      31232,  26112,  25088,  28160,  27136,  11008,  10496,  12032,  11520,
      8960,   8448,   9984,   9472,   15104,  14592,  16128,  15616,  13056,
      12544,  14080,  13568,  344,    328,    376,    360,    280,    264,
      312,    296,    472,    456,    504,    488,    408,    392,    440,
      424,    88,     72,     120,    104,    24,     8,      56,     40,
      216,    200,    248,    232,    152,    136,    184,    168,    1376,
      1312,   1504,   1440,   1120,   1056,   1248,   1184,   1888,   1824,
      2016,   1952,   1632,   1568,   1760,   1696,   688,    656,    752,
      720,    560,    528,    624,    592,    944,    912,    1008,   976,
      816,    784,    880,    848};
  int16_t* speech = (int16_t*)output;
  for (int i = 0; i < length; ++i) {
    speech[i] = a_law_table[input[i]];
  }
  return length << 1;
}

int PcmADeCoder::GetWhite(uint8_t* output, const int length, const int frames) {
  if (length < frames << 1) {
    PrintLog("[Error] Pcm a law decoder GetWhite: no enough memory");
    return -1;
  }
  memset(output, 0, frames << 1);
  return frames << 1;
}