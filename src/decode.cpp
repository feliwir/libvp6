#include <vp6/decode.h>
#include "tables.h"

vp6::DecodingContext::DecodingContext(uint16_t width, uint16_t height,
	uint32_t denominator, uint32_t numerator,
	uint32_t framecount, bool flip)
{
	Width = width;
	Height = height;
	Denominator = denominator;
	Numerator = numerator;
	Framecount = framecount;

	if (flip)
	{
		m_flip = -1;
		m_frbi = 2;
		m_srbi = 0;
	}
	else
	{
		m_flip = 1;
		m_frbi = 0;
		m_srbi = 2;
	}
}

vp6::DecodingContext::~DecodingContext()
{
	if (RangeDec != nullptr)
		delete RangeDec;

	if (AboveBlocks != nullptr)
		delete AboveBlocks;
}

void vp6::DecodingContext::ProcessPacket(uint8_t * data, int packet_size)
{
	m_frames[(int)FrameSelect::PREVIOUS] = m_frames[(int)FrameSelect::CURRENT];

	auto frame = m_frames[(int)FrameSelect::CURRENT] = std::make_shared<Frame>(data, packet_size - 8, this);
	frame->Decode();
}

void vp6::DecodingContext::ParseCoefficients(int dequantAc)
{
	int ctx, coeff, coeff_index, idx;
	int pt = 0, sign, cg;
	uint8_t* model1, * model2, * model3;

	for (int b = 0; b < 6; ++b)
	{
		//codetyps
		int ct = 1;
		int run = 1;

		if (b > 3)
			pt = 1;

		ctx = m_leftBlocks[Tables::B6To4[b]].NotNullDc + AboveBlocks[m_aboveBlocksIdx[b]].NotNullDc;
		model1 = m_model.CoeffDccv[pt];
		model2 = m_model.CoeffDcct[pt][ctx];

		coeff_index = 0;
		for (;;)
		{
			if ((coeff_index > 1 && ct == 0) || CoeffDec->GetBitProbabilityBranch(model2[0]) > 0)
			{
				//Parse a coefficient
				if (CoeffDec->GetBitProbabilityBranch(model2[2]) > 0)
				{
					if (CoeffDec->GetBitProbabilityBranch(model2[3]) > 0)
					{
						idx = CoeffDec->GetTree(Tables::PcTree, model1);
						coeff = Tables::CoeffBias[idx + 5];
						for (int i = Tables::CoeffBitLength[idx]; i >= 0; --i)
							coeff += CoeffDec->GetBitProbability(Tables::CoeffParseTable[idx][i]) << i;
					}
					else
					{
						if (CoeffDec->GetBitProbabilityBranch(model2[4]) > 0)
							coeff = 3 + CoeffDec->GetBitProbability(model1[5]);
						else
							coeff = 2;
					}

					ct = 2;
				}
				else
				{
					ct = 1;
					coeff = 1;
				}

				sign = CoeffDec->ReadBit();
				coeff = (coeff ^ -sign) + sign;
				if (coeff_index > 0)
					coeff *= dequantAc;

				idx = m_model.CoeffIndexToPos[coeff_index];
				m_blockCoeff[b][Tables::Scantable[idx]] = (short)coeff;
				run = 1;
			}
			//Parse a run
			else
			{
				ct = 0;
				if (coeff_index > 0)
				{
					if (CoeffDec->GetBitProbabilityBranch(model2[1]) <= 0)
						break;

					model3 = m_model.CoeffRunv[coeff_index >= 6];
					run = CoeffDec->GetTree(Tables::PcrTree, model3);
					if (run <= 0)
					{
						run = 9;
						for (int i = 0; i < 6; ++i)
						{
							run += CoeffDec->GetBitProbability(model3[i + 8]) << i;
						}
					}
				}
			}

			coeff_index += run;
			if (coeff_index >= 64)
				break;

			cg = Tables::CoeffGroups[coeff_index];
			model1 = model2 = m_model.CoeffRact[pt][ct][cg];
		}
		m_leftBlocks[Tables::B6To4[b]].NotNullDc = AboveBlocks[m_aboveBlocksIdx[b]].NotNullDc = m_blockCoeff[b, 0] != 0;
	}
}
