#pragma once 
#include <istream>
#include <vector>
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace vp6
{
	class Util
	{
		static inline uint16_t Reverse(const uint16_t num)
		{
			#ifdef _MSC_VER
			return _byteswap_ushort(num);
			#endif
		}

		static inline uint32_t Reverse(const uint32_t num)
		{
			#ifdef _MSC_VER
			return _byteswap_ulong(num);
			#endif
		}

		static inline uint64_t Reverse(const uint64_t num)
		{
			#ifdef _MSC_VER
			return _byteswap_uint64(num);
			#endif
		}
	};

	class BinaryReader
	{
	public:
		BinaryReader(std::istream& stream) : m_stream(stream)
		{
			m_stream.seekg(0, std::ios::beg);
		}

		template<class T>
		inline T Read()
		{
			T result = T();
			m_stream.read((char*)&result, sizeof(T));
			return result;
		}

		inline void ReadBuffer(std::vector<uint8_t>& buffer)
		{
			m_stream.read((char*)buffer.data(), buffer.size());
		}

		template<class T>
		inline T ReadReverse()
		{
			return Util::Reverse(Read<T>());
		}

		inline std::istream& Stream()
		{
			return m_stream;
		}

	private:
		std::istream& m_stream;
	};
}