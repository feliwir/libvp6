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
  public:
    static inline uint16_t Reverse(const uint16_t num)
    {
#ifdef _MSC_VER
        return _byteswap_ushort(num);
#else
        return __builtin_bswap16(num);
#endif
    }

    static inline uint32_t Reverse(const uint32_t num)
    {
#ifdef _MSC_VER
        return _byteswap_ulong(num);
#else
        return __builtin_bswap32(num);
#endif
    }

    static inline uint64_t Reverse(const uint64_t num)
    {
#ifdef _MSC_VER
        return _byteswap_uint64(num);
#else
        return __builtin_bswap64(num);
#endif
    }

    static inline std::vector<uint8_t> Yuv420pToRgb(std::vector<uint8_t *> planes, int width, int height)
    {
        std::vector<uint8_t> buffer;
        uint32_t size = width * height;

        uint8_t *ybuf = planes[0];
        uint8_t *ubuf = planes[1];
        uint8_t *vbuf = planes[2];

        // Allocate the RGB buffer
        buffer.resize(size * 3);

        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                // get YUV values
                auto &y = ybuf[row * width + col];
                auto &u = ubuf[(row / 2) * (width / 2) + col / 2];
                auto &v = vbuf[(row / 2) * (width / 2) + col / 2];

                // get RGB values
                auto &r = buffer[(row * width + col) * 3];
                auto &g = buffer[(row * width + col) * 3 + 1];
                auto &b = buffer[(row * width + col) * 3 + 2];

                // perform the conversion
                b = (1.164 * (y - 16) + 2.018 * (u - 128));
                g = (1.164 * (y - 16) - 0.813 * (v - 128) - 0.391 * (u - 128));
                r = (1.164 * (y - 16) + 1.596 * (v - 128));
            }
        }

        return buffer;
    }
};

class BinaryReader
{
  public:
    BinaryReader(std::istream &stream) : m_stream(stream)
    {
        m_stream.seekg(0, std::ios::beg);
    }

    template <class T> inline T Read()
    {
        T result = T();
        m_stream.read((char *)&result, sizeof(T));
        return result;
    }

    inline void ReadBuffer(std::vector<uint8_t> &buffer)
    {
        m_stream.read((char *)buffer.data(), buffer.size());
    }

    template <class T> inline T ReadReverse()
    {
        return Util::Reverse(Read<T>());
    }

    inline std::istream &Stream()
    {
        return m_stream;
    }

  private:
    std::istream &m_stream;
};
} // namespace vp6