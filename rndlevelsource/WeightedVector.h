#pragma once

#include <vector>
#include <random>

template <class _Ty>
class WeightedVector
{
	std::vector<std::pair<size_t, _Ty>> vec;
	size_t totalWeight = 0;
public:

	class iterator
	{
		typename std::vector<std::pair<size_t, _Ty>>::iterator iter;
	public:
		iterator(const decltype(iter)& iter)
		{
			this->iter = iter;
		}

		_Ty& operator *() const
		{
			return iter->second;
		}

		const iterator& operator++()
		{
			++iter;
			return *this;
		}

		bool operator!=(const iterator& rhs)
		{
			return this->iter != rhs.iter;
		}
	};

	class const_iterator
	{
		typename std::vector<std::pair<size_t, _Ty>>::const_iterator iter;
	public:
		const_iterator(const decltype(iter)& iter)
		{
			this->iter = iter;
		}

		const _Ty& operator *() const
		{
			return iter->second;
		}

		const_iterator& operator++()
		{
			++iter;
			return *this;
		}

		bool operator!=(const const_iterator& rhs) const
		{
			return this->iter != rhs.iter;
		}
	};

	iterator begin()
	{
		return iterator(vec.begin());
	}

	iterator end()
	{
		return iterator(vec.end());
	}

	const_iterator begin() const
	{
		return const_iterator(vec.cbegin());
	}

	const_iterator end() const
	{
		return const_iterator(vec.cend());
	}

	void push_back(const _Ty& elem, unsigned int weight = 1)
	{
		totalWeight += weight;
		vec.push_back(std::pair<unsigned int, _Ty>(weight, elem));
	}

	void push_back(_Ty&& elem, size_t weight = 1)
	{
		totalWeight += weight;
		vec.push_back(std::pair<size_t, _Ty>(weight, elem));
	}

	_Ty* getWeighted(size_t weight)
	{
		if (weight >= totalWeight)
			return nullptr;

		size_t cWeight = 0;
		for (auto& pair : vec)
		{
			if (pair.first + cWeight > weight)
				return &pair.second;
			cWeight += pair.first;
		}
		return nullptr;
	}

	_Ty* getIndexed(unsigned int index)
	{
		if (index >= vec.size()) return nullptr;
		return &vec.at(index).second;
	}

	// Modify weight of the object pointed at by ptr.
	// Returns how much the weight actually changed by ( Weight can't be less than 0 )
	int changeWeight(_Ty* ptr, int diff)
	{
		for (auto& pair : vec)
		{
			if (&pair.second == ptr)
			{
				int aDiff = (-diff > int(pair.first)) ? -int(pair.first) : diff;
				totalWeight += aDiff;
				pair.first += aDiff;
				return aDiff;
			}
		}
		return 0;
	}

	void setWeight(_Ty* ptr, unsigned int newWeight)
	{
		for (auto& pair : vec)
		{
			if (&pair.second == ptr)
			{
				totalWeight -= (pair.first - newWeight);
				pair.first = newWeight;
				return;
			}
		}
	}

	void addToAllWeights(int diff)
	{
		for (auto& pair : vec)
		{
			if (-diff > pair.first)
			{
				totalWeight -= pair.first;
				pair.first = 0;
				continue;
			}
			pair.first += diff;
			totalWeight += diff;
		}
	}

	template <typename _Eng>
	void addToRandomWeights(unsigned int distWeight, _Eng& engine)
	{
		std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
		for (unsigned int n = 0; n < distWeight; n++)
		{
			auto& pair = vec[dist(engine)];
			++pair.first;
		}
		totalWeight += distWeight;
	}

	size_t size() const
	{
		return vec.size();
	}

	size_t weight() const
	{
		return totalWeight;
	}

	unsigned int reachable() const
	{
		unsigned int count = 0;
		for (const auto& pair : vec)
		{
			if (pair.first) count++;
		}
		return count;
	}

	WeightedVector(const std::vector<_Ty>& other, unsigned int initWeight = 1)
	{
		for (const auto& item : other)
		{
			vec.emplace_back(initWeight, item);
		}
		totalWeight = initWeight * other.size();
	}

	WeightedVector() = default;

	~WeightedVector() = default;
};

