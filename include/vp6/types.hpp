#pragma once
#include <stdint.h>

namespace vp6
{
struct Motionvector
{
  public:
    Motionvector() : X(0), Y(0)
    {
    }

    Motionvector(int x, int y) : X(x), Y(y)
    {
    }

    bool operator==(const Motionvector &other) const
    {
        return X == other.X && Y == other.Y;
    }

    bool operator==(const int &other) const
    {
        return X == other && Y == other;
    }

    int X;
    int Y;
};

struct Tree
{
    int8_t Value;
    int8_t ProbIdx;
};

enum class FrameSelect : int
{
    NONE = -1,
    CURRENT = 0,
    PREVIOUS = 1,
    GOLDEN = 2,
};

struct Reference
{
    uint8_t NotNullDc;
    FrameSelect RefFrame;
    int16_t DcCoeff;
};

enum class FormatType
{
    VP60 = 6,
    VP61 = 7,
    VP62 = 8,
};

enum class ProfileType
{
    SIMPLE = 0,
    ADVANCED = 3
};

enum class ScalingMode
{
    MAINTAIN_ASPECT_RATIO = 0,
    SCALE_TO_FIT = 1,
    CENTER = 2,
    OTHER = 4
};

enum class CodingMode
{
    // CODING MODE            PREDICTION FRAME    MOTIONVECTOR
    INTER_MV = 0,         //  PREVIOUS            FIXED (0,0)
    INTRA = 1,            //  NONE                NONE
    INTER_PLUS_MV = 2,    //  PREVIOUS            NEWLY CALCULATED
    INTER_NEAREST_MV = 3, //  PREVIOUS            SAME MV AS NEAREST
    INTER_NEAR_MV = 4,    //  PREVIOUS            SAME MV AS NEAR
    USING_GOLDEN = 5,     //  GOLDEN              FIXED (0,0)
    GOLDEN_MV = 6,        //  GOLDEN              NEWLY CALCULATED
    INTER_FOURMV = 7,     //  PREVIOUS            EACH LUMABLOCK HAS ONE MV
    GOLD_NEAREST_MV = 8,  //  GOLDEN              SAME MV AS NEAREST
    GOLD_NEAR_MV = 9,     //  GOLDEN              SAME MV AS NEAR
};

struct Macroblock
{
    CodingMode Type;
    Motionvector Mv;
};

enum class FrameType : int
{
    /// <summary>
    /// Can be decoded without a previous frame. Also called Keyframe
    /// </summary>
    INTRA = 0,
    /// <summary>
    /// Requires a reference frame
    /// </summary>
    INTER = 1
};
} // namespace vp6