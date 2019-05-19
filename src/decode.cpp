#include <vp6/decode.hpp>
#include "tables.hpp"

vp6::DecodingContext::DecodingContext(uint16_t width, uint16_t height,
	uint32_t denominator, uint32_t numerator,
	uint32_t framecount, bool flip)
{
	Width = width;
	Height = height;
	Denominator = denominator;
	Numerator = numerator;
	Framecount = framecount;

	std::memset(BlockCoeff, 0x0, sizeof(BlockCoeff));

	if (flip)
	{
		Flip = -1;
		Frbi = 2;
		Srbi = 0;
	}
	else
	{
		Flip = 1;
		Frbi = 0;
		Srbi = 2;
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
	Frames[(int)FrameSelect::PREVIOUS] = Frames[(int)FrameSelect::CURRENT];

	auto frame = Frames[static_cast<int>(FrameSelect::CURRENT)] = std::make_shared<Frame>(data, packet_size - 8, this);
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

		ctx = LeftBlocks[Tables::B6To4[b]].NotNullDc + AboveBlocks[AboveBlocksIdx[b]].NotNullDc;
		model1 = Model.CoeffDccv[pt];
		model2 = Model.CoeffDcct[pt][ctx];

		coeff_index = 0;
		for (;;)
		{
			if ((coeff_index > 1 && ct == 0) || CoeffDec->GetBitProbabilityBranch(model2[0]))
			{
				//Parse a coefficient
				if (CoeffDec->GetBitProbabilityBranch(model2[2]))
				{
					if (CoeffDec->GetBitProbabilityBranch(model2[3]))
					{
						idx = CoeffDec->GetTree(Tables::PcTree, model1);
						coeff = Tables::CoeffBias[idx + 5];
						for (int i = Tables::CoeffBitLength[idx]; i >= 0; --i)
							coeff += CoeffDec->GetBitProbability(Tables::CoeffParseTable[idx][i]) << i;
					}
					else
					{
						if (CoeffDec->GetBitProbabilityBranch(model2[4]))
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

				idx = Model.CoeffIndexToPos[coeff_index];
				BlockCoeff[b][Tables::Scantable[idx]] = (short)coeff;
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

					model3 = Model.CoeffRunv[coeff_index >= 6];
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
			model1 = model2 = Model.CoeffRact[pt][ct][cg];
		}
		LeftBlocks[Tables::B6To4[b]].NotNullDc = AboveBlocks[AboveBlocksIdx[b]].NotNullDc = !!BlockCoeff[b][0];
	}
}

void vp6::DecodingContext::ParseCoefficientsHuffman(int dequantAc)
{
}

void vp6::DecodingContext::AddPredictorsDc(int ref_frame, int dequantAc)
{
	int idx = Tables::Scantable[0];

	for (int b = 0; b < 6; ++b)
	{
		auto& ab = AboveBlocks[AboveBlocksIdx[b]];
		auto& lb = LeftBlocks[Tables::B6To4[b]];

		int count = 0, dc = 0;

		if (ref_frame == lb.RefFrame)
		{
			dc += lb.DcCoeff;
			count++;
		}
		if (ref_frame == ab.RefFrame)
		{
			dc += ab.DcCoeff;
			count++;
		}
		if (count == 0)
			dc = PrevDc[Tables::B2p[b]][ref_frame];
		else if (count == 2)
			dc /= 2;

		BlockCoeff[b][idx] += (short)dc;
		PrevDc[Tables::B2p[b]][ref_frame] = BlockCoeff[b][idx];
		ab.DcCoeff = BlockCoeff[b][idx];
		ab.RefFrame = ref_frame;
		lb.DcCoeff = BlockCoeff[b][idx];
		lb.RefFrame = ref_frame;
		BlockCoeff[b][idx] *= (short)dequantAc;
	}
}
