#pragma once
#include "types.hpp"
#include <stdint.h>

namespace vp6
{
class RangeDecoder
{
  public:
    RangeDecoder(uint8_t *buffer, int size);

    int ReadBitsNn(int bits);
    int ReadBits(int bits);
    int ReadBit();

    int GetBitProbabilityBranch(int prob);
    int GetBitProbability(int prob);
    unsigned int Renormalize();
    int GetTree(const Tree *tree, uint8_t *probs);

  private:
    int m_index;
    int m_size;
    uint8_t *m_buffer;
    int m_high;
    int m_bits;
    uint32_t m_codeword;
};
} // namespace vp6