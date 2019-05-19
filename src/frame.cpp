#include <vp6/frame.hpp>
#include <vp6/decode.hpp>
#include <vp6/rangedecoder.hpp>
#include "dequantize.hpp"
#include "tables.hpp"

#include <exception>
using namespace std::placeholders;

vp6::Frame::Frame(uint8_t* data, int data_size, DecodingContext* ctx) : m_ctx(ctx)
{
	m_type = static_cast<FrameType>((data[0] >> 7) & 0x01);
	m_quantizer = (data[0] >> 1) & 0x3F;

	//Get the quantizer values
	m_dequant_ac = Dequantizer::AC[m_quantizer] << 2;
	m_dequant_dc = Dequantizer::DC[m_quantizer] << 2;

	m_seperateCoeff = data[0] & 0x01;

	bool interlacing = false;

	switch (m_type)
	{
	case FrameType::INTRA:
		ctx->Format = static_cast<Format>(data[1] >> 3);
		ctx->Profile = static_cast<Profile>(data[1] & 0x06);
		interlacing = data[1] & 1;

		if (m_seperateCoeff || ctx->Profile == Profile::SIMPLE)
		{
			m_coeffOffset = ((data[2] << 8) | data[3]);
			data += 2;
			data_size -= 2;
		}

		m_vfrags = data[2];
		m_hfrags = data[3];

		if (m_vfrags == 0 || m_hfrags == 0)
		{
			return;
		}

		m_dimX = m_hfrags * 16;
		m_dimY = m_vfrags * 16;

		m_ovfrags = data[4];
		m_ohfrags = data[5];

		m_presX = m_ohfrags * 16;
		m_presY = m_ovfrags * 16;

		//check if size changed
		if (m_dimX != ctx->Width || m_dimY != ctx->Height || ctx->Macroblocks == nullptr)
		{
			ctx->MbWidth = m_hfrags;
			ctx->MbHeight = m_vfrags;
			ctx->Width = m_dimX;
			ctx->Height = m_dimY;
			//Allocate the Macroblocks
			int length = m_vfrags * m_hfrags;
			ctx->Macroblocks = new Macroblock[length];

			//Allocate the above Blocks
			ctx->AboveBlocks = new Reference[4 * m_hfrags + 6];

			//Set stride & size
			ctx->YStride = ctx->Width;
			ctx->UvStride = ctx->YStride / 2;
			ctx->YSize = ctx->YStride * (ctx->Height);
			ctx->UvSize = ctx->YSize / 4;
		}

		ctx->RangeDec = new RangeDecoder(data + 6, data_size - 6);
		ctx->Scaling = static_cast<ScalingMode>(ctx->RangeDec->ReadBits(2));
		ctx->IsGolden = false;
		break;
	case FrameType::INTER:
	default:
		break;
	}

	if (ctx->Profile == Profile::ADVANCED || ctx->Format == Format::VP62)
	{
		throw new std::exception("Unsupported profile");
	}

	m_useHuffman = ctx->RangeDec->ReadBit();
	m_parseCoeff = std::bind(&DecodingContext::ParseCoefficients, ctx, _1);

	if (m_coeffOffset)
	{
		data += m_coeffOffset;
		data_size -= m_coeffOffset;

		if (m_useHuffman)
		{
			m_parseCoeff = std::bind(&DecodingContext::ParseCoefficientsHuffman, ctx, _1);
			//ctx->BitReader = new BitReader(buf, index);
		}
		else
		{
			ctx->CoeffDec = new RangeDecoder(data, data_size);
		}
	}
	else
	{
		ctx->CoeffDec = ctx->RangeDec;
	}

	Planes.push_back(new uint8_t[m_ctx->YSize]);
	Planes.push_back(new uint8_t[m_ctx->UvSize]);
	Planes.push_back(new uint8_t[m_ctx->UvSize]);

	Strides.push_back(m_ctx->Flip * m_ctx->YStride);
	Strides.push_back(m_ctx->Flip * m_ctx->UvStride);
	Strides.push_back(m_ctx->Flip * m_ctx->UvStride);
}

vp6::Frame::~Frame()
{
	for (auto& plane : Planes)
	{
		delete[] plane;
	}
}

void vp6::Frame::Decode()
{
	int mb_row_flip = -1;
	int mb_offset = 0;

	if (m_type == FrameType::INTRA)
	{
		m_ctx->Model.Reset();
		//All macroblocks are intra frames
		for (int b = 0; b < m_ctx->MbHeight * m_ctx->MbWidth; b++)
		{
			m_ctx->Macroblocks[b].Type = CodingMode::INTRA;
		}
	}
	else
	{
		m_ctx->MacroblockType = CodingMode::INTER_MV;
	}

	ParseCoeffModels();

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			m_ctx->PrevDc[i][j] = 0;

	m_ctx->PrevDc[1][static_cast<int>(FrameSelect::CURRENT)] = 128;
	m_ctx->PrevDc[2][static_cast<int>(FrameSelect::CURRENT)] = 128;

	for (int block = 0; block < 4 * m_ctx->MbWidth + 6; block++)
	{
		m_ctx->AboveBlocks[block].RefFrame = static_cast<int>(FrameSelect::NONE);
		m_ctx->AboveBlocks[block].DcCoeff = 0;
		m_ctx->AboveBlocks[block].NotNullDc = 0;
	}

	m_ctx->AboveBlocks[2 * m_ctx->MbWidth + 2].RefFrame = static_cast<int>(FrameSelect::CURRENT);
	m_ctx->AboveBlocks[3 * m_ctx->MbWidth + 4].RefFrame = static_cast<int>(FrameSelect::CURRENT);

	if (m_ctx->Flip < 0)
		mb_offset = 7;

	//The loop for decoding each Macroblock
	for (int row = 0; row < m_ctx->MbHeight; ++row)
	{
		if (m_ctx->Flip < 0)
			mb_row_flip = (int)m_ctx->MbHeight - row - 1;
		else
			mb_row_flip = row;

		for (int block = 0; block < 4; ++block)
		{
			m_ctx->LeftBlocks[block].RefFrame = static_cast<int>(FrameSelect::NONE);
			m_ctx->LeftBlocks[block].DcCoeff = 0;
			m_ctx->LeftBlocks[block].NotNullDc = 0;
		}

		m_ctx->AboveBlocksIdx[0] = 1;
		m_ctx->AboveBlocksIdx[1] = 2;
		m_ctx->AboveBlocksIdx[2] = 1;
		m_ctx->AboveBlocksIdx[3] = 2;
		m_ctx->AboveBlocksIdx[4] = 2 * (int)m_ctx->MbWidth + 2 + 1;
		m_ctx->AboveBlocksIdx[5] = 3 * (int)m_ctx->MbWidth + 4 + 1;

		//calculate the pixeloffset for each block
		m_ctx->BlockOffset[m_ctx->Frbi] = (int)((mb_row_flip * 16 + mb_offset) * m_ctx->YStride);
		m_ctx->BlockOffset[m_ctx->Srbi] = (int)(m_ctx->BlockOffset[m_ctx->Frbi] + 8 * m_ctx->YStride);
		m_ctx->BlockOffset[1] = m_ctx->BlockOffset[0] + 8;
		m_ctx->BlockOffset[3] = m_ctx->BlockOffset[2] + 8;
		m_ctx->BlockOffset[4] = (int)((mb_row_flip * 8 + mb_offset) * m_ctx->UvStride);
		m_ctx->BlockOffset[5] = m_ctx->BlockOffset[4];

		for (int column = 0; column < m_ctx->MbWidth; ++column)
		{
			DecodeMacroblock(row, column);

			for (int y = 0; y < 4; y++)
			{
				m_ctx->AboveBlocksIdx[y] += 2;
				m_ctx->BlockOffset[y] += 16;
			}

			for (int uv = 4; uv < 6; uv++)
			{
				m_ctx->AboveBlocksIdx[uv] += 1;
				m_ctx->BlockOffset[uv] += 8;
			}
		}
	}
}

void vp6::Frame::ParseCoeffModels()
{
	int DefaultProb[11];
	int cg, ctx, pos;
	int ct;    /* code type */
	auto rangeDec = m_ctx->RangeDec;
	auto& model = m_ctx->Model;

	std::memset(DefaultProb, 0x80, sizeof(DefaultProb));

	/* plane type (0 for Y, 1 for U or V) */
	for (int pt = 0; pt < 2; pt++)
		for (int node = 0; node < 11; node++)
			if (rangeDec->GetBitProbability(Tables::DccvPct[pt][node]))
			{
				DefaultProb[node] = rangeDec->ReadBitsNn(7);
				model.CoeffDccv[pt][node] = DefaultProb[node];
			}
			else if (m_type == FrameType::INTRA) {
				model.CoeffDccv[pt][node] = DefaultProb[node];
			}

	if (rangeDec->ReadBit())
	{
		for (int pos = 1; pos < 64; ++pos)
		{
			if (rangeDec->GetBitProbabilityBranch(Tables::CoeffReorderPct[pos]))
			{
				model.CoeffReorder[pos] = rangeDec->ReadBits(4);
			}
		}

		model.InitializeCoeffOrderTable();
	}

	for (int cg = 0; cg < 2; ++cg)
	{
		for (int node = 0; node < 14; ++node)
		{
			if (rangeDec->GetBitProbabilityBranch(Tables::RunvPct[cg][node]))
			{
				model.CoeffRunv[cg][node] = rangeDec->ReadBitsNn(7);
			}
		}
	}

	for (int ct = 0; ct < 3; ct++)
	{
		for (int pt = 0; pt < 2; pt++)
		{
			for (int cg = 0; cg < 6; cg++)
			{
				for (int node = 0; node < 11; node++)
				{
					if (rangeDec->GetBitProbabilityBranch(Tables::RactPct[ct][pt][cg][node]))
					{
						DefaultProb[node] = rangeDec->ReadBitsNn(7);
						model.CoeffRact[pt][ct][cg][node] = DefaultProb[node];
					}
					else if (m_type == FrameType::INTRA)
					{
						model.CoeffRact[pt][ct][cg][node] = DefaultProb[node];
					}
				}
			}
		}
	}

	if (m_useHuffman)
	{
		for (int pt = 0; pt < 2; pt++)
		{
			uint8_t* dccv = model.CoeffDccv[pt];
			m_ctx->HuffDccv[pt] = std::make_shared<Huffman>(dccv, Tables::HuffCoeffMap, 12);

			uint8_t* runv = model.CoeffRunv[pt];
			m_ctx->HuffRunv[pt] = std::make_shared<Huffman>(runv, Tables::HuffRunMap, 9);

			for (int ct = 0; ct < 3; ct++)
				for (int cg = 0; cg < 6; cg++)
				{
					uint8_t* ract = model.CoeffRact[pt][ct][cg];
					m_ctx->HuffRact[pt][ct][cg] = std::make_shared<Huffman>(ract, Tables::HuffCoeffMap, 12);
				}
		}
	}
	else
	{
		//Calculate DCCT
		for (int pt = 0; pt < 2; pt++)
			for (int ctx = 0; ctx < 3; ctx++)
				for (int node = 0; node < 5; node++)
					model.CoeffDcct[pt][ctx][node] = std::clamp(((model.CoeffDccv[pt][node] * Tables::DccvLc[ctx][node][0] + 128) >> 8) + Tables::DccvLc[ctx][node][1], 1, 255);
	}

	
}

void vp6::Frame::DecodeMacroblock(int row, int column)
{
	CodingMode mode;

	if (m_type == FrameType::INTRA)
	{
		mode = CodingMode::INTRA;
	}
	else
	{
		mode = DecodeMotionvector(row, column);
	}

	m_parseCoeff(m_dequant_ac);

	//TODO: work here
	RenderMacroblock(mode);
}

int vp6::Frame::GetVectorPredictors(int row, int column, FrameSelect ref_frame)
{
	return 0;
}

void vp6::Frame::ParseVectorAdjustment(Motionvector& vect)
{
	auto rd = m_ctx->RangeDec;
	vect = { 0, 0 };
	if (m_ctx->VectorCandidatePos < 2)
		vect = m_ctx->VectorCandidate[0];

	for (int comp = 0; comp < 2; ++comp)
	{
		int delta = 0;

		if (rd->GetBitProbabilityBranch(m_ctx->Model.VectorDct[comp]) > 0)
		{
			const std::array<uint8_t, 7> prob_order = { 0, 1, 2, 7, 6, 5, 4 };
			for (int i = 0; i < prob_order.size(); ++i)
			{
				int j = prob_order[i];
				delta |= rd->GetBitProbability(m_ctx->Model.VectorFdv[comp][j]) << j;
			}
			if ((delta & 0xF0) > 0)
				delta |= rd->GetBitProbability(m_ctx->Model.VectorFdv[comp][3]) << 3;
			else
				delta |= 8;
		}
		else
		{
			uint8_t* slice = m_ctx->Model.VectorPdv[comp];
			delta = rd->GetTree(Tables::PvaTree, slice);
		}

		if (!!delta && rd->GetBitProbabilityBranch(m_ctx->Model.VectorSig[comp]) > 0)
			delta = -delta;

		if (comp <= 0)
			vect.X += delta;
		else
			vect.Y += delta;
	}
}

void vp6::Frame::RenderMacroblock(CodingMode mode)
{
	int ab, b_max, b;
	int plane;
	int ref_frame = (int)Tables::ReferenceFrame[(int)mode];

	m_ctx->AddPredictorsDc(ref_frame, m_dequant_dc);

	auto frame_ref = m_ctx->Frames[ref_frame];

	//TODO: add profile for alpha
	ab = 6;
	b_max = 6;

	switch (mode)
	{
	case CodingMode::INTRA:
		for (b = 0; b < b_max; ++b)
		{
			int16_t* slice = m_ctx->BlockCoeff[b];
			plane = Tables::B2p[b];
			m_ctx->Idct.Put(Planes[plane], m_ctx->BlockOffset[b], Strides[plane], slice);
		}
		break;
	default:
		throw new std::exception("This macroblock type is not supported!");
		break;
	}

}

vp6::CodingMode vp6::Frame::DecodeMotionvector(int row, int column)
{
	vp6::Motionvector mv, vector{0,0};
	int vp = GetVectorPredictors(row, column, FrameSelect::PREVIOUS);

	m_ctx->MacroblockType = ParseMacroblockType(vp);
	m_ctx->Macroblocks[row * m_ctx->MbWidth + column].Type = m_ctx->MacroblockType;

	switch (m_ctx->MacroblockType)
	{
	case CodingMode::INTER_NEAREST_MV:
		mv = m_ctx->VectorCandidate[0];
		break;
	case CodingMode::INTER_NEAR_MV:
		mv = m_ctx->VectorCandidate[1];
		break;
	case CodingMode::GOLD_NEAREST_MV:
		GetVectorPredictors(row, column, FrameSelect::GOLDEN);
		mv = m_ctx->VectorCandidate[0];
		break;
	case CodingMode::GOLD_NEAR_MV:
		GetVectorPredictors(row, column, FrameSelect::GOLDEN);
		mv = m_ctx->VectorCandidate[1];
		break;
	case CodingMode::INTER_PLUS_MV:
		ParseVectorAdjustment(vector);
		mv = vector;
		break;
	case CodingMode::GOLDEN_MV:
		GetVectorPredictors(row, column, FrameSelect::GOLDEN);
		ParseVectorAdjustment(vector);
		mv = vector;
		break;
	case CodingMode::INTER_FOURMV:
		throw new std::exception("Not done FourMV yet");
		break;
	}

	m_ctx->Macroblocks[row * m_ctx->MbWidth + column].Mv = mv;
	for (int b = 0; b < 6; ++b)
		m_ctx->Mvs[b] = mv;

	return m_ctx->MacroblockType;
}

vp6::CodingMode vp6::Frame::ParseMacroblockType(int vt)
{
	uint8_t* mb_type_model = m_ctx->Model.MacroblockType[vt][(int)m_ctx->MacroblockType];
	auto rd = m_ctx->RangeDec;
	if (rd->GetBitProbabilityBranch(mb_type_model[0]) > 0)
	{
		return m_ctx->MacroblockType;
	}
	else
	{
		return (CodingMode)rd->GetTree(Tables::PmbtTree, mb_type_model);
	}
}
