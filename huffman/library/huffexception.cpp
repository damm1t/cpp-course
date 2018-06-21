#include "huffexception.h"

using std::string;

HuffException::HuffException(error err, string filename) {
	err_ = err;
	filename_ = filename;
}


HuffException::error HuffException::get_error() const {
	return err_;
}


string HuffException::get_filename() const {
	return filename_;
}