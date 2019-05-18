#pragma once
#include <stdint.h>
#include <array>
#include "context.h"
#include "frame.h"
#include "types.h"
#include "model.h"
#include "idct.h"
#include "huffman.h"
#include "rangedecoder.h"

namespace vp6
{
	class DecodingContext : public Context
	{
	public:
		DecodingContext(uint16_t width, uint16_t height, 
						uint32_t denominator, uint32_t numerator, 
						uint32_t framecount, bool flip);

		~DecodingContext();

		void ProcessPacket(uint8_t* packet, int packet_size);
		void ParseCoefficients(int dequantAc);
		void ParseCoefficientsHuffman(int dequantAc);

	public:
		Reference* AboveBlocks = nullptr;
		RangeDecoder* RangeDec = nullptr;
		RangeDecoder* CoeffDec = nullptr;
	private:
		std::array<Motionvector, 6> m_mvs;
		std::array<std::shared_ptr<Frame>, 3> m_frames;

		std::array<Reference, 4> m_leftBlocks;
		std::array<int32_t, 6> m_aboveBlocksIdx;

		Model m_model;
		int16_t m_prevDc[3][3];
		std::array<Motionvector, 2> m_vectorCandidate;
		int16_t m_blockCoeff[6][64];
		std::array<int32_t, 6> m_blockOffset;
		IDCT m_idct;
		std::array<std::shared_ptr<Huffman>, 2> m_huffDccv;
		std::array<std::shared_ptr<Huffman>, 2> m_huffRunv;
		std::shared_ptr<Huffman> m_huffRact[2][3][6];
		uint32_t m_nbNull[2][2];
		int m_flip;
		int m_frbi;
		int m_srbi;
	};
}