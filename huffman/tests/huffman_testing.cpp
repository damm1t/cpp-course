#include <library/huffman.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>
#include <vector>

using namespace huffman;

const string filename_input = "input.txt", filename_output = "output.txt", filename_hf = "temp.txt";
const size_t TEST_BUF = 1024;
byte* huf_tree;
size_t huf_size;
byte* input;
size_t input_size;
byte* encode;
size_t encode_size;
byte* output;
size_t output_size;
size_t main_size;

void append(byte* arr, size_t& size, const byte* add, const size_t& add_size)
{
	for (size_t i = 0; i < add_size; ++i)
	{
		*(arr + size + i) = add[i];
	}
	size += add_size;
}

void compress(string filename_in, string filename_out, string filename_hf)
{
	std::ifstream fin(filename_in.c_str(), std::ios_base::binary);
	std::ofstream fhf(filename_hf.c_str(), std::ios_base::binary);
	char* input_chunk = new char[BUFFER];
	byte* chunk;
	HuffmanEncoder encoder;
	size_t main_size = 0;
	for (;;)
	{
		fin.read(input_chunk, BUFFER);
		chunk = reinterpret_cast<byte*>(input_chunk);
		const auto size = fin ? BUFFER : fin.gcount();
		main_size += size;
		encoder.append(chunk, size);
		if (!fin)
		{
			encoder.append(chunk, 0);
			break;
		}
	}
	vector<byte> output;
	byte* out = nullptr;
	size_t out_size;
	encoder.write_tree(out, out_size);
	//fhf.write(reinterpret_cast<char*>(&out_size), sizeof out_size);
	fhf.write(reinterpret_cast<const char*>(&out[0]), out_size);
	delete[] out;
	fhf.close();


	std::ofstream fout(filename_out.c_str(), std::ios_base::binary);
	fin.clear();
	fin.seekg(0);
	fout.write(reinterpret_cast<char*>(&main_size), sizeof size_t);
	for (;;)
	{
		fin.read(input_chunk, BUFFER);
		const auto size = fin ? BUFFER : fin.gcount();
		chunk = reinterpret_cast<byte*>(input_chunk);
		encoder.encode(chunk, size, output);
		if (!output.empty())
		{
			fout.write(reinterpret_cast<const char*>(&output[0]), output.size());
		}
		if (!fin)
		{
			encoder.encode(chunk, 0, output);
			if (!output.empty())
				fout.write(reinterpret_cast<const char*>(&output[0]), output.size());
			break;
		}
	}

	fin.close();
	fout.close();
	delete[] input_chunk;
}

void decompress(string filename_in, string filename_out, string filename_hf)
{
	std::ifstream fhf(filename_hf.c_str(), std::ios_base::binary);
	char* input_chunk = new char[BUFFER];
	byte* chunk;
	HuffmanDecoder dencoder;
	for (;;) // read & build tree
	{
		fhf.read(input_chunk, BUFFER);
		chunk = reinterpret_cast<byte*>(input_chunk);
		const auto size = fhf ? BUFFER : fhf.gcount();
		dencoder.append(chunk, size);
		if (!fhf)
		{
			dencoder.append(chunk, 0); // build tree
			break;
		}
	}
	fhf.close();
	std::ifstream fin(filename_in.c_str(), std::ios_base::binary);
	vector<byte> output;

	std::ofstream fout(filename_out.c_str(), std::ios_base::binary);
	size_t main_size = 0;
	fin.read(reinterpret_cast<char*>(&main_size), sizeof size_t);
	for (;;)
	{
		fin.read(input_chunk, BUFFER);
		const auto size = fin ? BUFFER : fin.gcount();
		chunk = reinterpret_cast<byte*>(input_chunk);
		dencoder.decode(chunk, size, output);
		const size_t out_size = std::min(main_size, output.size());
		if (!output.empty())
		{
			fout.write(reinterpret_cast<const char*>(&output[0]), out_size);
			main_size -= out_size;
		}
		if (!fin || !out_size)
			break;
	}
	delete[] input_chunk;
	fin.close();
	fout.close();
}

void run_encode()
{
	byte* chunk = new byte[BUFFER];
	HuffmanEncoder encoder;
	main_size = 0;
	size_t stream_it = 0;
	for (;;)
	{
		size_t size = 0;

		while (size < BUFFER && stream_it < input_size)
		{
			chunk[size++] = input[stream_it++];
		}
		main_size += size;
		encoder.append(chunk, size);
		if (stream_it == input_size)
		{
			encoder.append(chunk, 0);
			break;
		}
	}
	encoder.write_tree(huf_tree, huf_size);

	vector<byte> output;
	stream_it = 0;
	encode_size = 0;
	for (;;)
	{
		size_t size = 0;
		while (size < BUFFER && stream_it < input_size)
			chunk[size++] = input[stream_it++];
		encoder.encode(chunk, size, output);
		if (!output.empty())
		{
			append(encode, encode_size, output.data(), output.size());
		}
		if (stream_it == input_size)
		{
			encoder.encode(chunk, 0, output);
			if (!output.empty())
			{
				append(encode, encode_size, output.data(), output.size());
			}
			break;
		}
	}
	delete[] chunk;
}

void run_decode()
{
	byte* chunk = new byte[BUFFER];
	HuffmanDecoder dencoder;
	size_t stream_it = 0;
	for (;;)
	{
		size_t size = 0;

		while (size < BUFFER && stream_it < huf_size)
		{
			chunk[size++] = huf_tree[stream_it++];
		}
		dencoder.append(chunk, size);
		if (stream_it == huf_size)
		{
			dencoder.append(chunk, 0); // build tree
			break;
		}
	}

	vector<byte> out;

	stream_it = 0;
	output_size = 0;
	for (;;)
	{
		size_t size = 0;

		while (size < BUFFER && stream_it < encode_size)
		{
			chunk[size++] = encode[stream_it++];
		}
		dencoder.decode(chunk, size, out);
		const size_t out_size = std::min(main_size, out.size());
		if (!out.empty())
		{
			append(output, output_size, out.data(), out_size);
			main_size -= out_size;
		}
		if (stream_it == encode_size || !out_size)
			break;
	}
	delete[] chunk;
}


string run_encode_decode(string& s)
{
	huf_size = 0;
	input_size = 0;
	encode = new byte[TEST_BUF];
	encode_size = 0;
	output = new byte[TEST_BUF];
	output_size = 0;
	main_size = 0;
	assert(s.length() < TEST_BUF);
	char* c_inp = new char[TEST_BUF];
	strcpy(c_inp, s.c_str());
	input = reinterpret_cast<byte*>(c_inp);
	input_size = s.length();
	run_encode();
	run_decode();
	char* c_out = reinterpret_cast<char*>(output);
	string out = "";
	for(size_t i = 0; i < output_size; ++i)
	{
		out += c_out[i];
	}
	delete[] c_inp;
	delete[] huf_tree;
	delete[] encode;
	delete[] output;
	return out;
}

TEST(encode_decode, empty)
{
	string test = "";
	string decode = run_encode_decode(test);
	ASSERT_EQ(test, decode);
}

TEST(encode_decode, a)
{
	string test = "a";
	string decode = run_encode_decode(test);
	ASSERT_EQ(test, decode);
}

TEST(encode_decode, multiple_a)
{
	string test = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	string decode = run_encode_decode(test);
	ASSERT_EQ(test, decode);
}

TEST(encode_decode, ab)
{
	string test = "ab";
	string decode = run_encode_decode(test);
ASSERT_EQ(test, decode);
}

TEST(encode_decode, alphabet)
{
	string test = "abcdefghijklmnopqrstuvwxyz";
	string decode = run_encode_decode(test);
ASSERT_EQ(test, decode);
}

TEST(encode_decode, capital_alphabet)
{
	string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	string decode = run_encode_decode(test);
ASSERT_EQ(test, decode);
}

TEST(encode_decode, random)
{
	for (size_t i = 0; i < 5; i++)
	{
		string test;
		for (size_t j = 0; j < 100; j++)
		{
			test.push_back(static_cast<char>(rand() % 255));
		}
		string decode = run_encode_decode(test);

		ASSERT_EQ(test, decode);
	}
}


void generate(const size_t buf)
{
	std::ofstream fout(filename_input.c_str(), std::ios_base::binary);
	for (int i = 0; i < buf; ++i)
	{
		byte* out = new byte[buf];
		for (int j = 0; j < buf; ++j)
		{
			out[j] = rand() % 255;
		}
		fout.write(reinterpret_cast<const char*>(out), buf);
		delete[] out;
	}
	fout.close();
}

void check(const string& input_decode)
{
	std::ifstream fin(filename_input.c_str(), std::ios_base::binary);
	std::ifstream forig(input_decode.c_str(), std::ios_base::binary);
	char* input_chunk = new char[BUFFER];
	char* original_chunk = new char[BUFFER];
	for (;;)
	{
		fin.read(input_chunk, BUFFER);
		const auto size_in = fin ? BUFFER : fin.gcount();
		forig.read(original_chunk, BUFFER);
		const auto size_new = forig ? BUFFER : forig.gcount();
		if (!fin || !forig)
			break;
		ASSERT_EQ(size_in, size_new);
		for (size_t i = 0; i < size_in; ++i)
		{
			ASSERT_EQ(input_chunk[i], original_chunk[i]);
		}
	}
	delete[] input_chunk;
	delete[] original_chunk;
}

TEST(encode_decode, file_input)
{
	generate(256);

	compress(filename_input, filename_output, filename_hf);
	string input_decode = "new.in";
	decompress(filename_output, input_decode, filename_hf);
	check(input_decode);
}

TEST(encode_decode, file_big_input)
{
	generate(1024);

	compress(filename_input, filename_output, filename_hf);
	string input_decode = "new.in";
	decompress(filename_output, input_decode, filename_hf);
	check(input_decode);
}
