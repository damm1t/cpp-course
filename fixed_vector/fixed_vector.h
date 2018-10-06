//
// Created by donat on 06.10.2018.
//

#ifndef FIXED_VECTOR_VECTOR_H
#define FIXED_VECTOR_VECTOR_H

#include <cstddef>
#include <type_traits>
#include <iterator>
#include <cassert>

template<typename T, size_t CAPACITY>
struct fixed_vector {
private:
    size_t size_;
    T *data_ = static_cast<T*>(operator new(CAPACITY * sizeof(T)));

public:

    fixed_vector() : size_(0) {}

    fixed_vector(fixed_vector const &other) : size_(other.size_) {
        for (size_t i = 0; i < size_; i++)
            new(data_ + i)T(*(other.data_ + i));
    }

    fixed_vector &operator=(fixed_vector other) {
        swap(*this, other);
        return *this;
    }

    void push_back(T const &value) {
        assert(size() < CAPACITY);
        new(data_ + size_++)T(value);
    }

    void pop_back() {
        assert(0 < size());
        (data_ + --size_)->~T();
    }

    void push_front(T const &value) {
        assert(size() < CAPACITY);
        for (size_t i = size_; i > 0; i--) {
            new(data_ + i)T(*(data_ + i - 1));
            (data_ + i - 1)->~T();
        }
        new(data_)T(value);
        size_++;
    }

    void pop_front() {
        assert(0 < size());
        (data_)->~T();
        size_--;
        for (size_t i = 0; i < size_; i++) {
            new(data_ + i)T(*(data_ + i + 1));
            (data_ + i + 1)->~T();
        }
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return CAPACITY;
    }

    bool empty() const {
        return size_ == 0;
    }

    T operator[](size_t ind) const {
        assert(0 <= ind && ind < size_);
        return *(data_ + ind);
    }

    T &operator[](size_t ind) {
        assert(0 <= ind && ind < size_);
        return *(data_ + ind);
    }

    friend void swap(fixed_vector<T, CAPACITY> &_one, fixed_vector<T, CAPACITY> &_two) {
        std::swap(_one.size_, _two.size_);
        std::swap(_one.data_, _two.data_);
    }

    template<typename it>
    struct iterator_impl : std::iterator<std::random_access_iterator_tag, it> {
    private:
        it *iterator_;
    public:
        using diff_t = typename std::iterator<std::random_access_iterator_tag, it>::difference_type;

        iterator_impl() : iterator_(nullptr) {}

        explicit iterator_impl(it *other) : iterator_(other) {}

        iterator_impl(const iterator_impl &other) : iterator_(other.iterator_) {}

        iterator_impl &operator=(it *other) {
            iterator_ = other;
            return *this;
        }

        iterator_impl &operator=(const iterator_impl &other) {
            iterator_ = other.iterator_;
            return *this;
        }

        iterator_impl &operator+=(diff_t other) {
            iterator_ += other;
            return *this;
        }

        iterator_impl &operator-=(diff_t other) {
            iterator_ -= other;
            return *this;
        }

        it &operator*() const { return *iterator_; }

        it *operator->() const { return iterator_; }

        it &operator[](diff_t index) const { return iterator_[index]; }

        iterator_impl &operator++() {
            ++iterator_;
            return *this;
        }

        iterator_impl &operator--() {
            --iterator_;
            return *this;
        }

        iterator_impl operator++(int) {
            iterator_impl tmp(*this);
            ++iterator_;
            return tmp;
        }

        iterator_impl operator--(int) {
            iterator_impl tmp(*this);
            --iterator_;
            return tmp;
        }

        diff_t operator-(const iterator_impl &other) const { return iterator_ - other.iterator_; }

        iterator_impl operator+(diff_t other) const { return iterator_impl(iterator_ + other); }

        iterator_impl operator-(diff_t other) const { return iterator_impl(iterator_ - other); }

        friend iterator_impl operator+(diff_t one, const iterator_impl &two) { return iterator_impl(one + two.iterator_); }

        friend iterator_impl operator-(diff_t one, const iterator_impl &two) { return iterator_impl(one - two.iterator_); }

        bool operator==(const iterator_impl &rhs) const { return iterator_ == rhs.iterator_; }

        bool operator!=(const iterator_impl &rhs) const { return iterator_ != rhs.iterator_; }

        bool operator>(const iterator_impl &rhs) const { return iterator_ > rhs.iterator_; }

        bool operator<(const iterator_impl &rhs) const { return iterator_ < rhs.iterator_; }

        bool operator>=(const iterator_impl &rhs) const { return iterator_ >= rhs.iterator_; }

        bool operator<=(const iterator_impl &rhs) const { return iterator_ <= rhs.iterator_; }
    };

    using iterator = iterator_impl<T>;
    using const_iterator = iterator_impl<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(data_);
    }

    iterator end() {
        return iterator(data_ + size_);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_iterator begin() const {
        return const_iterator(data_);
    }

    const_iterator end() const {
        return const_iterator(const_cast<const T *>(data_ + size_));
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    iterator insert(const_iterator index, T const &value) {
        assert(size() < CAPACITY);
        assert(begin() <= index && index <= end());
        for (size_t i = size_; i > index - begin(); i--) {
            new(data_ + i)T(*(data_ + i - 1));
            (data_ + i - 1)->~T();
        }
        new(data_ + (index - begin()))T(value);
        size_++;
        return index;
    }

    iterator erase(iterator index) {
        assert(begin() <= index && index < end());
        (data_ + (index - begin()))->~T();
        --size_;
        for (size_t i = index - begin(); i < size_; i++) {
            new(data_ + i)T(*(data_ + i + 1));
            (data_ + i + 1)->~T();
        }
        return index;
    }

    iterator erase(iterator begin, iterator end) {
        assert(begin() <= begin && begin < end && end < end());
        for (auto i = begin; i != end; ++i) {
            (data_ + (i - begin()))->~T();
            --size_;
        }
        for (auto i = end, j = begin; i != end(); i++, j++) {
            new(data_ + j)T(*(data_ + i));
            (data_ + i)->~T();
        }
        return begin;
    }

    T front() {
        assert(size_ > 0);
        return *(data_);
    }

    T back() {
        assert(size_ > 0);
        return *(data_ + size_ - 1);
    }

    void clear() {
        for (; size_ > 0;) {
            pop_back();
        }
    }


};

#endif //FIXED_VECTOR_VECTOR_H
