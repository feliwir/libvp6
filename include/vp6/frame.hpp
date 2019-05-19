#pragma once
#include <stdint.h>
#include "types.hpp"
#include <functional>

namespace vp6
{
	class DecodingContext;

	class Frame
	{
	public:
		Frame(uint8_t* data, int packet_size,DecodingContext* ctx);
		~Frame();
		void Decode();
	private:
		void ParseCoeffModels();
		void DecodeMacroblock(int row, int column);
		CodingMode DecodeMotionvector(int row, int column);
		CodingMode ParseMacroblockType(int vt);
		int GetVectorPredictors(int row, int column, FrameSelect ref_frame);
		void ParseVectorAdjustment(Motionvector& vect);
		void RenderMacroblock(CodingMode mode);

		//Output data
		std::vector<uint8_t*> Planes;
		std::vector<int> Strides;
	private:
		//Intra or Interframe
		FrameType m_type;
		bool m_isGolden;

		//Arithmetic coding 
		int32_t m_quantizer;
		int32_t m_dequant_ac;
		int32_t m_dequant_dc;
		bool m_seperateCoeff;
		uint16_t m_coeffOffset;

		//Fragments/ Amount of MBs
		uint8_t m_vfrags;
		uint8_t m_hfrags;

		//Calculated size
		int32_t m_dimX;
		int32_t m_dimY;

		//Output fragments
		int32_t m_ovfrags;
		int32_t m_ohfrags;

		//Calculated output size
		int32_t m_presX;
		int32_t m_presY;

		//Delegate for parsing coefficients
		std::function<void(int)> m_parseCoeff;



		bool m_useHuffman;
		DecodingContext* m_ctx;
	};
}