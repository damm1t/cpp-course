#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <iostream>
#include <functional>
#include "data_ptr.h"

const uint32_t MAXINT32 = (1ll << 32) - 1, LOG = 32;
const uint64_t base = MAXINT32 + 1ll;
const uint32_t base10 = uint32_t(base % 10ll);


struct big_integer
{
	big_integer();
	big_integer(int);
	big_integer(uint32_t);
	big_integer(uint64_t);
	big_integer(std::vector<uint32_t>& v, const short signum = 1);
	explicit big_integer(const std::string&);
	big_integer(big_integer const&);

	std::string to_string() const;

	bool is_deg2() const;

	big_integer& operator = (const big_integer&);

	friend bool operator == (big_integer const&, big_integer const&);
	friend bool operator != (big_integer const&, big_integer const&);
	friend bool operator < (big_integer const&, big_integer const&);
	friend bool operator <= (big_integer const&, big_integer const&);
	friend bool operator > (big_integer const&, big_integer const&);
	friend bool operator >= (big_integer const&, big_integer const&);

	friend std::ostream& operator<<(std::ostream&, big_integer const&);
	friend std::istream& operator>>(std::istream&, big_integer const&);

	friend big_integer operator-(big_integer const&);
	friend big_integer operator+(big_integer const&);

	friend big_integer operator+(big_integer const&, big_integer const&);
	friend big_integer operator-(big_integer const&, big_integer const&);
	friend big_integer operator*(big_integer const&, big_integer const&);
	friend big_integer operator/(big_integer const&, big_integer const&);
	friend big_integer operator%(big_integer const&, big_integer const&);


	friend big_integer& operator+=(big_integer&, big_integer const&);
	friend big_integer& operator-=(big_integer&, big_integer const&);
	friend big_integer& operator/=(big_integer&, big_integer const&);
	friend big_integer& operator*=(big_integer&, big_integer const&);
	friend big_integer& operator%=(big_integer&, big_integer const&);
	friend big_integer& operator^=(big_integer&, big_integer const&);
	friend big_integer& operator|=(big_integer&, big_integer const&);
	friend big_integer& operator&=(big_integer&, big_integer const&);
	friend big_integer& operator<<=(big_integer&, int);
	friend big_integer& operator>>=(big_integer&, int);
	template<class FunctorT>
	static big_integer any_binary(big_integer const&, big_integer const&, FunctorT);
	friend big_integer operator^(big_integer const&, big_integer const&);
	friend big_integer operator|(big_integer const&, big_integer const&);
	friend big_integer operator&(big_integer const&, big_integer const&);
	friend big_integer operator<<(big_integer const&, int);
	friend big_integer operator>>(big_integer const&, int);
	friend big_integer operator~(big_integer const&);
	friend bool operator!(big_integer const&);

	inline static big_integer base_deg(size_t n);

	~big_integer() = default;

private:

	//std::vector<uint32_t> data_;
	opt_vector data_;
	short signum_;

	void negate();
	void mul_long_short(uint32_t val);
	void add(uint32_t val);
	uint32_t div_long_short(uint32_t val);
	uint32_t div10();
	uint32_t mod(uint32_t val);
	void correct_size(size_t expected_size = 0);
	void add(big_integer const&);
	void subtract(big_integer const&);
	void mul_max_int32();
	void div_max_int32();
	void invert();
	void to_add_code();
	void to_simple_code();

	uint32_t& operator[](size_t);
	uint32_t const& operator[](size_t) const;
	size_t size() const;
	uint32_t back();
	uint32_t back() const;
	void pop_back();
	void push_back(uint32_t);
	//bool empty() const;
	friend int cmp(big_integer const& a, big_integer const& b, const bool comp_abs = false);
	friend int abs_cmp(big_integer const& a, big_integer const& b);


};



std::string to_string(big_integer const& b);

#endif // BIG_INTEGER_H
