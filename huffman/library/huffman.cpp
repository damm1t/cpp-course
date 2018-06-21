#include "huffman.h"
#include <cassert>
#include <set>
#include <algorithm>
#include <nmmintrin.h>

namespace huffman
{
	void write_int_to_byte_array(byte* x, const int value, int& end)
	{
		x[end++] = static_cast<byte>(value & 0x000000ff);
		x[end++] = static_cast<byte>((value & 0x0000ff00) >> 8);
		x[end++] = static_cast<byte>((value & 0x00ff0000) >> 16);
		x[end++] = static_cast<byte>((value & 0xff000000) >> 24);
	}

	int read_int_from_byte_array(const byte* x, const size_t& size, int& end)
	{
		assert(size - end >= 4);
		int res = 0;
		for (int i = 0; i < 4; ++i)
		{
			res += (static_cast<int>(x[end++]) << i);
		}
		return res;
	}

	/** TreeNode CLASS: **/

	bool tree_node::is_leaf() const
	{
		return !left && !right;
	}

	shared_ptr<tree_node> node_merge(shared_ptr<tree_node>& node1, shared_ptr<tree_node>& node2)
	{
		auto result = std::make_shared<tree_node>(tree_node());
		result->freq = node1->freq + node2->freq;
		result->left = node1;
		result->right = node2;
		return result;
	}

	/************************ HuffmanEncoder CLASS: **************************************/

	void HuffmanEncoder::simplify(byte* output, vector<byte>& bite_array, int& end)
	{
		for (int i = 0; i < bite_array.size(); i += 8) // ToDo
		{
			byte val = 0;
			for (int j = i; j < CHAR_BIT + i; ++j)
			{
				const bool include = j < bite_array.size() ? bite_array[j] != 0 : 0;
				val <<= 1;
				val += include;
			}
			output[end++] = val;
		}
	}

	void HuffmanEncoder::append(byte* data, const size_t len)
	{
		if (len == 0)
			build();
		for (size_t i = 0; i < len; ++i)
			++freqs[data[i]];
	}

	void HuffmanEncoder::create_bin_code(const tree_ptr cur)
	{
		if (cur->is_leaf())
			nodes.push_back(cur->symb);
		if (cur->left)
		{
			bin_tree.push_back(1);
			create_bin_code(cur->left);
		}
		if (cur->right)
		{
			bin_tree.push_back(1);
			create_bin_code(cur->right);
		}
		bin_tree.push_back(0);
	}

	void HuffmanEncoder::dfs(const tree_ptr cur, vector<byte>& key)
	{
		if (cur->is_leaf())
			codes[cur->symb] = key;
		if (cur->left)
		{
			key.push_back(0);
			dfs(cur->left, key);
			key.pop_back();
		}
		if (cur->right)
		{
			key.push_back(1);
			dfs(cur->right, key);
			key.pop_back();
		}
	}

	void HuffmanEncoder::build()
	{
		if (freqs.empty()) // empty input
			return;
		std::multiset<tree_ptr, set_comp> groups;
		size_t it_node = 0;
		for (const auto p : freqs)
		{
			const auto node = std::make_shared<tree_node>(tree_node(p.first, p.second));
			groups.insert(node);
		}

		while (groups.size() > 1)
		{
			tree_ptr tree1 = *groups.begin();
			groups.erase(groups.begin());

			tree_ptr tree2 = *groups.begin();
			groups.erase(groups.begin());

			groups.insert(node_merge(tree1, tree2));
		}

		tree = *groups.begin();
		vector<byte> key;
		dfs(tree, key); // generate codes
	}

	inline void HuffmanEncoder::compression(vector<byte>& input, vector<byte>& output)
	{
		const size_t new_size = (input.size() + 7) / 8;
		byte* compress = new byte[new_size];
		int end = 0;
		simplify(compress, input, end);
		for (int j = 0; j < end; ++j)
			output.push_back(compress[j]);
		input.clear();
		delete[] compress;
	}

	void HuffmanEncoder::encode(byte* input, const size_t len, vector<byte>& output)
	{
		output.clear();
		if (len == 0)
		{
			compression(buf, output);
			return;
		}

		for (size_t i = 0; i < len; ++i)
		{
			assert(codes.find(input[i]) != codes.end());
			for (auto val : codes[input[i]])
			{
				buf.push_back(val);
				if (buf.size() == CHAR_BIT * BUFFER)
				{
					compression(buf, output);
				}
			}
		}
	}


	void HuffmanEncoder::write_tree(byte*& output, size_t& size)
	{
		if (bin_tree.empty()) // if not created yet
			create_bin_code(tree);
		//int64_t: 64 / 8 = 8 byte
		const int simp_tree = (bin_tree.size() + 7) / 8;
		size = 4 + nodes.size() + 4 + simp_tree;
		output = new byte[size];
		int end = 0;

		write_int_to_byte_array(output, static_cast<int>(nodes.size()), end);
		for (const auto node : nodes)
			output[end++] = node;

		write_int_to_byte_array(output, simp_tree, end);

		simplify(output, bin_tree, end);
	}


	/************************ HuffmanDecoder CLASS: **************************************/

	void HuffmanDecoder::update(const byte move)
	{
		if (q.empty())
			q.push(tree_);

		for (int i = CHAR_BIT - 1; i >= 0; --i)
			if ((move & (1 << i)) != 0)
				if (!q.top()->left)
				{
					q.top()->left = std::make_shared<tree_node>(tree_node());
					q.push(q.top()->left);
				}
				else
				{
					q.top()->right = std::make_shared<tree_node>(tree_node());
					q.push(q.top()->right);
				}
			else
			{
				if (q.empty())
					break;

				if (q.top()->is_leaf())
					q.top()->symb = nodes[it_nodes++];
				q.pop();
			}
	}

	void HuffmanDecoder::build(const byte* stream, const size_t& size)
	{
		/*while (it_ < size_prev)
		{
			if (cnt_nodes == -1)
				cnt_nodes = read_int_from_byte_array(input_prev, it_);

			else if (cnt_nodes > 0)
				nodes.push_back(input_prev[it_++]), cnt_nodes--;

			else if (cnt_bytes == -1)
				cnt_bytes = read_int_from_byte_array(input_prev, it_);

			else
				update(input_prev[it_++]);
		}*/
		while (it_ < size_prev)
		{
			if (cnt_nodes == -1)
			{
				cnt_nodes = read_int_from_byte_array(stream, size, it_);
			}
			else if (cnt_nodes > 0)
			{
				assert(size - it_ >= 0);
				nodes.push_back(stream[it_++]), cnt_nodes--;
			}

			else if (cnt_bytes == -1)
				cnt_bytes = read_int_from_byte_array(stream, size, it_);

			else
			{
				assert(size - it_ >= 0);
				update(stream[it_++]);
			}
		}
		it_ -= size_prev;
	}

	void HuffmanDecoder::decode_bit(const bool key, vector<byte>& output)
	{
		if (key == false)
			it_tree_ = it_tree_->left;
		else
			it_tree_ = it_tree_->right;
		if (it_tree_->is_leaf())
		{
			output.push_back(it_tree_->symb);
			it_tree_ = tree_;
		}
	}

	void HuffmanDecoder::create(const byte* input, const size_t& size)
	{
		delete[] input_prev;
		size_prev = size;
		input_prev = new byte[size_prev];
		memcpy(input_prev, input, size_prev);
	}


	void HuffmanDecoder::append(byte* input, const size_t size)
	{
		if (size == 0)
		{
			build(input_prev, size_prev);
			return;
		}
		if (size_prev == 0)
		{
			create(input, size);
			size_prev = size;
			return;
		}
		byte* stream = new byte[size + size_prev];
		for (auto i = 0; i < size_prev; ++i)
			stream[i] = input_prev[i];
		for (auto i = 0; i < size; ++i)
			stream[i + size_prev] = input[i];
		build(stream, size + size_prev);
		delete[] stream;
		create(input, size);
	}

	void HuffmanDecoder::decode(byte* input, const size_t size, vector<byte>& output)
	{
		output.clear();
		for (size_t t = 0; t < size; ++t)
		{
			const auto cur = input[t];
			for (int i = CHAR_BIT - 1; i >= 0; --i)
			{
				decode_bit((cur & (1 << i)) != 0, output);
			}
		}
	}

	void HuffmanEncoder::clear()
	{
		tree.reset();
		codes.clear();
		freqs.clear();
	}
}
