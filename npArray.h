#pragma once
#ifndef _ALARR_H_
#define _ALARR_H_

// `clear` must do m_size = 0;
template<typename type>
class npArray
{
	size_t     m_allocated;
	void Reallocate(size_t new_capacity)
	{
		new_capacity += 4 * (m_allocated / 2);
		auto tmp_size = new_capacity * sizeof(type);
		type* new_data = static_cast<type*>(malloc(tmp_size));
		memset(new_data, 0, tmp_size);

		if (m_data)
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				new(&new_data[i]) type(m_data[i]);
				(&m_data[i])->~type();
			}
			free(m_data);
		}
		m_data = new_data;
		m_allocated = new_capacity;
	}
public:
	npArray() :m_allocated(0), m_size(0), m_data(0) {}
	npArray(const npArray<type>& other)
	{
		assign(other);
	}
	npArray(npArray<type>&& other)
	{
		m_allocated = other.m_allocated;
		m_data = other.m_data;
		m_size = other.m_size;
		other.m_allocated = 0;
		other.m_data = 0;
		other.m_size = 0;
	}
	~npArray() { FreeMemory(); }

	npArray<type>& operator=(const npArray<type>& other)
	{
		assign(other);
		return *this;
	}
	npArray<type>& operator=(npArray<type>&& other)
	{
		m_allocated = other.m_allocated;
		m_data = other.m_data;
		m_size = other.m_size;
		other.m_allocated = 0;
		other.m_data = 0;
		other.m_size = 0;
		return *this;
	}

	void assign(const npArray<type>& other)
	{
		FreeMemory();
		if (other.m_allocated)
		{
			Reallocate(other.m_allocated);
			if (other.m_size)
			{
				for (size_t i = 0; i < m_size; ++i)
				{
					push_back(other.m_data[i]);
				}
			}
		}
	}

	const type& operator[](size_t id) const { return m_data[id]; }
	type& operator[](size_t id) { return m_data[id]; }

	size_t capacity()
	{
		return m_allocated;
	}

	size_t size()
	{
		return m_size;
	}

	void ShrinkToFit()
	{
		auto new_capacity = m_size;
		auto tmp_size = new_capacity * sizeof(type);
		type* new_data = static_cast<type*>(malloc(tmp_size));
		memset(new_data, 0, tmp_size);

		if (m_data)
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				new(&new_data[i]) type(m_data[i]);
				(&m_data[i])->~type();
			}
			free(m_data);
		}
		m_data = new_data;
		m_allocated = new_capacity;
	}

	void reserve(size_t new_capacity)
	{
		if (new_capacity > m_allocated)
			Reallocate(new_capacity);
	}

	void insert(size_t where, const type& object)
	{
		if (where >= m_size)
		{
			push_back(object);
		}
		else
		{
			size_t new_size = m_size + 1;
			if (new_size > m_allocated)
				Reallocate(new_size);

			for (size_t i = m_size; i > where;)
			{
				m_data[i] = m_data[i - 1];
				--i;
			}
			new(&m_data[where]) type(object);

			m_size = new_size;
		}
	}

	void push_back(const type& object)
	{
		size_t new_size = m_size + 1;
		if (new_size > m_allocated)
			Reallocate(new_size);
		new(&m_data[m_size]) type(object);
		m_size = new_size;
	}

	void PushFront(const type& object)
	{
		insert(0, object);
	}

	void FreeMemory()
	{
		if (m_data)
		{
			for (size_t i = 0; i < m_allocated; ++i)
			{
				(&m_data[i])->~type();
			}
			free(m_data);

			m_allocated = m_size = 0;
			m_data = nullptr;
		}
	}

	void clear()
	{
		m_size = 0;
	}

	type* Get(size_t index)
	{
		if (index >= m_allocated)
			return nullptr;

		return &m_data[index];
	}

	/*
	struct _pred
	{
	bool operator() (const Map::renderNode& a, const Map::renderNode& b) const
	{
	return a.m_position.y > b.m_position.y;
	}
	};
	m_renderSprites.sort_insertion(_pred());
	*/
	template<class _Pr>
	void SortInsertion(_Pr _pred)
	{
		size_t i, j;
		type t;
		for (i = 1; i < m_size; ++i)
		{
			for (j = i; j > 0 && _pred(m_data[j - 1], m_data[j]); --j)
			{
				t = m_data[j];
				m_data[j] = m_data[j - 1];
				m_data[j - 1] = t;
			}
		}
	}

	void erase(size_t index)
	{
		for (size_t i = index; i < m_size; ++i)
		{
			if (i + 1 < m_size)
				m_data[i] = m_data[i + 1];
			else
				m_data[i] = 0;
		}
		if (m_size)
			--m_size;
	}

	bool EraseFirst(const type& ref)
	{
		for (size_t i = 0u; i < m_size; ++i)
		{
			if (m_data[i] == ref)
			{
				erase(i);
				return true;
			}
		}
		return false;
	}

	type* m_data = 0;
	size_t m_size = 0;
};



#endif

