#pragma once

#include <vector>

template <class _Ty>
class WeightedVector
{
	std::vector<std::pair<unsigned int, _Ty>> vec;
	unsigned int totWeight;

public:

	class iterator {
		
		std::_Vector_iterator<std::_Vector_val<std::_Simple_types<std::pair<unsigned int, _Ty>>>> iter; // Why do I have to do this?

	public:
		iterator(const decltype(iter) &iter) { this->iter = iter; }

		_Ty &operator *() const { return iter->second; }

		const iterator &operator++() {
			++iter;
			return *this;
		}

		bool operator!=(const iterator& rhs) {
			return this->iter != rhs.iter;
		}

	};

	class const_iterator {

		std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<std::pair<unsigned int, _Ty>>>> iter; // Why do I have to do this?

	public:
		const_iterator(const decltype(iter) &iter) { this->iter = iter; }

		const _Ty &operator *() const { return iter->second; }

		const_iterator &operator++() {
			++iter;
			return *this;
		}

		bool operator!=(const const_iterator& rhs) const {
			return this->iter != rhs.iter;
		}

	};

	iterator begin() { return iterator(vec.begin()); }
	iterator end() { return iterator(vec.end()); }
	const_iterator begin() const { return const_iterator(vec.cbegin()); }
	const_iterator end() const { return const_iterator(vec.cend()); }

	void push_back(const _Ty &elem, unsigned int weight = 1) {
		totWeight += weight;
		vec.push_back(std::pair<unsigned int, _Ty>(weight, elem));
	}

	void push_back(_Ty &&elem, unsigned int weight = 1) {
		totWeight += weight;
		vec.push_back(std::pair<unsigned int, _Ty>(weight, elem));
	}

	_Ty *getWeighted(unsigned int weight) {
		if (weight >= totWeight) return nullptr;
		unsigned int cWeight = 0;
		for (auto &pair : vec) {
			if (pair.first + cWeight > weight) 
				return &pair.second;
			cWeight += pair.first;
		}
		return nullptr;
	}

	_Ty *getIndexed(unsigned int index) {
		if (index >= vec.size()) return nullptr;
		return &vec.at(index);
	}

	void setWeight(_Ty *ptr, unsigned int newWeight) {
		for (auto &pair : vec) {
			if (&pair.second == ptr) {
				totWeight -= (pair.first - newWeight);
				pair.first = newWeight;
				return;
			}
		}
	}

	void addToAllWeights(int diff) {
		for (auto &pair : vec){
			if (-diff > pair.first) {
				totWeight -= pair.first;
				pair.first = 0;
				continue;
			}
			pair.first += diff;
			totWeight += diff;
		}
	}

	unsigned int size() const { return vec.size(); }
	unsigned int weight() const { return totWeight; }
	unsigned int reachable() const {
		unsigned int count = 0;
		for (const auto &pair : vec) {
			if (pair.first) count++;
		}
		return count;
	}

	WeightedVector() :
		totWeight(0)
	{
	}

	~WeightedVector()
	{
	}
};
