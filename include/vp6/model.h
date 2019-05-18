#pragma once
#include <stdint.h>

namespace vp6
{
	class Model
	{
	public:
		Model();

		uint8_t CoeffDccv[2][11];
		uint8_t CoeffDcct[2][36][5];
		uint8_t CoeffIndexToPos[64];
		uint8_t CoeffRunv[2][14];
		uint8_t CoeffRact[2][3][6][11];
	private:
		constexpr void InitializeCoeffOrderTable()
		{
			uint8_t idx = 1;

			for (uint8_t i = 0; i < 16; i++)
				for (uint8_t pos = 1; pos < 64; pos++)
					if (m_coeffReorder[pos] == i)
						CoeffIndexToPos[idx++] = pos;
		}

	private:
		uint8_t m_coeffReorder[64];
		uint8_t m_vectorSig[2];
		uint8_t m_vectorDct[2];
		uint8_t m_vectorPdi[2][2];
		uint8_t m_vectorPdv[2][7];
		uint8_t m_vectorFdv[2][8];
		uint8_t m_defMbTypesStats[3][10][2];
		uint8_t m_mbType[3][10][10];
	};
}