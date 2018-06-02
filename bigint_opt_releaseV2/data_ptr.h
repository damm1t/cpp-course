#ifndef DATA_H
#define DATA_H

#include <vector>
#include <cstddef>
#include <memory>
#include <iostream>

using shared_data = std::shared_ptr<std::vector<uint32_t>>;

struct opt_vector {
	opt_vector();
	opt_vector(uint32_t a);
	~opt_vector();
	opt_vector(opt_vector const &other);

	uint32_t& back();

	uint32_t const& back() const;

	/*void resize(size_t i);

	void resize(size_t i, uint32_t val);*/

	size_t size() const;

	/*auto begin() const
	{
		if(this->small_flag)
		{
			
		}
		else
		{
			return this->data_ptr->begin();
		}
	}

	auto end() const
	{
		if (this->small_flag)
		{
			return &this->small_value;
		}
		else
		{
			return this->data_ptr->end();
		}
	}
	auto rbegin() const
	{
		return this->end();
	}
	auto rend() const
	{
		return this->begin();
	}
	*/
	uint32_t& operator[](size_t i);

	uint32_t const& operator[](size_t i) const;

	opt_vector &operator=(opt_vector const &other);

	uint32_t pop_back();

	void push_back(uint32_t val);

	//void swap(opt_vector &b) noexcept;

private:
	union {
		uint32_t small_value;
		shared_data data_ptr;
	};
	bool small_flag; //small_value or not
	bool empty_flag;
	void make_own();

};

#endif //DATA_H
