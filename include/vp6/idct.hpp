#pragma once
#include <algorithm>
#include <stdint.h>

namespace vp6
{
class IDCT
{
  public:
    inline void Put(uint8_t *dest, int blockOffset, int stride, short *input)
    {
        Calculate(dest, blockOffset, stride, input, 1);
        std::fill_n(input, 64, 0);
    }

    inline void Add(uint8_t *dest, int blockOffset, int stride, short *input)
    {
      Calculate(dest, blockOffset, stride, input, 2);
      std::fill_n(input, 64, 0);
    }

    // IDCT
    void Calculate(uint8_t *dest, int blockOffset, int stride, short *input, int type);

  private:
    inline int M(int a, int b)
    {
        return (a * b) >> 16;
    }

    static constexpr int IdctAdjustBeforeShift = 8;
    static constexpr int xC1S7 = 64277;
    static constexpr int xC2S6 = 60547;
    static constexpr int xC3S5 = 54491;
    static constexpr int xC4S4 = 46341;
    static constexpr int xC5S3 = 36410;
    static constexpr int xC6S2 = 25080;
    static constexpr int xC7S1 = 12785;
};
} // namespace vp6