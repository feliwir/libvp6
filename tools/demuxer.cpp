#include "demuxer.hpp"

vp6::Demuxer::Demuxer(std::istream&& stream) : m_reader(stream)
{
	auto fourcc = m_reader.Read<uint32_t>();

	if (!(fourcc == AVP6_TAG ||
		fourcc == MVhd_TAG))

	{
		return;
	}

	m_valid = true;
	m_fourcc = fourcc;

	ProcessHeader();
	ReadPacket();
}

bool vp6::Demuxer::ProcessHeader()
{
	auto headersize = m_reader.Read<uint32_t>();
	if (headersize < 8)
		return false;

	if (m_fourcc == AVP6_TAG)
	{
		while (m_alpha == nullptr || m_color == nullptr)
		{
			auto startpos = m_reader.Stream().tellg();
			auto fourcc = m_reader.Read<int32_t>();
			auto blocksize = m_reader.Read<int32_t>();

			if (fourcc == MVhd_TAG)
			{
				ProcessVideoHeader(m_color);
			}
			else if (fourcc == AVhd_TAG)
			{
				ProcessVideoHeader(m_alpha);
			}
			else
			{
				m_reader.Stream().seekg(-8, std::ios::cur);
				break;
			}

			m_reader.Stream().seekg(static_cast<uint64_t>(startpos) + blocksize, std::ios::beg);
		}
	}
	else if (m_fourcc == MVhd_TAG)
	{
		ProcessVideoHeader(m_color);
	}
}

bool vp6::Demuxer::CheckCodec()
{
	auto fourcc = m_reader.Read<uint32_t>();

	return (fourcc == vp60_TAG || fourcc == vp61_TAG);
}

bool vp6::Demuxer::ProcessVideoHeader(std::shared_ptr<DecodingContext>& ctx)
{
	if (!CheckCodec())
		return false;

	auto width = m_reader.Read<uint16_t>();
	auto height = m_reader.Read<uint16_t>();
	auto framecount = m_reader.Read<uint32_t>();
	auto largestFrame = m_reader.Read<uint32_t>();
	auto denominator = m_reader.Read<uint32_t>();
	auto numerator = m_reader.Read<uint32_t>();

	ctx = std::make_shared<DecodingContext>(width, height, denominator, numerator, framecount, true);
}

bool vp6::Demuxer::ReadPacket()
{
	bool read = !m_reader.Stream().eof();
	while (read)
	{
		auto chunk_type = m_reader.Read<int32_t>();
		auto chunk_size = m_reader.Read<int32_t>();
		
		if (chunk_size < 8)
			return false;

		m_buffer.resize(chunk_size);
		m_reader.ReadBuffer(m_buffer);

		//Pass packet to the video stream
		if (chunk_type == MV0K_TAG || chunk_type == MV0F_TAG)
		{
			m_color->ProcessPacket(m_buffer.data(), chunk_size);
		}
		//Pass packet to the alpha stream
		else if (chunk_type == AV0K_TAG || chunk_type == AV0F_TAG)
		{
			m_alpha->ProcessPacket(m_buffer.data(), chunk_size);
		}
		else
		{
			return false;
		}
	}

	return true;
}