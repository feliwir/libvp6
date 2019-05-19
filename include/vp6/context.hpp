#pragma once 
#include <stdint.h>
#include "types.hpp"

namespace vp6
{
	class Context
	{
	public:
		uint16_t Width;
		uint16_t Height;
		uint16_t MbWidth;
		uint16_t MbHeight;

		uint16_t YStride;
		uint16_t UvStride;
		uint32_t YSize;
		uint32_t UvSize;

		uint32_t Framecount;
		uint32_t LargestFrame;

		uint32_t Denominator;
		uint32_t Numerator;

		ScalingMode Scaling;
		bool IsGolden;

		Format Format;
		Macroblock* Macroblocks = nullptr;
		Profile Profile;
		CodingMode MacroblockType;		
	};
}