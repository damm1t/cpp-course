#ifndef SET_H
#define SET_H
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <cassert>

namespace algo
{
	using std::shared_ptr;
	using std::unique_ptr;

	template <typename T>
	struct persistent_set
	{
		struct iterator;

		persistent_set() = default;

		persistent_set(persistent_set const& other);

		// �������� this ���, ����� �� �������� �� �� ��������, ��� � rhs.
		// ������������ ��� ���������, ������������� persistent_set'� this, ������� end().
		persistent_set& operator=(persistent_set const& rhs);

		// ����������. ���������� ��� �������� �������� persistent_set.
		// ������������ ��� ��������� ����������� �� �������� ����� persistent_set
		// (������� ��������� ����������� �� ������� ��������� �� ���������).
		~persistent_set();

		// ����� ��������.
		// ���������� �������� �� ��������� �������, ���� end(), ���� �������
		// � ��������� ��������� ���������.
		iterator find(T value);

		// ������� ��������.
		// 1. ���� ����� ���� ��� ������������, ������� �� �������������, ������������ ��������
		//    �� ��� �������������� ������� � false.
		// 2. ���� ������ ����� ��� ���, ������������� �������, ������������ �������� �� ���������
		//    ������� � true.
		// ���� ������� �����������, ������������ ��� ���������, ������������� persistent_set'� this, ������� end().
		std::pair<iterator, bool> insert(T value);

		// �������� ��������.
		// ������������ ��� ���������, ������������� persistent_set'� this, ������� end().
		void erase(iterator it);

		// ��������� �������� �� ������� � ����������� ������.
		iterator begin() const;
		// ��������� �������� �� ������� ��������� �� ��������� � ������������ ������.
		iterator end() const;
	private:
		struct node
		{
			T value;
			shared_ptr<node> left, right;

			node(T val) : value(val)
			{
			}

			node(node& other) : value(other.value), left(other.left), right(other.right)
			{
			}

			~node()
			{
				value.~T();
				left.~shared_ptr();
				right.~shared_ptr();
			}

			node(shared_ptr<node> l, shared_ptr<node> r, T val) : value(val), left(l), right(r)
			{
			}

		private:
			node() = delete;
		};

	private:
		std::pair<iterator, bool> create_node(node* root, T x, char type);
		void invalidate_iterators()
		{
			version++;
		}
		unique_ptr<node> root;
		uint64_t version = 0;
	};

	

	template <typename T>
	struct persistent_set<T>::iterator
	{
		// ������� �� ������� ������ ��������� ��������.
		// ������������� ��������� end() ������������.
		// ������������� ����������� ��������� ������������.
		T const& operator*() const;

		// ������� � �������� �� ��������� �� �������� ������.
		// ��������� ��������� end() �����������.
		// ��������� ����������� ��������� �����������.
		iterator& operator++();
		iterator operator++(int);

		// ������� � �������� � ���������� �� �������� ������.
		// ��������� ��������� begin() �����������.
		// ��������� ����������� ��������� �����������.
		iterator& operator--();
		iterator operator--(int);

		bool operator==(iterator const& other) const noexcept
		{
			return (ptr == other.ptr && set == other.set && version == other.version);
		}

		bool operator!=(iterator const& other) const noexcept
		{
			return !(*this == other);
		}

		iterator(persistent_set<T>* set, node* ptr, uint64_t version) : set(set), ptr(ptr), version(version){}
		
	private:
		persistent_set<T>* set;
		node* ptr;
		uint64_t version;
	};

	template <typename T>
	std::pair<typename persistent_set<T>::iterator, bool> persistent_set<T>::create_node(node* root, T x, char type)
	{
		if (type == 'l')
		{
			if (!root->left)
			{
				root->left = std::make_shared<node>(x);
				auto it = persistent_set<T>::iterator(this, root->left.get(), version);
				return std::pair<iterator, bool>(it, true);
			}
			if (root->left.use_count() > 1)
			{
				auto new_left_node = std::make_shared<node>(*(root->left));
				root->left = new_left_node;
			}
			return create_node(root->left.get(), x, (root->left->value > x) ? 'l' : 'r');
		}
		else
		{
			assert(type == 'r');
			if (!root->right)
			{
				root->right = std::make_shared<node>(x);
				return std::make_pair(iterator(this, root->right.get(), version), true);
			}
			if (root->right.use_count() > 1)
			{
				auto new_right_node = std::make_shared<node>(*(root->right));
				root->right = new_right_node;
			}
			return create_node(root->right.get(), x, (root->right->value > x) ? 'l' : 'r');
		}
	}

	template <typename T>
	T const& persistent_set<T>::iterator::operator*() const
	{
		return ptr->value;
	}

	template <typename T>
	typename persistent_set<T>::iterator& persistent_set<T>::iterator::operator++()
	{
		return iterator(nullptr, nullptr, 0);
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::iterator::operator++(int)
	{
		return iterator(nullptr, nullptr, 0);
	}

	template <typename T>
	typename persistent_set<T>::iterator& persistent_set<T>::iterator::operator--()
	{
		return iterator(nullptr, nullptr, 0);
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::iterator::operator--(int)
	{
		return iterator(nullptr, nullptr, 0);
	}

	template <typename T>
	persistent_set<T>::persistent_set(persistent_set const& other)
	{
		version = 0;
		if (other.root)
		{
			root = std::make_unique<node>(other.root->value);
			root->left = other.root->left;
			root->right = other.root->right;
		}
	}

	template <typename T>
	persistent_set<T>& persistent_set<T>::operator=(persistent_set const& rhs)
	{
		version++;		
		if (rhs.root)
		{
			root = std::make_unique<node>(rhs.root->value);
			root->left = rhs.root->left;
			root->right = rhs.root->right;
		}
	}

	template <typename T>
	persistent_set<T>::~persistent_set()
	{
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::find(T value)
	{
		return iterator(nullptr, nullptr, 0);
	}

	template <typename T>
	std::pair<typename persistent_set<T>::iterator, bool> persistent_set<T>::insert(T value)
	{
		if (root == nullptr)
		{
			invalidate_iterators();
			root = std::make_unique<node>(value);
			auto it = iterator(this, root.get(), version);
			return std::pair<iterator, bool>(it, true);
		}
		/*iterator it = find(value); TODO
		if (it != end())
			return std::make_pair(it, false);*/
		invalidate_iterators();
		return create_node(root.get(), value, (root->value > value) ? 'l' : 'r');
	}

	template <typename T>
	void persistent_set<T>::erase(iterator it)
	{
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::begin() const
	{
		return iterator(nullptr, nullptr, 0);
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::end() const
	{
		return iterator(nullptr, nullptr, 0);
	}
}
#endif //SET_H
