#include <vp6/rangedecoder.h>
#include "tables.h"

vp6::RangeDecoder::RangeDecoder(uint8_t* buffer, int startpos, int size)
{
	m_index = 0;
	m_high = 255;
	m_bits = -16;
	m_size = size;
	m_startpos = startpos;
	m_buffer = buffer;
	m_codeword = (m_buffer[m_startpos + m_index++] << 16);
	m_codeword |= (m_buffer[m_startpos + m_index++] << 8);
	m_codeword |= (m_buffer[m_startpos + m_index++]);
}

int vp6::RangeDecoder::ReadBitsNn(int bits)
{
	int v = ReadBits(bits) << 1;
	return v + !!v;
}

int vp6::RangeDecoder::ReadBits(int bits)
{
	int value = 0;

	while (bits > 0)
	{
		value = (value << 1) | ReadBit();
		--bits;
	}

	return value;
}

int vp6::RangeDecoder::ReadBit()
{
	uint32_t codeword = Renormalize();
	int low = (m_high + 1) >> 1;
	uint32_t low_shift = low << 16;

	bool bit = codeword >= low_shift;
	if (bit)
	{
		m_high -= (uint8_t)low;
		codeword -= low_shift;
	}
	else
	{
		m_high = (uint8_t)low;
	}

	m_codeword = codeword;
	return bit;
}

int vp6::RangeDecoder::GetBitProbabilityBranch(int prob)
{
	uint32_t codeword = Renormalize();
	uint32_t low = (uint32_t)(1 + (((m_high - 1) * prob) >> 8));
	uint32_t low_shift = low << 16;

	if (codeword >= low_shift)
	{
		m_high -= (int)low;
		m_codeword = codeword - low_shift;
		return 1;
	}

	m_codeword = codeword;
	m_high = (int)low;
	return 0;
}

int vp6::RangeDecoder::GetBitProbability(int prob)
{
	uint32_t codeword = Renormalize();
	uint32_t low = (uint32_t)(1 + (((m_high - 1) * prob) >> 8));
	uint32_t low_shift = low << 16;
	int bit = codeword >= low_shift;

	m_high = (int)(bit > 0 ? m_high - low : low);
	m_codeword = bit > 0 ? codeword - low_shift : codeword;

	return bit;
}

unsigned int vp6::RangeDecoder::Renormalize()
{
	int shift = TablesNormShift::[m_high];
	int bits = m_bits;
	uint32_t codeword = m_codeword;
	uint32_t tmp = 0;
	m_high <<= shift;
	codeword <<= shift;
	bits += shift;

	if (bits >= 0 && (m_startpos + m_index) + 1 < m_buffer.Length)
	{
		tmp |= (uint32_t)(m_buffer[m_startpos + m_index++] << 8);
		tmp |= (uint32_t)(m_buffer[m_startpos + m_index++]);
		codeword |= tmp << bits;
		bits -= 16;
	}
	m_bits = bits;
	return codeword;
}
