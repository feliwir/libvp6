#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>

namespace vp6
{
	struct Node
	{
		int Probability;
		int Symbol;
		Node* Left;
		Node* Right;
	};

	class Huffman
	{
	public:
		Huffman(const uint8_t* model, const uint8_t* map, int size)
		{
			int a, b;
			m_bits = log2(size) + 1;
			m_nodes.resize(2 * size);

			//Convert to Huffman Probabilities
			m_nodes[size].Probability = 256;

			for (int i = 0; i < size - 1; i++)
			{
				a = m_nodes[size + i].Probability * model[i] >> 8;
				b = m_nodes[size + i].Probability * (255 - model[i]) >> 8;
				m_nodes[map[2 * i]].Probability = a + ((a == 0) ? 1 : 0);
				m_nodes[map[2 * i + 1]].Probability = b + ((b == 0) ? 1 : 0);
				m_nodes[i].Symbol = i;
			}

			m_nodes[size - 1].Symbol = size - 1;

			//Sort depending on probabilities
			std::sort(m_nodes.begin(), m_nodes.end(), [](Node& a, Node& b)
			{
				return (a.Probability - b.Probability) * 16 + (b.Symbol - a.Symbol);
			});

			//Always pack 2 together
			int cur_node = size, j;
			m_nodes.back().Probability = 0;

			for (int i = 0; i < size * 2 - 1; i += 2)
			{
				int cur_count = m_nodes[i].Probability + m_nodes[i + 1].Probability;
				// find correct place to insert new node, and
				// make space for the new node while at it
				for (j = cur_node; j > i + 2; j--)
				{
					if (cur_count > m_nodes[j - 1].Probability)
						break;
					m_nodes[j] = m_nodes[j - 1];
				}
				m_nodes[j].Symbol = -1;
				m_nodes[j].Probability = cur_count;
				cur_node++;
			}
		}

	private:
		std::vector<Node> m_nodes;
		int m_bits;
		Node m_root;
	};
}