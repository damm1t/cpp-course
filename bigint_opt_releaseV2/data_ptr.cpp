//
// Created by damm1t on 30.05.18.
//
#include "data_ptr.h"
#include <stdexcept>
#include <algorithm>
#include <cassert>


opt_vector::opt_vector() : small_flag(true)
{
}

opt_vector::opt_vector(const uint32_t a) : small_flag(true)
{
	small_value = a;
}

opt_vector::~opt_vector()
{
	if(!this->small_flag)
	{
		data_ptr.~shared_ptr();
	}
}

opt_vector::opt_vector(opt_vector const& other)
{
	this->small_flag = other.small_flag;
	if (this->small_flag)
		this->small_value = other.small_value;
	else
		this->data_ptr = other.data_ptr;
}


opt_vector& opt_vector::operator=(opt_vector const& other)
{
	if (this == &other)
		return *this;

	small_flag = other.small_flag;
	if (small_flag)
		small_value = other.small_value;
	else
		data_ptr = other.data_ptr;
	return *this;
}

uint32_t& opt_vector::back()
{
	if (this->small_flag)
		return this->small_value;

	this->make_own();
	return this->data_ptr->back();
}

uint32_t opt_vector::back() const
{
	return this->small_flag ? this->small_value : this->data_ptr->back();
}

size_t opt_vector::size() const
{
	if (this->small_flag)
		return 1;
	return this->data_ptr->size();
}

uint32_t const& opt_vector::operator[](size_t i) const
{
	assert(i < size());
	if (this->small_flag) {
		assert(i == 0);
		return this->small_value;
	}
	return this->data_ptr->at(i);
}

uint32_t& opt_vector::operator[](size_t i)
{
	assert(i < size());
	if (this->small_flag)
		return this->small_value;

	this->make_own();
	return this->data_ptr->at(i);
}

//void opt_vector::swap(opt_vector& b)
//{
//	std::swap(this->data_ptr, b.data_ptr);
//	if (small_flag)
//		std::swap(this->small_value, b.small_value);
//	std::swap(this->small_flag, b.small_flag);
//}

void opt_vector::make_own()
{
	if (!this->data_ptr.unique())
		this->data_ptr = std::make_shared<std::vector<uint32_t>>(*this->data_ptr);
}

//
//void opt_vector::resize(const size_t size)
//{
//	assert(size == 0); //TODO 
//
//	if (this->small_flag)
//	{
//		if (size > 1)
//		{
//			const auto old_value = small_value;
//			data_ptr = std::make_shared<std::vector<uint32_t>>(size);
//			(*data_ptr)[size - 1] = old_value;
//			this->small_flag = false;
//		}
//	}
//	else
//	{
//		if (size <= 1)
//		{
//			this->small_value = this->data_ptr->at(0);
//			this->small_flag = true;
//		}
//		else {
//			this->make_own();
//			this->data_ptr->resize(size);
//		}
//	}
//}
//
//void opt_vector::resize(size_t i, uint32_t val)
//{
//	if (this->small_flag)
//	{
//		if (i > 1)
//		{
//			const auto old_value = small_value;
//			data_ptr = std::make_shared<std::vector<uint32_t>>(std::vector<uint32_t>(i, val));
//			(*data_ptr)[i - 1] = old_value;
//			this->small_flag = false;
//		} else
//		{
//			
//		}
//	}
//	else
//	{
//		if (i <= 1)
//		{
//			this->small_value = val;
//			this->small_flag = 3;
//		}
//		else
//		{
//			this->make_own();
//			this->data_ptr->resize(i, val);
//		}
//	}
//}

uint32_t opt_vector::pop_back()
{
	if (this->data_ptr->size() > 1)
	{
		if (this->data_ptr->size() == 2)
		{
			const uint32_t tmp = this->data_ptr->at(0);
			const uint32_t res = this->data_ptr->at(1);
			this->small_value = tmp;
			this->small_flag = true;
			return res;
		}
		else
		{
			this->make_own();
			const uint32_t res = this->data_ptr->back();
			this->data_ptr->pop_back();
			return res;
		}
	}
	else
	{
		const uint32_t res = small_value;
		small_value = 0;
		small_flag = true;
		return res;
	}
}

void opt_vector::push_back(const uint32_t val)
{
	if (this->small_flag)
	{
		if (this->small_value == 0)
		{
			this->small_value = val;
			this->small_flag = true;
		}
		else
		{
			const uint32_t tmp = this->small_value;
			this->data_ptr = std::make_shared<std::vector<uint32_t>>();
			this->data_ptr->push_back(tmp);
			this->data_ptr->push_back(val);
			this->small_flag = false;
		}
	}
	else
	{
		this->make_own();
		this->data_ptr->push_back(val);
	}
}
