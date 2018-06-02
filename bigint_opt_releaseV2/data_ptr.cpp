//
// Created by damm1t on 30.05.18.
//
#include "data_ptr.h"
#include <stdexcept>
#include <algorithm>
#include <cassert>


opt_vector::opt_vector() : small_flag(true), empty_flag(true)
{
}

opt_vector::opt_vector(const uint32_t a) : small_flag(true), empty_flag(false)
{
	small_value = a;
}

opt_vector::~opt_vector()
{
	if (!this->small_flag)
	{
		data_ptr.~shared_ptr();
	}
}

opt_vector::opt_vector(opt_vector const& other) : small_flag(other.small_flag), empty_flag(other.empty_flag)
{
	if (this->small_flag)
		this->small_value = other.small_value;
	else
		this->data_ptr = other.data_ptr;
}


opt_vector& opt_vector::operator=(opt_vector const& other)
{
	if (this == &other)
		return *this;

	if (small_flag != other.small_flag)
	{
		if (small_flag)
		{
			to_big();
			data_ptr = other.data_ptr;
		}
		else
		{
			to_small();
			small_value = other.small_value;
		}
	}
	else if (small_flag)
		small_value = other.small_value;
	else
		data_ptr = other.data_ptr;
	empty_flag = other.empty_flag;
	small_flag = other.small_flag;
	return *this;
}

uint32_t& opt_vector::back()
{
	assert(empty_flag == false);
	if (this->small_flag)
		return this->small_value;

	this->make_own();
	return this->data_ptr->back();
}

uint32_t const& opt_vector::back() const
{
	assert(empty_flag == false);
	return this->small_flag ? this->small_value : this->data_ptr->back();
}

size_t opt_vector::size() const
{
	if (empty_flag)
		return 0;
	if (this->small_flag)
		return 1;
	return this->data_ptr->size();
}

uint32_t const& opt_vector::operator[](size_t i) const
{
	assert(i < size());
	if (this->small_flag)
	{
		assert(i == 0);
		return small_value;
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

void opt_vector::make_own()
{
	if (!this->data_ptr.unique())
		this->data_ptr = std::make_shared<std::vector<uint32_t>>(*this->data_ptr);
}

void opt_vector::to_big()
{
	if (!small_flag)
		return;
	auto tmp = small_value;
	new(&data_ptr) std::shared_ptr<std::vector<uint32_t>>();
	data_ptr = std::make_shared<std::vector<uint32_t>>(1, tmp);
	small_flag = false;
}

void opt_vector::to_small()
{
	if (small_flag)
		return;
	data_ptr.~shared_ptr();
	small_flag = false;
	small_flag = true;
}

uint32_t opt_vector::pop_back()
{
	if (empty_flag)
		return 0;
	if (this->data_ptr->size() > 1)
	{
		if (this->data_ptr->size() == 2)
		{
			const uint32_t tmp = this->data_ptr->at(0);
			const uint32_t res = this->data_ptr->at(1);
			small_value = tmp;
			small_flag = true;
			return res;
		}
		else
		{
			make_own();
			const uint32_t res = this->data_ptr->back();
			data_ptr->pop_back();
			return res;
		}
	}
	else
	{
		const uint32_t res = small_value;
		small_value = 0;
		small_flag = true;
		empty_flag = true;
		return res;
	}
}

void opt_vector::push_back(const uint32_t val)
{
	if (empty_flag)
	{
		small_value = val;
		small_flag = true;
		empty_flag = false;
	}
	else if (small_flag)
	{
		const uint32_t tmp = small_value;
		new(&data_ptr) std::shared_ptr<std::vector<uint32_t>>();
		data_ptr = std::make_shared<std::vector<uint32_t>>();
		data_ptr->push_back(tmp);
		data_ptr->push_back(val);
		small_flag = false;
	}
	else
	{
		this->make_own();
		this->data_ptr->push_back(val);
	}
}
