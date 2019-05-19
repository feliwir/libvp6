#pragma once
#include <stdint.h>
#include <array>

namespace vp6
{
	class ModelType
	{
	public:
		ModelType();

		void Reset();

		uint8_t CoeffDccv[2][11];
		uint8_t CoeffDcct[2][36][5];
		uint8_t CoeffIndexToPos[64];
		uint8_t CoeffRunv[2][14];
		uint8_t CoeffRact[2][3][6][11];
		uint8_t CoeffReorder[64];

		// Used for vector prediction
		uint8_t VectorDct[2];
		uint8_t VectorFdv[2][8];
		uint8_t VectorPdv[2][7];
		uint8_t VectorSig[2];

		uint8_t MacroblockType[3][10][10];

		constexpr void InitializeCoeffOrderTable()
		{
			uint8_t idx = 1;
			CoeffIndexToPos[0] = 0;

			for (uint8_t i = 0; i < 16; i++)
				for (uint8_t pos = 1; pos < 64; pos++)
					if (CoeffReorder[pos] == i)
						CoeffIndexToPos[idx++] = pos;
		}

	private:
		uint8_t m_vectorPdi[2][2];
		uint8_t m_mbTypesStats[3][10][2];
	};
}