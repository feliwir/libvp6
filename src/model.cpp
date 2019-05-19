#include <vp6/model.hpp>

constexpr uint8_t DefaultMbTypesStats[3][10][2] = {
	{ {  69, 42 }, {   1,  2 }, {  1,   7 }, {  44, 42 }, {  6, 22 },
	  {   1,  3 }, {   0,  2 }, {  1,   5 }, {   0,  1 }, {  0,  0 }, },
	{ { 229,  8 }, {   1,  1 }, {  0,   8 }, {   0,  0 }, {  0,  0 },
	  {   1,  2 }, {   0,  1 }, {  0,   0 }, {   1,  1 }, {  0,  0 }, },
	{ { 122, 35 }, {   1,  1 }, {  1,   6 }, {  46, 34 }, {  0,  0 },
	  {   1,  2 }, {   0,  1 }, {  0,   1 }, {   1,  1 }, {  0,  0 }, },
};

constexpr uint8_t DefaultVectorFdv[2][8] = {
	{ 247, 210, 135, 68, 138, 220, 239, 246 },
	{ 244, 184, 201, 44, 173, 221, 239, 253 },
};

constexpr uint8_t DefaultVectorPdv[2][7] = {
	{ 225, 146, 172, 147, 214,  39, 156 },
	{ 204, 170, 119, 235, 140, 230, 228 },
};

//Default Coeff Reorder
constexpr uint8_t DefaultCoeffReorder[64] = {
	0,  0,  1,  1,  1,  2,  2,  2,
	2,  2,  2,  3,  3,  4,  4,  4,
	5,  5,  5,  5,  6,  6,  7,  7,
	7,  7,  7,  8,  8,  9,  9,  9,
	9,  9,  9, 10, 10, 11, 11, 11,
	11, 11, 11, 12, 12, 12, 12, 12,
	12, 13, 13, 13, 13, 13, 14, 14,
	14, 14, 15, 15, 15, 15, 15, 15,
};

//Default Coeff Runv Reorder
constexpr uint8_t DefaultCoeffRunv[2][14] = {
	{ 198, 197, 196, 146, 198, 204, 169, 142, 130, 136, 149, 149, 191, 249 },
	{ 135, 201, 181, 154,  98, 117, 132, 126, 146, 169, 184, 240, 246, 254 },
};

constexpr uint8_t DefaultVectorDct[] = { 0xA2, 0xA4 };
constexpr uint8_t DefaultVectorSig[] = { 0x80, 0x80 };

vp6::Model::Model()
{
	Reset();
}

void vp6::Model::Reset()
{
	//Vectors
	std::memcpy(VectorDct,DefaultVectorDct,sizeof(DefaultVectorDct));
	std::memcpy(VectorSig, DefaultVectorSig, sizeof(DefaultVectorSig));
	std::memcpy(VectorFdv, DefaultVectorFdv, sizeof(DefaultVectorFdv));
	std::memcpy(VectorPdv, DefaultVectorPdv, sizeof(DefaultVectorPdv));

	//Coefficients
	std::memcpy(CoeffRunv, DefaultCoeffRunv, sizeof(DefaultCoeffRunv));
	std::memcpy(CoeffReorder, DefaultCoeffReorder, sizeof(DefaultCoeffReorder));

	//Macroblocks
	std::memcpy(m_mbTypesStats, DefaultMbTypesStats, sizeof(DefaultMbTypesStats));

	InitializeCoeffOrderTable();
}