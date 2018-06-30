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

		// Изменяет this так, чтобы он содержал те же элементы, что и rhs.
		// Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
		persistent_set& operator=(persistent_set const& rhs);

		// Деструктор. Вызывается при удалении объектов persistent_set.
		// Инвалидирует все итераторы ссылающиеся на элементы этого persistent_set
		// (включая итераторы ссылающиеся на элемент следующий за последним).
		~persistent_set() = default;

		// Поиск элемента.
		// Возвращает итератор на найденный элемент, либо end(), если элемент
		// с указанным значением отсутвует.
		iterator find(T value);

		// Вставка элемента.
		// 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
		//    на уже присутствующий элемент и false.
		// 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
		//    элемент и true.
		// Если вставка произведена, инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
		std::pair<iterator, bool> insert(T value);

		// Удаление элемента.
		// Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
		void erase(iterator it);

		// Возващает итератор на элемент с минимальный ключом.
		iterator begin() const;
		// Возващает итератор на элемент следующий за элементом с максимальным ключом.
		iterator end() const;

		void swap(persistent_set<T>& other) noexcept
		{
			swap(root, other.root);
			swap(version, other.version);
		}
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
		void find_del(shared_ptr<node> root, T x);
		void del(shared_ptr<node> root);
		iterator find(node* cur, T value);
		iterator next(const node* cur, const node* last, T const& x) const;
		iterator prev(const node* cur, const node* last, T const& x) const;
		node* go_left(node* cur) const noexcept;
		node* go_right(node* cur) const noexcept;

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
		friend persistent_set<T>;
		// Элемент на который сейчас ссылается итератор.
		// Разыменование итератора end() неопределено.
		// Разыменование невалидного итератора неопределено.
		T const& operator*() const;

		// Переход к элементу со следующим по величине ключом.
		// Инкремент итератора end() неопределен.
		// Инкремент невалидного итератора неопределен.
		iterator& operator++();
		iterator operator++(int);

		// Переход к элементу с предыдущим по величине ключом.
		// Декремент итератора begin() неопределен.
		// Декремент невалидного итератора неопределен.
		iterator& operator--();
		iterator operator--(int);

		bool operator==(iterator const& other) const noexcept
		{
			if(is_end != other.is_end)
			{
				return false;
			}
			if(is_end)
			{
				return version == other.version;
			}
			return (ptr == other.ptr && set == other.set && version == other.version);
		}

		bool operator!=(iterator const& other) const noexcept
		{
			return !(*this == other);
		}

		iterator(const persistent_set<T>* set, const node* ptr, const uint64_t version, const bool is_end = false) : set(set), ptr(ptr), version(version), is_end(is_end){}
		iterator(iterator const& other) : set(other.set), ptr(other.ptr), version(other.version), is_end(other.is_end){}
	private:
		const persistent_set<T>* set;
		const node* ptr;
		uint64_t version;
		bool is_end;
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
	void persistent_set<T>::find_del(shared_ptr<node> root, T x)
	{
		if(root->value == x)
		{
			del(root);
		}
		else if (root->value > x)
		{
			if (root->left.use_count() > 1)
			{
				auto new_left_node = std::make_shared<node>(*(root->left));
				root->left = new_left_node;
			}
			return find_del(root->left, x);
		}
		else
		{
			if (root->right.use_count() > 1)
			{
				auto new_right_node = std::make_shared<node>(*(root->right));
				root->right = new_right_node;
			}
			return find_del(root->right, x);
		}
	}

	template <typename T>
	void persistent_set<T>::del(shared_ptr<node> root)
	{

		T tmp(root->value);
		if (root->left)
		{
			if (root->left.use_count() > 1)
			{
				auto new_left_node = std::make_shared<node>(*(root->left));
				root->left = new_left_node;
			}
			root->value = root->left->value;
			root->left->value = tmp;
			del(root->left);
		}
		else if (root->right)
		{
			if (root->right.use_count() > 1)
			{
				auto new_right_node = std::make_shared<node>(*(root->right));
				root->right = new_right_node;
			}
			root->value = root->right->value;
			root->right->value = tmp;
			del(root->right);
		}
		else
		{
			root.~shared_ptr();
		}
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::find(node* cur, T value)
	{
		if (cur == nullptr)
		{
			return end();
		}
		else if (cur->value == value)
		{
			return iterator(this, cur, version);
		}
		else
		{
			return find((cur->value > value) ? cur->left.get() : cur->right.get(), value);
		}
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::next(const node* cur, const node* last, T const& x) const
	{
		///			сur					value	?	x
		///		left	right
		if (x < cur->value)
		{
			return next(cur->left.get(), cur, x);
		}
		else if (x > cur->value)
		{
			return next(cur->right.get(), last, x);
		}
		// expect x == cur->value
		if (cur->right)
		{
			cur = go_left(cur->right.get());
			return iterator(this, cur, version);
		}
		// right list
		if (last) // was cur = cur->left
		{
			return iterator(this, last, version);
		}
		return end();
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::prev(const node* cur, const node* last, T const& x) const
	{
		///			сur					value	?	x
		///		left	right
		if (x < cur->value)
		{
			return prev(cur->left.get(), last, x);
		}
		else if (x > cur->value)
		{
			return prev(cur->right.get(), cur, x);
		}
		// expect x == cur->value
		if (cur->left)
		{
			cur = go_right(cur->left.get());
			return iterator(this, cur, version);
		}
		// left list
		if (last) // was cur = cur->right
		{
			return iterator(this, last, version);
		}
		return end();
	}

	template <typename T>
	typename persistent_set<T>::node* persistent_set<T>::go_left(node* cur) const noexcept
	{
		while (cur->left)
		{
			cur = cur->left.get();
		}
		return cur;
	}

	template <typename T>
	typename persistent_set<T>::node* persistent_set<T>::go_right(node* cur) const noexcept
	{
		while (cur->right)
		{
			cur = cur->right.get();
		}
		return cur;
	}

	template <typename T>
	T const& persistent_set<T>::iterator::operator*() const
	{
		return ptr->value;
	}

	template <typename T>
	typename persistent_set<T>::iterator& persistent_set<T>::iterator::operator++()
	{
		if(is_end)
		{
			throw std::runtime_error("Not found next element");
		}
		*this = set->next(set->root.get(), nullptr, ptr->value);
		return *this;
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::iterator::operator++(int postfix)
	{
		iterator tmp(*this);
		++(*this);
		return tmp;
	}

	template <typename T>
	typename persistent_set<T>::iterator& persistent_set<T>::iterator::operator--()
	{
		*this = set->prev(set->root.get(), nullptr, ptr->value);
		return *this;
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::iterator::operator--(int postfix)
	{
		iterator tmp(*this);
		--(*this);
		return tmp;
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
	typename persistent_set<T>::iterator persistent_set<T>::find(T value)
	{
		return find(root.get(), value);
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
		iterator it = find(value);
		if (it != end())
			return std::make_pair(it, false);
		invalidate_iterators();
		return create_node(root.get(), value, (root->value > value) ? 'l' : 'r');
	}

	template <typename T>
	void persistent_set<T>::erase(iterator it)
	{
		invalidate_iterators();
		T value(it.ptr->value);
		if(find(value) == end())
		{
			return;
		}
		if(root->value == value)
		{
			T tmp(root->value);
			
			if(root->right)
			{
				if (root->right.use_count() > 1)
				{
					auto new_right_node = std::make_shared<node>(*(root->right));
					root->right = new_right_node;
				}
				root->value = root->right->value;
				root->right->value = tmp;
				del(root->right);
			}
			else if (root->left)
			{
				if (root->left.use_count() > 1)
				{
					auto new_left_node = std::make_shared<node>(*(root->left));
					root->left = new_left_node;
				}
				root->value = root->left->value;
				root->left->value = tmp;
				del(root->left);
			}
			else
			{
				root.~unique_ptr();
			}
			return;
		}
		find_del((root->value > value) ? root->left : root->right, value);
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::begin() const
	{
		auto min = go_left(root.get());

		if(min == nullptr)
		{
			return end();
		}
		else
		{
			//return end();
			return iterator(this, min, version);
		}
	}

	template <typename T>
	typename persistent_set<T>::iterator persistent_set<T>::end() const
	{
		return iterator(nullptr, nullptr, version, true);
	}
}
#endif //SET_H
