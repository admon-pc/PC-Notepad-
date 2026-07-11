#ifndef _NPLIST_H_
#define _NPLIST_H_

template<typename _type>
struct npListNode
{
	npListNode() :m_left(0), m_right(0) {}
	~npListNode() {}
	_type m_data;
	npListNode* m_left;
	npListNode* m_right;
};

// circular double linked list
template<typename _type>
class npList
{
	npList(const npList& other) {};
	npList(npList&& other) {};
public:
	npList() :m_head(0) {}
	~npList() {
		clear();
	}

	npListNode<_type>* find(const _type& data)
	{
		if (!m_head)
			return 0;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			if (node->m_data == data)
				return node;

			if (node == last)
				break;
			node = node->m_right;
		}
		return 0;
	}

	void clear()
	{
		if (!m_head)
			return;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = m_head->m_right;
			m_head->~npListNode();
			alMemory::Free(m_head);

			if (m_head == last)
				break;
			m_head = next;
		}
		m_head = nullptr;
	}


	npListNode<_type>* insert_after(const _type& after_this, const _type& data) {
		npListNode<_type>* node = (npListNode<_type>*)alMemory::Malloc(sizeof(npListNode<_type>));
		new(node)npListNode<_type>();

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto c = m_head;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_data == after_this)
				{
					auto r = c->m_right;

					node->m_left = c;
					node->m_right = r;

					c->m_right = node;
					r->m_left = node;

					return node;
				}

				if (c == l)
					break;
				c = c->m_right;
			}

			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
		return node;
	}

	npListNode<_type>* insert_before(const _type& after_this, const _type& data) {
		npListNode<_type>* node = (npListNode<_type>*)alMemory::Malloc(sizeof(npListNode<_type>));
		new(node)npListNode<_type>();

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto c = m_head;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_data == after_this)
				{
					auto l = c->m_left;

					node->m_left = l;
					node->m_right = c;

					c->m_left = node;
					l->m_right = node;

					return node;
				}

				if (c == l)
					break;
				c = c->m_right;
			}

			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
		return node;
	}

	npListNode<_type>* push_back(const _type& data)
	{
		npListNode<_type>* node = (npListNode<_type>*)alMemory::Malloc(sizeof(npListNode<_type>));
		new(node)npListNode<_type>();

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
		return node;
	}

	npListNode<_type>* push_front(const _type& data)
	{
		npListNode<_type>* node = (npListNode<_type>*)alMemory::Malloc(sizeof(npListNode<_type>));
		new(node)npListNode<_type>();

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			node->m_right = m_head;
			node->m_left = m_head->m_left;
			m_head->m_left->m_right = node;
			m_head->m_left = node;
			m_head = node;
		}
		return node;
	}

	void pop_front()
	{
		if (!m_head)
			return;
		auto next = m_head->m_right;
		auto last = m_head->m_left;
		m_head->~npListNode();
		alMemory::Free(m_head);

		if (next == m_head)
		{
			m_head = nullptr;
			return;
		}
		m_head = next;
		next->m_left = last;
		last->m_right = next;
	}

	void pop_back()
	{
		if (!m_head)
			return;

		auto lastNode = m_head->m_left;
		lastNode->m_left->m_right = m_head;
		m_head->m_left = lastNode->m_left;

		lastNode->~npListNode();
		alMemory::Free(lastNode);

		if (lastNode == m_head)
		{
			m_head = nullptr;
		}
	}

	// maybe not correct
	void erase(npListNode<_type>* node) {
		auto l = node->m_left;
		auto r = node->m_right;
		l->m_right = r;
		r->m_left = l;
		node->~npListNode();
		alMemory::Free(node);
		if (node == m_head)
			m_head = 0;
	}

	bool erase_first(const _type& object) {
		if (!m_head)
			return false;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data == object)
			{
				if (node == m_head)
					pop_front();
				else
				{
					node->m_left->m_right = node->m_right;
					node->m_right->m_left = node->m_left;
					node->~npListNode();
					alMemory::Free(node);

					// ???
					if (node == m_head)
						m_head = 0;
				}
				return true;
			}
			if (node == last)
				break;
			node = next;
		}
		return false;
	}

	void replace(const _type& oldObject, const _type& newObject) {
		if (!m_head)
			return;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data == oldObject)
			{
				node->m_data = newObject;
				return;
			}
			if (node == last)
				break;
			node = next;
		}
	}

	void reverse() {
		if (!m_head)
			return;
		npListNode<_type>* tail = m_head->m_left;
		npListNode<_type>* curr = m_head;
		while (true)
		{
			auto l = curr->m_left;
			auto r = curr->m_right;

			curr->m_left = r;
			curr->m_right = l;

			if (curr == tail)
				break;
			curr = r;
		}
		m_head = tail;
	}

	void erase_by_node(npListNode<_type>* object)
	{
		if (!m_head)
			return;

		object->m_left->m_right = object->m_right;
		object->m_right->m_left = object->m_left;

		if (object == m_head)
			m_head = m_head->m_right;

		if (object == m_head)
			m_head = 0;

		object->~npListNode();
		alMemory::Free(object);
	}

	class Iterator
	{
		//	friend class ConstIterator;

		npListNode<_type>* m_node;
		npListNode<_type>* m_nodeEnd;
		bool m_isEnd;
	public:
		Iterator() :m_node(0), m_isEnd(true) {}
		Iterator(npListNode<_type>* head) :m_node(head), m_isEnd(false)
		{
			if (!head)
			{
				m_isEnd = true;
			}
			else
			{
				m_nodeEnd = head->m_left;
			}
		}
		~Iterator() {}

		Iterator& operator ++() {
			if (m_node == m_nodeEnd) {
				m_isEnd = true;
			}
			m_node = m_node->m_right;
			return *this;
		}

		bool operator ==(const Iterator& other) const {
			if (m_isEnd != other.m_isEnd) return false;
			return true;
		}
		bool operator !=(const Iterator& other) const {
			if (m_isEnd != other.m_isEnd) return true;
			return false;
		}
		const _type& operator*() {
			return m_node->m_data;
		}
		_type* operator->() {
			return m_node->m_data;
		}
	};

	Iterator begin() {
		return Iterator(m_head);
	}
	Iterator end() {
		return Iterator();
	}

	npListNode<_type>* m_head;
};



#endif

