#pragma once

#include <mutex>

template <class T>
class LinkedNode
{
private:
	T *val;
	LinkedNode<T> *next;
public:

	void setnext(decltype(next) newNext) {
		next = newNext;
	}

	void setval(decltype(val) newVal) {
		this->val = newVal;
	}

	void add(LinkedNode<T> *val) {
		next = val;
	}

	T &value() {
		return *val;
	}

	auto ptr() -> decltype(val) {
		return val;
	}

	auto nextnode() -> decltype(next) {
		return next;
	}

	T *operator*() {
		return this->ptr();
	}

	LinkedNode(T *val) {
		this->val = val;
		next = nullptr;
	}

	~LinkedNode(void) {
		if(val != nullptr) delete val;
	}
};

template<class T>
class LinkedList
{
protected:
	LinkedNode<T> *first, *last, *prev;
	unsigned int size_, prevind_;
	std::mutex cmnmtx;

	auto getNode(unsigned int index) -> decltype(first) {
		if(index >= size_) return nullptr;
		cmnmtx.lock();
		unsigned int cind = 0;
		auto cur = first;
		if(index >= prevind_ && prev != nullptr) {
			cind = prevind_;
			cur = prev;
		}
		while(index != cind) {
			cur = cur->nextnode();
			cind++;
		}
		prevind_ = index;
		prev = cur;
		cmnmtx.unlock();
		return cur;
	}

	static unsigned int partition(T **vals, unsigned int beg, unsigned int end, unsigned int pivotind, bool (*cmpfnc)(const T &, const T &)) {
		T *pivot = vals[pivotind];
		std::swap(vals[pivotind], vals[end]);
		unsigned int storeind = beg;
		for(unsigned int n = beg; n < end; n++) {
			if(cmpfnc(*vals[n], *pivot)) {
				std::swap(vals[n], vals[storeind]);
				storeind++;
			}
		}
		std::swap(vals[storeind], vals[end]);
		return storeind;
	}

	static void qsort(T **vals, unsigned int beg, unsigned int end, bool (*cmpfnc)(const T &, const T &)) {
		if(cmpfnc == nullptr || beg >= end) return;
		unsigned int pivotval = end;
		unsigned int piv = partition(vals, beg, end, pivotval, cmpfnc);
		if(piv > 0) qsort(vals, beg, piv-1, cmpfnc);
		if(piv < end) qsort(vals, piv+1, end, cmpfnc);
	}

public:

	static void swap(LinkedList<T> &lhs, LinkedList<T> &rhs) {
		std::swap(lhs.first, rhs.first);
		std::swap(lhs.last, rhs.last);
		std::swap(lhs.prev, rhs.prev);
		std::swap(lhs.size_, rhs.size_);
		std::swap(lhs.prevind_, rhs.prevind_);
	}

	void quicksort(bool (*cmpfnc)(const T &, const T &) = nullptr) {
		T **vals = toPtrArray();
		qsort(vals, 0, size_ - 1, cmpfnc);
		//unsigned int arrsize = size_;
		//unassign();
		//put(vals, arrsize);
		delete [] vals;
	}

	class iterator {
		LinkedNode<T> *node;
	public:
		iterator(LinkedNode<T> *node) {this->node = node;}

		T &operator *() const {return *node->ptr();}

		const iterator &operator++() {
			node = node->nextnode();
			return *this;
		}

		bool operator!=(const iterator& rhs) {
			return this->node != rhs.node;
		}

	};

	class const_iterator {
		LinkedNode<T> *node;
	public:
		const_iterator(LinkedNode<T> *node) {this->node = node;}

		const T &operator *() const {return *node->ptr();}

		const const_iterator &operator++() {
			node = node->nextnode();
			return *this;
		}

		bool operator!=(const const_iterator& rhs) {
			return this->node != rhs.node;
		}
	};

	iterator begin() {return iterator(first);}
	iterator end() {return iterator(nullptr);}
	const_iterator begin() const {return const_iterator(first);}
	const_iterator end() const {return const_iterator(nullptr);}

	virtual void put(T *val) {
		cmnmtx.lock();
		if(first == nullptr) {
			first = new LinkedNode<T>(val);
			last = first;
			prev = first;
		} else {
			last->add(new LinkedNode<T>(val));
			last = last->nextnode();
		}
		size_++;
		cmnmtx.unlock();
	}

	void put(T *arr, unsigned int len) {
		for(unsigned int n = 0; n < len; n++) {
			put(new T(arr[n]));
		}
	}

	void put(T **arr, unsigned int len) {
		for(unsigned int n = 0; n < len; n++) {
			put(arr[n]);
		}
	}

	T &get(unsigned int index) {
		LinkedNode<T> *n = getNode(index);
		if(n == nullptr) return T();
		return n->value();
	}

	T *getptr(unsigned int index) {
		auto n = getNode(index);
		if(n == nullptr) return 0;
		return n->ptr();
	}

	bool remove(T *ptr) {
		unsigned int ind = 0;
		for(auto &elem : *this) {
			if(&elem == ptr) {
				remove(ind);
				return true;
			}
			ind++;
		}
		return false;
	}

	T *remove(unsigned int index) {
		if(index < 0 || index >= size()) return nullptr;
		T *ret = nullptr;
		if(index == 0) {
			if(prev == first) prev = prev->nextnode();
			else prevind_--;
			ret = first->ptr();
			auto delnode = first;
			first = first->nextnode();
			delnode->setval(nullptr);
			delnode->setnext(nullptr);
			delete delnode;
		} else {
			auto prevnode = getNode(index-1);
			auto delnode = prevnode->nextnode();
			ret = delnode->ptr();
			delnode->setval(nullptr);
			prevnode->setnext(delnode->nextnode());
			delnode->setnext(nullptr);
			delete delnode;
			prevind_ = 0;
			prev = first;
		}
		size_--;
		return ret;
	}

	LinkedList<T> &operator+=(T *rhs) {
		put(rhs);
		return *this;
	}

	LinkedList<T> &operator+=(const T &rhs) {
		put(new T(rhs));
		return *this;
	}

	LinkedList<T> &operator+=(const LinkedList<T> &rhs) {
		unsigned int cap = rhs.size(), count = 0;
		for(const T &elem : rhs) {
			put(new T(const_cast<T &>(elem)));
			if(++count == cap) break;
		}
		return *this;
	}

	LinkedList<T> &operator-=(T *rhs) {
		remove(rhs);
		return *this;
	}

	LinkedList<T> &operator-=(const LinkedList<T> &rhs) {
		if(this == &rhs) {
			clear();
			return *this;
		}
		for(T &elem : rhs) {
			this->remove(&elem);
		}
		return *this;
	}

	LinkedList<T> &operator=(const LinkedList<T> &orig) {
		if(this == &orig) return *this;
		clear();
		for(const T &origt : orig) {
			put(new T(origt));
		}
		return *this;
	}

	LinkedList<T> &operator=(LinkedList<T> &&orig) {
		swap(*this, orig);
		return *this;
	}

	template <class CT>
	T *get_first_match(CT cmp, bool (*cmpfnc)(const T &lhs, const CT &rhs)) const {
		for(const T &elem : *this) {
			if(cmpfnc(elem, cmp)) return const_cast<T *>(&elem);
		}
		return nullptr;
	}

	template <class CT>
	T *remove_first_match(CT cmp, bool (*cmpfnc)(const T &lhs, const CT &rhs)) {
		for(T &elem : *this) {
			if(cmpfnc(elem, cmp)) {
				remove(&elem);
				return &elem;
			}
		}
		return nullptr;
	}

	template <class CT>
	LinkedList<T> get_all_match(CT cmp, bool (*cmpfnc)(const T &lhs, const CT &rhs)) const {
		LinkedList<T> ret;
		for(T &elem : *this) {
			if(cmpfnc(elem, cmp)) ret += elem;
		}
		return ret;
	}

	template <class CT>
	LinkedList<T> remove_all_match(CT cmp, bool (*cmpfnc)(const T &lhs, const CT &rhs)) {
		LinkedList<T> ret;
		for(T &elem : *this) {
			if(cmpfnc(elem, cmp)) ret += &elem;
		}
		(*this) -= ret;
		return ret;
	}

	T pop_first() {
		T ret = first->value();
		if(prev == first) prev = prev->nextnode();
		first = first->nextnode();
		return ret;
	}

	T *peek_last() {
		if(size_ == 0) return nullptr;
		return last->ptr();
	}

	T *toArray() {
		if(size_ == 0) return nullptr;
		T *ret = new T[size_];
		for(unsigned int n = 0; n < size_; n++) {
			ret[n] = T(get(n));
		}
		return ret;
	}

	T **toPtrArray() {
		if(size_ == 0) return nullptr;
		T **ret = new T*[size_];
		for(unsigned int n = 0; n < size_; n++) {
			ret[n] = getptr(n);
		}
		return ret;
	}

	void replace(T *val, unsigned int index) {
		if(index < 0 || index >= size()) return;
		auto node = getNode(index);
		delete node->ptr();
		node->setval(val);
	}

	T removeval(unsigned int index) {
		if(index < 0 || index >= size()) throw index;
		T ret;
		if(index == 0) {
			if(prev == first) prev = prev->nextnode();
			else prevind_--;
			ret = T(first->value());
			auto delnode = first;
			first = first->nextnode();
			delnode->setval(nullptr);
			delnode->setnext(nullptr);
			delete delnode;
		} else {
			auto prevnode = getNode(index-1);
			auto delnode = prevnode->nextnode();
			ret = T(delnode->value());
			delnode->setval(nullptr);
			prevnode->setnext(delnode->nextnode());
			delnode->setnext(nullptr);
			delete delnode;
			prevind_ = 0;
			prev = first;
		}
		size_--;
		return ret;
	}

	unsigned int size() const {
		return size_;
	}

	LinkedList(const LinkedList<T> &orig) :
        first(nullptr),
        last(nullptr),
        prev(nullptr),
        size_(0),
        prevind_(0)
	{
		for(const T &origt : orig) {
			put(new T(origt));
		}
	}

	LinkedList(LinkedList<T> &&orig) :
        first(nullptr),
        last(nullptr),
        prev(nullptr),
        size_(0),
        prevind_(0)
	{
		swap(*this, orig);
	}

	LinkedList() :
        first(nullptr),
        last(nullptr),
        prev(nullptr),
        size_(0),
        prevind_(0)
	{
	}

	void unassign() {
		size_ = 0;
		prevind_ = 0;
		first = nullptr;
		last = nullptr;
		prev = nullptr;
	}

	virtual void clear() {
		size_ = 0;
		prevind_ = 0;
		LinkedNode<T> *node = first, *nnode = nullptr;
		while(node != nullptr) {
			nnode = node;
			node = node->nextnode();
			if(nnode != nullptr) {
				delete nnode;
			}
		}
		first = nullptr;
		last = nullptr;
		prev = nullptr;
	}

	void reverse() {
		auto curptr = first;
		decltype(curptr) nextptr = nullptr;
		while(curptr != nullptr) {
			auto prevptr = curptr->nextnode();
			curptr->setnext(nextptr);
			nextptr = curptr;
			curptr = prevptr;
		}
		swap(first, last);
		prevind_ = 0;
		prev = first;
	}

	LinkedList(const T** arr, unsigned int len) {
		size_ = 0;
		prevind_ = 0;
		put(arr, len);
	}

	~LinkedList(void)
	{
		clear();
	}
};
