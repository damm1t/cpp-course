//
// Created by damm1t on 01.04.18.
//
#include "big_integer.h"
#include <stdexcept>
#include <algorithm>

// private functions:

void big_integer::negate()
{
	signum_ = -signum_;
}

void big_integer::mul_long_short(const uint32_t val)
{
	uint64_t tmp = 0;
	for (size_t i = 0; i < data_.size(); ++i)
	{
		tmp += uint64_t(data_[i]) * uint64_t(val);
		data_[i] = static_cast<uint32_t>(tmp & MAXINT32);
		tmp >>= LOG;
	}
	while (tmp)
	{
		data_.push_back(uint32_t(tmp & MAXINT32));
		tmp >>= LOG;
	}
}

// PRE : value  >= 0
void big_integer::add(const uint32_t val)
{
	uint64_t tmp = val;
	for (size_t i = 0; i < data_.size(); ++i)
	{
		tmp += data_[i];
		data_[i] = tmp & MAXINT32;
		tmp >>= LOG;
		if (!tmp)
			break;
	}
	if (tmp)
		data_.push_back(static_cast<uint32_t>(tmp));
}

uint32_t big_integer::div_long_short(const uint32_t val)
{
	uint32_t mod = 0;
	const uint32_t mod_val = base % val;
	uint64_t carry = 0;
	for (auto i = data_.size() - 1;; --i)
	{
		mod = (uint64_t(mod) * mod_val + uint64_t(data_[i])) % val;
		const auto tmp = carry * base + static_cast<uint64_t>(data_[i]);
		data_[i] = static_cast<uint32_t>(tmp / val);
		carry = tmp % val;
		if (i == 0) // i type of size_t
			break;
	}
	correct_size();
	return mod;
}

uint32_t big_integer::div10()
{
	uint32_t mod = 0;
	uint64_t carry = 0;
	for (auto i = data_.size() - 1; ; --i)
	{
		mod = (uint64_t(mod) * base10 + uint64_t(data_[i])) % 10;
		const auto tmp = carry * base + uint64_t(data_[i]);
		data_[i] = static_cast<uint32_t>(tmp / 10ll);
		carry = tmp % 10ll;
		if (!i)
			break;
	}
	correct_size();
	return mod;
}

uint32_t big_integer::mod(const uint32_t val)
{
	uint32_t ans = 0;
	const uint32_t mod_val = val == 10 ? base10 : base % val;
	for (size_t i = 0; i < data_.size(); ++i)
	{
		ans = (uint64_t(ans) * mod_val + data_[i]) % 10;
	}
	return ans;
}

void big_integer::correct_size(const size_t expected_size)
{
	if (expected_size == 0)
	{
		while (data_.size() > 1 && !data_.back())
			data_.pop_back();
	}
	else
		while (data_.size() < expected_size)
			data_.push_back(0);
	if (data_.back() == 0)
		signum_ = 0;
}

// PRE : sign(this) == sign(b)
void big_integer::add(big_integer const& b)
{
	uint64_t tmp = 0;
	correct_size(b.size());
	for (size_t i = 0; i < b.size(); ++i)
	{
		tmp += uint64_t(data_[i]) + uint64_t(b[i]);
		data_[i] = tmp & MAXINT32;
		tmp >>= LOG;
	}
	const auto pos = b.size();
	while (tmp)
	{
		correct_size(pos + 1);
		tmp += data_[pos];
		data_[pos] = tmp & MAXINT32;
		tmp >>= LOG;
	}
	correct_size();
}

// PRE : sign(this) == sign(b), |this| >= |b|
void big_integer::subtract(big_integer const& b)
{
	int64_t tmp = 0;
	for (size_t i = 0; i < b.size(); ++i)
	{
		tmp += int64_t(data_[i]) - b[i];
		if (tmp < 0)
		{
			data_[i] = static_cast<uint32_t>(tmp + int64_t(base));
			tmp = -1;
		}
		else
		{
			data_[i] = static_cast<uint32_t>(tmp);
			tmp = 0;
		}
	}
	const auto pos = b.size();
	while (tmp != 0)
	{
		tmp += data_[pos];
		if (tmp < 0)
		{
			data_[pos] = static_cast<uint32_t>(tmp + base);
			tmp = -1;
		}
		else
		{
			data_[pos] = static_cast<uint32_t>(tmp);
			tmp = 0;
		}
	}
	correct_size();
}

void big_integer::mul_max_int32()
{
	data_.push_back(0);
	for (auto i = data_.size() - 1; i > 0; --i)
		data_[i] = data_[i - 1];
	data_[0] = 0;
}

void big_integer::div_max_int32()
{
	for (size_t i = 0; i < data_.size() - 1; ++i)
		data_[i] = data_[i + 1];
	data_.back() = 0;
	correct_size();
}

void big_integer::invert()
{
	for (size_t i = 0; i < size(); ++i)
		data_[i] = ~data_[i];
}

void big_integer::to_add_code()
{
	push_back(0), push_back(0);
	if (signum_ < 0)
	{
		invert();
		add(1);
		signum_ = 1;
	}
}

void big_integer::to_simple_code()
{
	if (back() != 0)
		signum_ = -1, invert(), add(1);
	correct_size();
}

uint32_t& big_integer::operator[](const size_t i)
{
	return data_[i];
}

const uint32_t& big_integer::operator[](const size_t i) const
{
	return data_[i];
}

size_t big_integer::size() const
{
	return data_.size();
}

uint32_t& big_integer::back()
{
	return data_.back();
}

const uint32_t& big_integer::back() const
{
	return data_.back();
}

void big_integer::pop_back()
{
	data_.pop_back();
}

void big_integer::push_back(const uint32_t x)
{
	data_.push_back(x);
}

/*bool big_integer::empty() const
{
	return data_.empty();
}*/


//constructors :

big_integer::big_integer()
{
	this->data_ = opt_vector(0);
	this->signum_ = 0;
}

big_integer::big_integer(const int val)
{
	data_ = opt_vector(abs(int64_t(val)));
	signum_ = val ? (val < 0 ? -1 : 1) : 0;
}

big_integer::big_integer(const uint32_t val)
{
	data_ = opt_vector(val);
	signum_ = val != 0;
}

big_integer::big_integer(const uint64_t val)
{
	data_ = opt_vector(val & MAXINT32);
	data_.push_back(val >> LOG);
	correct_size();
	signum_ = val != 0;
}

big_integer::big_integer(std::vector<uint32_t>& v, const short signum)
	: data_(opt_vector())
	, signum_(signum)
{
	if (v.empty())
	{
		return;
	}
	data_[0] = v[0];
	for (size_t i = 1; i < v.size(); i++)
		this->data_.push_back(v[i]);
}

big_integer::big_integer(std::string const& s)
{
	data_.push_back(0);
	signum_ = 0;
	for (auto c : s)
	{
		if (c == '+')
			signum_ = 1;
		else if (c == '-')
			signum_ = -1;
		else
		{
			if (!signum_ && c != '0') signum_ = 1;
			mul_long_short(10);
			add(static_cast<uint32_t>(c - '0'));
		}
	}
	if (signum_ == -1 && !data_.back())
		signum_ = 0;
}

// copy constructor
big_integer::big_integer(big_integer const& other)
{
	data_ = other.data_;
	signum_ = other.signum_;
}

std::string big_integer::to_string() const
{
	std::string answer = "";
	auto cur = *this;
	while (cur.signum_ != 0)
	{
		answer.push_back(cur.div10() + '0');
	}
	if (signum_ < 0)
		answer.push_back('-');
	std::reverse(answer.begin(), answer.end());
	if (answer == "")
		answer = "0";
	return answer;
}

bool big_integer::is_deg2() const
{
	if (back() != 1) 
		return false;
	for (size_t i = 0; i < size() - 1; ++i)
		if (data_[i]) 
			return false;
	return true;
}

// operators :

big_integer& big_integer::operator=(const big_integer& other)
{
	if (this == &other)
		return *this;
	
	data_ = other.data_;
	signum_ = other.signum_;
	return *this;
}


// comporator

int abs_cmp(big_integer const& a, big_integer const& b)
{
	if (a.size() != b.size())
	{
		return a.size() > b.size() ? 1 : -1;
	}
	else
	{
		std::vector<int> v;
		v.rend();
		//const auto pair = std::mismatch(a.data_.rbegin(), a.data_.rend(), b.data_.rbegin());
		for (int i = a.size() - 1; i >= 0; --i)
			{
				if (a[i] == b[i])
					continue;
				return a[i] > b[i] ? 1 : -1;
			}
		return 0;
	}
}

int cmp(big_integer const& a, big_integer const& b, const bool comp_abs)
{
	const auto res_abs = abs_cmp(a, b);
	if (comp_abs)
		return res_abs;

	if (a.signum_ != b.signum_)
		return a.signum_ > b.signum_ ? 1 : -1;
	return a.signum_ > 0 ? res_abs : -res_abs;
}


//int cmp(big_integer const& a, big_integer const& b, const bool comp_abs)
//{
//	if (!comp_abs && a.signum_ != b.signum_)
//		return (a.signum_ > b.signum_ ? 1 : -1);
//	if (a.size() != b.size())
//	{
//		if (a.size() > b.size() && a.signum_ == 1 && !comp_abs)
//			return 1;
//		if (a.size() < b.size() && a.signum_ == -1 && !comp_abs)
//			return 1;
//		if (comp_abs)
//		{
//			if (a.size() > b.size())
//				return 1;
//		}
//		return -1;
//	}
//	//std::mismatch(a..rbegin())
//	for (int i = a.size() - 1; i >= 0; --i)
//	{
//		if (a[i] == b[i])
//			continue;
//		if (!comp_abs && a[i] > b[i] && a.signum_ == 1)
//			return 1;
//		if (!comp_abs && a[i] < b[i] && a.signum_ == -1)
//			return 1;
//		if (comp_abs)
//			return a[i] > b[i] ? 1 : -1;
//		return -1;
//	}
//	return 0;
//}

bool operator==(big_integer const& a, big_integer const& b)
{
	return cmp(a, b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
	return cmp(a, b) != 0;
}

bool operator<(big_integer const& a, big_integer const& b)
{
	return cmp(a, b) < 0;
}

bool operator>(big_integer const& a, big_integer const& b)
{
	return cmp(a, b) > 0;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
	return cmp(a, b) <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b)
{
	return cmp(a, b) >= 0;
}


//iostream:
std::istream& operator>>(std::istream& in, big_integer& bint)
{
	std::string tmp;
	in >> tmp;
	bint = big_integer(tmp);
	return in;
}

std::ostream& operator<<(std::ostream& out, big_integer const& bint)
{
	out << bint.to_string();
	return out;
}


//arithmetic :

// unary:
big_integer operator-(big_integer const& a)
{
	auto ans = a;
	ans.signum_ = -ans.signum_;
	return ans;
}

big_integer operator+(big_integer const& a)
{
	return a;
}

big_integer operator+(big_integer const& a, big_integer const& b)
{
	big_integer res;
	if (a.signum_ == 0)
		return res = b;
	if (b.signum_ == 0)
		return res = a;
	if (a.signum_ == b.signum_)
	{
		res = a;
		res.add(b);
		return res;
	}
	if (a.signum_ < 0)
	{
		auto tmp = a;
		tmp.negate();
		res = b;
		if (tmp > res)
		{
			tmp.subtract(res);
			res = -tmp;
		}
		else
			res.subtract(tmp);
	}
	else
	{
		auto tmp = b;
		tmp.negate();
		res = a;
		if (tmp > res)
		{
			tmp.subtract(res);
			res = -tmp;
		}
		else
			res.subtract(tmp);
	}
	res.correct_size();
	return res;
}

//binary
big_integer operator-(big_integer const& a, big_integer const& b)
{
	return a + -b;
}

big_integer operator*(big_integer const& a, big_integer const& b)
{
	big_integer res = 0;
	if (!a.signum_ || !b.signum_)
		return res;
	res.correct_size(a.size() + b.size());
	uint64_t tmp = 0;
	for (size_t i = 0; i < a.size(); ++i)
	{
		for (size_t j = 0; j < b.size(); ++j)
		{
			tmp += uint64_t(res[i + j]) + uint64_t(a[i]) * b[j];
			res[i + j] = tmp & MAXINT32;
			tmp >>= LOG;
		}
		auto pos = i + b.size();
		while (tmp)
		{
			tmp += uint64_t(res[pos]);
			res[pos++] = tmp & MAXINT32;
			tmp >>= LOG;
		}
	}
	res.correct_size();
	res.signum_ = a.signum_ * b.signum_;
	return res;
}

big_integer operator/(big_integer const& a, big_integer const& b)
{
	if (b == 0)
		throw std::runtime_error("DivByZezo Exception");
	if (a == 0)
		return 0;
	const short signum = a.signum_ * b.signum_;
	auto first = a * a.signum_, second = b * b.signum_;
	if (first < second)
		return 0;
	if (first == second)
		return signum;
	if (second.size() == 1)
	{
		first.div_long_short(second[0]);
		first.signum_ = signum;
		return first;
	}
	const auto normal = static_cast<uint32_t>(base / (second.back() + 1));
	first.mul_long_short(normal);
	second.mul_long_short(normal);
	const auto n = first.size(), m = second.size();
	std::vector<uint32_t> q(n);
	auto beta = big_integer::base_deg(n - m) * second;
	for (auto j = n - m; ; --j)
	{
		uint64_t qwrd = 0;
		if (m + j - 1 < first.size())
			qwrd = first[m + j - 1] / second.back();
		if (m + j < first.size())
			qwrd = (uint64_t(first[m + j]) * base + first[m + j - 1]) / second.back();
		q[j] = static_cast<uint32_t>(std::min(qwrd, uint64_t(MAXINT32)));
		auto tmp = beta;
		tmp.mul_long_short(q[j]);
		tmp.correct_size();
		first -= tmp;
		auto cnt = 0;
		while (first.signum_ == -1)
		{
			--q[j];
			first += beta;
			++cnt;
		}
		beta.div_max_int32();
		if (j == 0) break;
	}
	big_integer answer(q);
	answer.correct_size();
	answer.signum_ = signum;
	return answer;
}

big_integer operator%(big_integer const& a, big_integer const& b)
{
	return a - a / b * b;
}

big_integer& operator+=(big_integer& res, big_integer const& param)
{
	return res = res + param;
}

big_integer& operator-=(big_integer& res, big_integer const& param)
{
	return res = res - param;
}

big_integer& operator*=(big_integer& res, big_integer const& param)
{
	return res = res * param;
}

big_integer& operator/=(big_integer& res, big_integer const& param)
{
	return res = res / param;
}

big_integer& operator%=(big_integer& res, big_integer const& param)
{
	return res = res % param;
}

big_integer& operator^=(big_integer& res, big_integer const& param)
{
	return res = res ^ param;
}

big_integer& operator|=(big_integer& res, big_integer const& param)
{
	return res = res | param;
}

big_integer& operator&=(big_integer& res, big_integer const& param)
{
	return res = res & param;
}

big_integer& operator<<=(big_integer& res, const int rhs)
{
	return res = res << rhs;
}

big_integer& operator>>=(big_integer& res, const int rhs)
{
	return res = res >> rhs;
}

template <class FunctorT>
big_integer big_integer::any_binary(big_integer const& a, big_integer const& b, FunctorT f)
{
	auto answer = a, tmp = b;
	answer.correct_size(tmp.size());
	tmp.correct_size(answer.size());
	answer.to_add_code();
	tmp.to_add_code();
	for (size_t i = 0; i < tmp.size(); ++i)
		answer[i] = f(answer[i], tmp[i]);
	answer.to_simple_code();
	return answer;
}

big_integer operator^(big_integer const& a, big_integer const& b)
{
	return big_integer::any_binary(a, b, std::bit_xor<uint32_t>()); //std::bit_xor<uint32_t>()
}

big_integer operator|(big_integer const& a, big_integer const& b)
{
	return big_integer::any_binary(a, b, std::bit_or<uint32_t>());
}

big_integer operator&(big_integer const& a, big_integer const& b)
{
	return big_integer::any_binary(a, b, std::bit_and<uint32_t>());
}

big_integer operator<<(big_integer const& a, int rhs)
{
	if (rhs < 0) return a >> rhs;
	const auto complete = rhs >> 5;
	rhs &= 31;
	auto ans = a;
	ans.to_add_code();
	uint64_t tmp = 0;
	for (size_t i = 0; i < a.size(); ++i)
	{
		tmp |= uint64_t(a[i]) << rhs;
		ans[i] = tmp & MAXINT32;
		tmp >>= LOG;
	}
	if (tmp) ans.push_back(static_cast<uint32_t>(tmp));
	ans.correct_size();
	return ans * big_integer::base_deg(static_cast<size_t>(complete));
}

big_integer operator>>(big_integer const& a, int rhs)
{
	if (rhs < 0) return a << rhs;
	const auto complete = rhs >> 5;
	rhs &= 31;
	auto ans = a;
	uint32_t tmp = 0;
	for (auto i = a.size() - 1; ; --i)
	{
		const auto digit = ans[i];
		ans[i] = (tmp << LOG - rhs) + (ans[i] >> rhs);
		tmp = digit & (1ll << rhs) - 1;
		if (!i) break;
	}
	ans.correct_size();
	if (!a.is_deg2() && a.signum_ == -1) ans -= 1;
	return ans * big_integer::base_deg(static_cast<size_t>(complete));
}


big_integer operator~(big_integer const& a)
{
	auto answer = a;
	answer.to_add_code();
	answer.invert();
	answer.to_simple_code();
	return answer;
}

bool operator!(big_integer const& x)
{
	return !x.signum_;
}

big_integer big_integer::base_deg(const size_t n)
{
	std::vector<uint32_t> res_data(n + 1, 0);
	res_data.back() = 1;
	return big_integer(res_data, 1);
}


// destructor:
//big_integer::~big_integer()
//{
//	data_.clear();
//}

std::string to_string(big_integer const& b)
{
	return b.to_string();
}
