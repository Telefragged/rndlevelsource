#pragma once
#include "Timer.h"
#include <random>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sstream>

#define WNODE(_n) static_cast<WeightedNode<T>*>(_n)
#define UINTTIME (unsigned int)std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())

template <class T>
class WeightedNode
{
private:
	T *val;
	WeightedNode<T> *next;
	unsigned int weight;
public:

	static auto stdcmp(WeightedNode<T> *lhs, WeightedNode<T> *rhs) -> bool {
		return (lhs->value() <= rhs->value());
	}

	void setnext(decltype(next) newNext) {
		next = newNext;
	}

	void setval(decltype(val) newVal) {
		this->val = newVal;
	}

	void add(WeightedNode<T> *val) {
		next = val;
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

	unsigned int getWeight() {
		return weight;
	}

	void setWeight(unsigned int weight) {
		this->weight = weight;
	}

	WeightedNode(T *val)
	{
		this->val = val;
		this->next = nullptr;
		this->weight = 1;
	}

	WeightedNode(T *val, unsigned int weight)
	{
		this->val = val;
		this->next = nullptr;
		this->weight = weight;
	}

	~WeightedNode() {
		delete val;
	}
};

template <class T>
class WeightedList
{
private:
	unsigned int size_, prevind_, totweight_;
	WeightedNode<T> *first, *last, *prev;

	auto getNode(unsigned int index) -> decltype(first) {
		if(index >= size_) return nullptr;
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
		return cur;
	}
public:

	class iterator {
		WeightedNode<T> *node;
	public:
		iterator(WeightedNode<T> *node) {this->node = node;}

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
		WeightedNode<T> *node;
	public:
		const_iterator(WeightedNode<T> *node) {this->node = node;}

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

	static void swap(WeightedList<T> &lhs, WeightedList<T> &rhs) {
		std::swap(lhs.size_, rhs.size_);
		std::swap(lhs.totweight_, rhs.totweight_);
		std::swap(lhs.prevind_, rhs.prevind_);
		std::swap(lhs.first, rhs.first);
		std::swap(lhs.last, rhs.last);
		std::swap(lhs.prev, rhs.prev);
	}

	WeightedList<T> &operator=(WeightedList<T> &&orig) {
		swap(*this, orig);
		return *this;
	}

	WeightedList(void)
	{
		totweight_ = 0;
		size_ = 0;
		prevind_ = 0;
		first = nullptr;
		last = nullptr;
		prev = nullptr;
	}

	WeightedList(WeightedList<T> && orig)
	{
		totweight_ = 0;
		size_ = 0;
		prevind_ = 0;
		first = nullptr;
		last = nullptr;
		prev = nullptr;
		swap(*this, orig);
	}

	T *getweighted(unsigned int weight) {
		if(weight >= totweight_) return nullptr;
		WeightedNode<T> *cnode = first;
		unsigned int toget = weight, cweight = 0, cind = 0;
		while(cnode->getWeight() + cweight <= toget) {
			cind++;
			cweight+=cnode->getWeight();
			cnode = cnode->nextnode();
		}
		return cnode->ptr();
	}

	template <class _eng>
	T *getweighted(_eng &engine) {
		std::uniform_int_distribution<unsigned int> dist(0, totweight_-1);
		return getweighted(dist(engine));
	}

	void setWeight(unsigned int index, unsigned int newWeight) {
		WeightedNode<T> *node = getNode(index);
		unsigned int wdiff = node->getWeight() - newWeight;
		node->setWeight(newWeight);
		totweight_ -= wdiff;
	}

	void setWeight(T *ptr, unsigned int newWeight) {
		if(ptr == nullptr) return;
		unsigned int ind = 0;
		for(T &elem : *this) {
			if(&elem == ptr) {
				setWeight(ind, newWeight);
				return;
			}
			ind++;
		}
	}
	void put(T *val) {
		auto newNode = new WeightedNode<T>(val);
		if(first == nullptr) {
			first = newNode;
			last = first;
			prev = first;
		} else {
			last->add(newNode);
			last = last->nextnode();
		}
		size_++;
		totweight_++;
	}

	void put(T *val, unsigned int weight) {
		put(val);
		WeightedNode<T> *node = last;
		node->setWeight(weight);
		totweight_ += weight-1;
	}

	void clear() {
		WeightedNode<T> *node = first, *nnode = nullptr;
		while(node != nullptr) {
			nnode = node;
			node = node->nextnode();
			if(nnode != nullptr) delete nnode;
		}
		size_ = 0;
		prevind_ = 0;
		totweight_ = 0;
		first = nullptr;
		last = nullptr;
		prev = nullptr;
	}

	WeightedList(const WeightedList<T> &orig) {
		size_ = 0;
		prevind_ = 0;
		totweight_ = 0;
		first = nullptr;
		last = nullptr;
		prev = nullptr;
		WeightedNode<T> *origptr = orig.first;
		while(origptr != nullptr) {
			put(new T(*(origptr->ptr())), origptr->getWeight());
			origptr = origptr->nextnode();
		}
	}

	void printlist() {
		WeightedNode<T> *cnode = first;
		unsigned int cnt = 0, wcnt = 0;
		std::cout<<"+-------+----------+--------------+\n";
		std::cout<<"| index | "<<std::setw(11)<<"value | "<<std::setw(15)<<"weight |\n";
		std::cout<<"+-------+----------+--------------+\n";
		while(cnode != nullptr) {
			unsigned int nwgt = cnode->getWeight();
			std::ostringstream wgtstream;
			wgtstream.precision(2);
			wgtstream << std::fixed << nwgt <<" ("<<(float(nwgt)/totweight_)*100.0<<"%)";
			wcnt+=nwgt;
			std::cout<<"| "<<std::setw(5)<<++cnt<<" | "<<std::setw(8)<<(*cnode->ptr())["angles"]<<" | "<<std::setw(12)<<wgtstream.str()<<" |\n";
			cnode = cnode->nextnode();
		}
		std::cout<<"+-------+----------+--------------+\n";
		std::cout<<"| "<<std::setw(5)<<size_<<" | "<<std::setw(11)<<"* | "<<std::setw(12)<<wcnt<<" |\n";
		std::cout<<"+-------+----------+--------------+\n";
	}

	~WeightedList() {
		clear();
	}
};
