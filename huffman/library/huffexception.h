#ifndef HUFFMANEXCEPTION_H
#define HUFFMANEXCEPTION_H


#include <string>
#include <vector>

class HuffException {
public:
	enum error { INFILE_NOT_OPEN, OUTFILE_NOT_OPEN, UNCORRECT_FILE_FORMAT };
private:
	HuffException::error err_;
	std::string filename_;
public:
	HuffException(HuffException::error err, std::string filename);

	HuffException::error get_error() const;
	std::string get_filename() const;
};
#endif