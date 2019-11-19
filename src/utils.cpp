#include "utils.h"
#include "codec.h"

#include <cstdint>
#include <fstream>

int ReadConfig(Json::Value& config, const char* file_path) {
  std::ifstream fd;
  fd.open(file_path);
  if (!fd) {
    fprintf(stderr, "[Error] Cannot open config file\n");
    return -1;
  }
  try {
    fd >> config;
  } catch (...) {
    fprintf(stderr, "[Error] Cannot parse config file\n");
    fd.close();
    return -1;
  }
  fd.close();
  return 0;
}

MediaParam GenerateDefaultParam(CodecType type) {
  static const MediaParam amr_wb{99, 16000, 320};
  static const MediaParam codec2{96, 8000, 160};
  static const MediaParam pcm_u{0, 8000, 160};
  static const MediaParam pcm_a{8, 8000, 160};
  switch (type) {
    case CodecType::AMR_WB:
      return amr_wb;
    case CodecType::Codec2:
      return codec2;
    case CodecType::PcmA:
      return pcm_a;
    case CodecType::PcmU:
      return pcm_u;
  }
  static const MediaParam empty{0, 0, 0};
  return empty;
}