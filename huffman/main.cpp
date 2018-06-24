#include "library/huffman.h"
#include "library/huffexception.h"
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"

#include "library/huffman.h"
#include <algorithm>
using namespace huffman;

void compress(string filename_in, string filename_out, string filename_hf)
{
	std::ifstream fin(filename_in.c_str(), std::ios_base::binary);
	if (!fin.is_open())
	{
		throw HuffException(HuffException::INFILE_NOT_OPEN, filename_in);
	}

	std::ofstream fhf(filename_hf.c_str(), std::ios_base::binary);
	if (!fhf.is_open())
	{
		fin.close();
		throw HuffException(HuffException::OUTFILE_NOT_OPEN, filename_hf);
	}
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
	fhf.write(reinterpret_cast<const char*>(&out[0]), out_size);
	delete[] out;
	fhf.close();


	std::ofstream fout(filename_out.c_str(), std::ios_base::binary);
	if (!fout.is_open())
	{
		fin.close();
		throw HuffException(HuffException::OUTFILE_NOT_OPEN, filename_out);
	}
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
	if (!fhf.is_open())
		throw HuffException(HuffException::OUTFILE_NOT_OPEN, filename_hf);
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
	if (!fin.is_open())
		throw HuffException(HuffException::INFILE_NOT_OPEN, filename_in);
	vector<byte> output;

	std::ofstream fout(filename_out.c_str(), std::ios_base::binary);
	if (!fout.is_open())
	{
		fin.close();
		throw HuffException(HuffException::OUTFILE_NOT_OPEN, filename_out);
	}
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

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		std::cout << "Usage: huffman [-d] input_file [output_file]" << std::endl;
		return 0;
	}

	bool decode = false;
	if (string(argv[1]) == "-d")
		decode = true;

	if (argc == 2 && decode)
	{
		std::cout << "Usage: huffman [-d] input_file [output_file]" << std::endl;
		return 0;
	}

	const string input = argv[1 + decode];

	string output;
	if (argc == 3 + decode)
	{
		output = argv[2 + decode];
	}
	else
	{
		output = "output.out";
	}
	const string out_hf = "output.hf";
	compress(input, output, out_hf);
	string input_decode = "original.in";
	decompress(output, input_decode, out_hf);
	return 0;
}
