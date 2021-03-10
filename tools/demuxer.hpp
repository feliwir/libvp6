#pragma once
#include "util.hpp"
#include <array>
#include <stdint.h>
#include <vp6/decode.hpp>

constexpr uint32_t MakeTag(const char f[4])
{
    return f[0] | f[1] << 8 | f[2] << 16 | f[3] << 24;
}

namespace vp6
{

// VP6 WITH ALPHA
constexpr uint32_t AVP6_TAG = MakeTag("AVP6");
// ALPHA MASK TAGS
constexpr uint32_t AVhd_TAG = MakeTag("AVhd");
constexpr uint32_t AV0K_TAG = MakeTag("AV0K");
constexpr uint32_t AV0F_TAG = MakeTag("AV0F");
// VIDEO TAGS
constexpr uint32_t MVhd_TAG = MakeTag("MVhd");
constexpr uint32_t MV0K_TAG = MakeTag("MV0K");
constexpr uint32_t MV0F_TAG = MakeTag("MV0F");
// KNOWN CODECS
constexpr uint32_t vp60_TAG = MakeTag("vp60");
constexpr uint32_t vp61_TAG = MakeTag("vp61");

class Demuxer
{
  public:
    Demuxer(std::istream &&input);

  private:
    bool ProcessHeader();
    bool ProcessVideoHeader(std::shared_ptr<DecodingContext> &ctx);
    bool CheckCodec();
    bool ReadPacket();

  private:
    std::shared_ptr<DecodingContext> m_color;
    std::shared_ptr<DecodingContext> m_alpha;
    std::vector<uint8_t> m_buffer;

    BinaryReader m_reader;
    bool m_valid = false;
    uint32_t m_fourcc = 0;
};
} // namespace vp6