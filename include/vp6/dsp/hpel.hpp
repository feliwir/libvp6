#pragma once
#include "../types.hpp"
#include <functional>
#include <stdint.h>
#include <vector>

union unaligned_32 {
    uint32_t l;
} __attribute__((packed)) av_alias;
#define AV_RN(s, p) (((const union unaligned_##s *)(p))->l)
#define AV_RN32(p) AV_RN(32, p)
#define BYTE_VEC32(c) ((c)*0x01010101UL)

static inline uint32_t no_rnd_avg32(uint32_t a, uint32_t b)
{
    return (a & b) + (((a ^ b) & ~BYTE_VEC32(0x01)) >> 1);
}

namespace vp6
{
class HPEL
{
  public:
    template <size_t H> static inline void Put(uint8_t *dst, const uint8_t *src, size_t linesize)
    {
        for (int i = 0; i < H; i++)
        {
            uint32_t a, b;
            a = AV_RN32(&src[i * linesize]);
            b = AV_RN32(&src[i * linesize + 1]);
            *((uint32_t *)&dst[i * linesize]) = no_rnd_avg32(a, b);
            a = AV_RN32(&src[i * linesize + 4]);
            b = AV_RN32(&src[i * linesize + 5]);
            *((uint32_t *)&dst[i * linesize + 4]) = no_rnd_avg32(a, b);
        }
    }
};
} // namespace vp6