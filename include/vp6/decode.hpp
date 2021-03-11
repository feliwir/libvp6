#pragma once
#include "context.hpp"
#include "frame.hpp"
#include "huffman.hpp"
#include "idct.hpp"
#include "model.hpp"
#include "rangedecoder.hpp"
#include "types.hpp"

#include <array>
#include <memory>
#include <stdint.h>

namespace vp6
{
class DecodingContext : public Context
{
  public:
    DecodingContext(uint16_t width, uint16_t height, uint32_t denominator, uint32_t numerator, uint32_t framecount,
                    bool flip);

    ~DecodingContext();

    void ProcessPacket(uint8_t *packet, int packet_size);
    void ParseCoefficients(int dequantAc);
    void ParseCoefficientsHuffman(int dequantAc);
    void AddPredictorsDc(FrameSelect ref_frame, int dequantAc);

    inline std::shared_ptr<Frame> GetCurrentFrame()
    {
        return Frames[static_cast<int>(FrameSelect::CURRENT)];
    }

  public:
    Reference *AboveBlocks = nullptr;
    RangeDecoder *RangeDec = nullptr;
    RangeDecoder *CoeffDec = nullptr;
    ModelType Model;
    std::array<std::shared_ptr<Huffman>, 2> HuffDccv;
    std::array<std::shared_ptr<Huffman>, 2> HuffRunv;
    std::shared_ptr<Huffman> HuffRact[2][3][6];
    int16_t PrevDc[3][3];
    std::array<Reference, 4> LeftBlocks;
    std::array<int32_t, 6> AboveBlocksIdx;
    std::array<int32_t, 6> BlockOffset;
    std::array<Motionvector, 2> VectorCandidate;
    int VectorCandidatePos;
    std::array<Motionvector, 6> Mvs;
    int16_t BlockCoeff[6][64];
    std::array<std::shared_ptr<Frame>, 3> Frames;

    // Inverse discrete cosine transform
    IDCT Idct;

    // Flip or not
    int Flip;
    int Frbi;
    int Srbi;

  private:
    uint32_t m_nbNull[2][2];
};
} // namespace vp6