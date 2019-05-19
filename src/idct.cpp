#include <vp6/idct.hpp>
#include "util.hpp"

void vp6::IDCT::Calculate(uint8_t* dest, int blockOffset, int stride, short* input, int type)
{
	int A, B, C, D, E, F, G, H;
	int Ad, Bd, Cd, Dd, Ed, Fd, Gd, Hd;
	int Add, Bdd;

	//INVERSE DCT ON ROWS
	for (int row = 0; row < 8; ++row)
	{
		short& x0 = input[row + 0 * 8];
		short& x1 = input[row + 1 * 8];
		short& x2 = input[row + 2 * 8];
		short& x3 = input[row + 3 * 8];
		short& x4 = input[row + 4 * 8];
		short& x5 = input[row + 5 * 8];
		short& x6 = input[row + 6 * 8];
		short& x7 = input[row + 7 * 8];

		//Check non zero values             
		if ((x0 | x1 | x2 | x3 | x4 | x5 | x6 | x7) != 0)
		{
			A = M(xC1S7, x1) + M(xC7S1, x7);
			B = M(xC7S1, x1) - M(xC1S7, x7);
			C = M(xC3S5, x3) + M(xC5S3, x5);
			D = M(xC3S5, x5) - M(xC5S3, x3);

			Ad = M(xC4S4, (A - C));
			Bd = M(xC4S4, (B - D));

			Cd = A + C;
			Dd = B + D;

			E = M(xC4S4, (x0 + x4));
			F = M(xC4S4, (x0 - x4));

			G = M(xC2S6, x2) + M(xC6S2, x6);
			H = M(xC6S2, x2) - M(xC2S6, x6);

			Ed = E - G;
			Gd = E + G;

			Add = F + Ad;
			Bdd = Bd - H;

			Fd = F - Ad;
			Hd = Bd + H;

			x0 = (short)(Gd + Cd);
			x7 = (short)(Gd - Cd);

			x1 = (short)(Add + Hd);
			x2 = (short)(Add - Hd);

			x3 = (short)(Ed + Dd);
			x4 = (short)(Ed - Dd);

			x5 = (short)(Fd + Bdd);
			x6 = (short)(Fd - Bdd);
		}
	}
	//INVERSE DCT ON COLUMNS
	for (int col = 0; col < 8; ++col)
	{
		short x0 = input[col * 8 + 0];
		short x1 = input[col * 8 + 1];
		short x2 = input[col * 8 + 2];
		short x3 = input[col * 8 + 3];
		short x4 = input[col * 8 + 4];
		short x5 = input[col * 8 + 5];
		short x6 = input[col * 8 + 6];
		short x7 = input[col * 8 + 7];

		uint8_t& d0 = dest[blockOffset + col + 0 * stride];
		uint8_t& d1 = dest[blockOffset + col + 1 * stride];
		uint8_t& d2 = dest[blockOffset + col + 2 * stride];
		uint8_t& d3 = dest[blockOffset + col + 3 * stride];
		uint8_t& d4 = dest[blockOffset + col + 4 * stride];
		uint8_t& d5 = dest[blockOffset + col + 5 * stride];
		uint8_t& d6 = dest[blockOffset + col + 6 * stride];
		uint8_t& d7 = dest[blockOffset + col + 7 * stride];

		//Check non zero values
		if ((x1 | x2 | x3 | x4 | x5 | x6 | x7) != 0)
		{
			A = M(xC1S7, x1) + M(xC7S1, x7);
			B = M(xC7S1, x1) - M(xC1S7, x7);
			C = M(xC3S5, x3) + M(xC5S3, x5);
			D = M(xC3S5, x5) - M(xC5S3, x3);

			Ad = M(xC4S4, (A - C));
			Bd = M(xC4S4, (B - D));

			Cd = A + C;
			Dd = B + D;

			E = M(xC4S4, (x0 + x4)) + 8;
			F = M(xC4S4, (x0 - x4)) + 8;

			if (type == 1)
			{ // HACK
				E += 16 * 128;
				F += 16 * 128;
			}

			G = M(xC2S6, x2) + M(xC6S2, x6);
			H = M(xC6S2, x2) - M(xC2S6, x6);

			Ed = E - G;
			Gd = E + G;

			Add = F + Ad;
			Bdd = Bd - H;

			Fd = F - Ad;
			Hd = Bd + H;
			if (type == 1)
			{
				d0 = Util::ClipByte((Gd + Cd) >> 4);
				d7 = Util::ClipByte((Gd - Cd) >> 4);

				d1 = Util::ClipByte((Add + Hd) >> 4);
				d2 = Util::ClipByte((Add - Hd) >> 4);

				d3 = Util::ClipByte((Ed + Dd) >> 4);
				d4 = Util::ClipByte((Ed - Dd) >> 4);

				d5 = Util::ClipByte((Fd + Bdd) >> 4);
				d6 = Util::ClipByte((Fd - Bdd) >> 4);
			}
			else
			{
				d0 = Util::ClipByte(d0 + ((Gd + Cd) >> 4));
				d7 = Util::ClipByte(d7 + ((Gd - Cd) >> 4));

				d1 = Util::ClipByte(d1 + ((Add + Hd) >> 4));
				d2 = Util::ClipByte(d2 + ((Add - Hd) >> 4));

				d3 = Util::ClipByte(d3 + ((Ed + Dd) >> 4));
				d4 = Util::ClipByte(d4 + ((Ed - Dd) >> 4));

				d5 = Util::ClipByte(d5 + ((Fd + Bdd) >> 4));
				d6 = Util::ClipByte(d6 + ((Fd - Bdd) >> 4));
			}
		}
		else
		{
			if (type == 1)
			{
				d0 = d1 = d2 = d3 = d4 = d5 = d6 = d7 =
					Util::ClipByte(128 + ((xC4S4 * x0 + (IdctAdjustBeforeShift << 16)) >> 20));
			}
			else
			{
				if (x0 > 0)
				{
					int v = (xC4S4 * x0 + (IdctAdjustBeforeShift << 16)) >> 20;
					d0 = Util::ClipByte(d0 + v);
					d1 = Util::ClipByte(d1 + v);
					d2 = Util::ClipByte(d2 + v);
					d3 = Util::ClipByte(d3 + v);
					d4 = Util::ClipByte(d4 + v);
					d5 = Util::ClipByte(d5 + v);
					d6 = Util::ClipByte(d6 + v);
					d7 = Util::ClipByte(d7 + v);
				}
			}
		}
	}
}
