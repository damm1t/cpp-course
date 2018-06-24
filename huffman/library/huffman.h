#ifndef HUFFMAN_H
#define HUFFMAN_H


#include <memory>
#include <cstddef>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include <bitset>
#include <stack>

using std::vector;
using std::map;
using std::pair;
using std::string;
using std::shared_ptr;
using std::stack;

namespace huffman
{
	const size_t BUFFER = 5;

	typedef unsigned char byte;
	void write_int_to_byte_array(byte* x, const int value, int& end);
	int read_int_from_byte_array(byte* x, const size_t& size, int& end);

	class tree_node
	{
	public:
		byte symb;
		size_t freq;
		bool is_leaf() const;
		shared_ptr<tree_node> left, right;

		tree_node() : symb(0), freq(0)
		{
		};
		tree_node(tree_node const&) = default;

		tree_node(const byte symb, const size_t freq) : symb(symb), freq(freq)
		{
		};
		friend shared_ptr<tree_node> node_merge(shared_ptr<tree_node>& node1,
		                                        shared_ptr<tree_node>& node2);
	};

	shared_ptr<tree_node> node_merge(shared_ptr<tree_node>& node1, shared_ptr<tree_node>& node2);

	typedef shared_ptr<tree_node> tree_ptr;

	struct set_comp
	{
		bool operator()(const tree_ptr& lhs, const tree_ptr& rhs) const
		{
			return lhs->freq < rhs->freq;
		}
	};

	class HuffmanDecoder
		//в конструкторе получает дерево хаффмана в виде (byte*)
		// и восстанавливает его
		// decode
	{
	private:
		int cnt_bytes = -1;
		int cnt_nodes = -1;
		vector<byte> nodes;
		int it_ = 0;
		byte* input_prev = nullptr;
		size_t size_prev = 0;
		stack<tree_ptr> q;
		int it_nodes = 0;
		tree_ptr tree_ = std::make_shared<tree_node>(tree_node());
		tree_ptr it_tree_ = tree_;
	private:
		void update(byte move);
		void build(const byte* stream, const size_t& size);
		void decode_bit(bool key, vector<byte>& output);
		void create(const byte* input, const size_t& size);
	public:
		void append(byte* input, size_t size); // size = 0 equals build
		void decode(byte* input, size_t size, vector<byte>& output);

		~HuffmanDecoder()
		{
			delete[] input_prev;
		}
	};

	class HuffmanEncoder
	{
		// строит дерево
		//получает n chunk-ов
		//на самом последнем -> готовое дерево
		//encode
		//получает chunk возвращет закодированный chunk
		//должен уметь предоставить дерево хаффмана (byte*)
	public:
		void encode(byte* input, size_t len, vector<byte>& output);
		void write_tree(byte*& output, size_t& size); // convert tree to binafy form
		void append(byte* data, size_t len); // add symbols

	private:
		tree_ptr tree;
		map<byte, size_t> freqs;
		map<byte, vector<byte>> codes;
		vector<byte> bin_tree, nodes;
		vector<byte> buf;
	private:
		void simplify(byte* output, vector<byte>& bite_array, int& end);
		void create_bin_code(tree_ptr cur);
		void dfs(tree_ptr cur, vector<byte>& key);
		void build();
		void compression(vector<byte>& input, vector<byte>& output);
		void clear();
	};
}


#endif
