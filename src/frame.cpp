#include <vp6/frame.h>
#include <vp6/decode.h>
#include <vp6/rangedecoder.h>
#include "dequantize.h"

#include <exception>

vp6::Frame::Frame(uint8_t* data, int packet_size, DecodingContext* ctx) : m_ctx(ctx)
{
	int index = 0;

	m_type = static_cast<FrameType>((data[index] >> 7) & 0x01);
	m_quantizer = (data[index] >> 1) & 0x3F;

	//Get the quantizer values
	m_dequant_ac = Dequantizer::AC[m_quantizer] << 2;
	m_dequant_dc = Dequantizer::DC[m_quantizer] << 2;

	m_seperateCoeff = data[index] & 0x01;
	++index;

	switch (m_type)
	{
	case FrameType::INTRA:
		ctx->Format = static_cast<Format>(data[index] >> 3);
		ctx->Profile = static_cast<Profile>(data[index] & 0x06);
		++index;

		if (m_seperateCoeff || ctx->Profile == Profile::SIMPLE)
		{
			m_coeffOffset = ((data[index++] << 8) | data[index++]);
		}

		m_vfrags = data[index++];
		m_hfrags = data[index++];

		if (m_vfrags == 0 || m_hfrags == 0)
		{
			return;
		}

		m_dimX = m_hfrags * 16;
		m_dimY = m_vfrags * 16;

		m_ovfrags = data[index++];
		m_ohfrags = data[index++];

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

		ctx->RangeDec = new RangeDecoder(data, index);
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
	auto parseCoeff = ctx->ParseCoefficients;

	if (m_coeffOffset > 0)
	{
		if (m_useHuffman)
		{
			parseCoeff = ctx->ParseCoefficientsHuffman;
			//ctx->BitReader = new BitReader(buf, index);
		}
		else
		{
			ctx->CoeffDec = new RangeDecoder(data, m_coeffOffset);
		}
	}
	else
	{
		ctx->CoeffDec = ctx->RangeDec;
	}
}

void vp6::Frame::Decode()
{

}