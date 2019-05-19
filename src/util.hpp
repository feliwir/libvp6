#pragma once 
#include <array>

namespace vp6
{
    class Util
    {
        public:
        template<class T>
        static inline uint8_t ClipByte(T input)
        {
            if (input < 0)
                return 0;
            else if (input > 255)
                return 255;
            else
                return (uint8_t)input;
        }

		template<class T, size_t  N>
		static constexpr const std::array<T, N> Transpose(std::array<T, N> arr)
		{
			std::array<T, N> result{};

			for (int i = 0; i < N; ++i)
			{
				uint8_t x = arr[i];
				result[i] = (uint8_t)(((x) >> 3) | (((x) & 7) << 3));
			}

			return result;
		}
    };
}