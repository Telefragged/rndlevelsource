#pragma once

#include <vector>
#include <random>
#include <cstddef>

#include <boost/iterator_adaptors.hpp>


template <class _Ty>
class WeightedVectorIterator
	: public boost::iterator_facade<
		WeightedVectorIterator<_Ty>,
		_Ty,
		boost::random_access_traversal_tag,
		_Ty&,
		ptrdiff_t>
{
	typename std::vector<std::pair<ptrdiff_t, _Ty>>::iterator iter;
public:
	WeightedVectorIterator()
	{
	}

	WeightedVectorIterator(const decltype(iter)& iter)
	{
		this->iter = iter;
	}

private:
	friend class boost::iterator_core_access;

	_Ty& dereference() const { return iter->second; }

	bool equal(WeightedVectorIterator<_Ty> const& other) const { return this->iter == other.iter; }
	void increment() { ++iter; }
	void decrement() { --iter; }
	void advance(ptrdiff_t n) { iter += n; }
	ptrdiff_t distance_to(WeightedVectorIterator<_Ty> const& other) const { return other.iter - this->iter; }
};

template <class _Ty>
class WeightedVectorConstIterator
	: public boost::iterator_facade<
	WeightedVectorConstIterator<_Ty>,
	_Ty,
	boost::random_access_traversal_tag,
	const _Ty&,
	ptrdiff_t>
{
	typename std::vector<std::pair<ptrdiff_t, _Ty>>::const_iterator iter;
public:

	WeightedVectorConstIterator()
	{
	}

	WeightedVectorConstIterator(const decltype(iter)& iter)
	{
		this->iter = iter;
	}

private:
	friend class boost::iterator_core_access;

	const _Ty& dereference() const { return iter->second; }
	bool equal(WeightedVectorConstIterator<_Ty> const& other) const { return this->iter == other.iter; }
	void increment() { ++iter; }
	void decrement() { --iter; }
	void advance(ptrdiff_t n) { iter += n; }
	ptrdiff_t distance_to(WeightedVectorConstIterator<_Ty> const& other) const { return other.iter - this->iter; }
};

template <class _Ty>
class WeightedVector
{
	std::vector<std::pair<ptrdiff_t, _Ty>> vec_;
	ptrdiff_t totalWeight_ = 0;
public:

	using iterator = WeightedVectorIterator<_Ty>;
	using const_iterator = WeightedVectorConstIterator<_Ty>;

	iterator begin()
	{
		return iterator(vec_.begin());
	}

	iterator end()
	{
		return iterator(vec_.end());
	}

	const_iterator cbegin() const
	{
		return const_iterator(vec_.cbegin());
	}

	const_iterator cend() const
	{
		return const_iterator(vec_.cend());
	}

	const_iterator begin() const
	{
		return cbegin();
	}

	const_iterator end() const
	{
		return cend();
	}

	void push_back(const _Ty& elem, ptrdiff_t weight = 1)
	{
		totalWeight_ += weight;
		vec_.push_back(std::make_pair(weight, elem));
	}

	void push_back(_Ty&& elem, ptrdiff_t weight = 1)
	{
		totalWeight_ += weight;
		vec_.push_back(std::make_pair(weight, elem));
	}

	_Ty& getWeighted(ptrdiff_t weight)
	{
		if (weight >= totalWeight_ || weight < 0)
			throw std::exception("Weight out of range");

		ptrdiff_t cWeight = 0;
		for (auto& pair : vec_)
		{
			if (pair.first + cWeight > weight)
				return pair.second;
			cWeight += pair.first;
		}

		throw std::exception("Weight out of range");
	}

	_Ty& getIndexed(size_t index)
	{
		return vec_.at(index).second;
	}

	_Ty& operator[](size_t index)
	{
		return vec_[index].second;
	}

	const _Ty& operator[](size_t index) const
	{
		return vec_[index].second;
	}

	// Modify weight of the object pointed at by ptr.
	// Returns how much the weight actually changed by ( Weight can't be less than 0 )
	ptrdiff_t changeWeight(_Ty* ptr, ptrdiff_t diff)
	{
		for (auto& pair : vec_)
		{
			if (&pair.second == ptr)
			{
				ptrdiff_t aDiff = (-diff > pair.first) ? -pair.first : diff;
				totalWeight_ += aDiff;
				pair.first += aDiff;
				return aDiff;
			}
		}
		return -1;
	}

	ptrdiff_t setWeight(_Ty* ptr, ptrdiff_t newWeight)
	{
		for (auto& pair : vec_)
		{
			if (&pair.second == ptr)
			{
				auto oldWeight = pair.first;
				totalWeight_ -= (pair.first - newWeight);
				pair.first = newWeight;
				return oldWeight;
			}
		}
		return -1;
	}

	void addToAllWeights(ptrdiff_t diff)
	{
		for (auto& pair : vec_)
		{
			if (-diff > pair.first)
			{
				totalWeight_ -= pair.first;
				pair.first = 0;
				continue;
			}
			pair.first += diff;
			totalWeight_ += diff;
		}
	}

	template <typename _Eng>
	void addToRandomWeights(ptrdiff_t distWeight, _Eng& engine)
	{
		if (distWeight < 0)
			throw std::exception("distWeight must be zero or greater");

		std::uniform_int_distribution<size_t> dist(0, vec_.size() - 1);
		for (ptrdiff_t n = 0; n < distWeight; n++)
		{
			auto& pair = vec_[dist(engine)];
			++pair.first;
		}
		totalWeight_ += distWeight;
	}

	void distributeWeight(ptrdiff_t distWeight)
	{
		if (size() == 0 || distWeight < 0)
			return;

		ptrdiff_t add = distWeight / (ptrdiff_t)size();
		ptrdiff_t remain = distWeight % (ptrdiff_t)size();

		for (auto &pair : vec_)
		{
			pair.first += add;

			if (remain > 0)
			{
				pair.first += 1;
				--remain;
			}
		}

		totalWeight_ += distWeight;
	}

	size_t size() const
	{
		return vec_.size();
	}

	ptrdiff_t totalWeight() const
	{
		return totalWeight_;
	}

	size_t reachable() const
	{
		size_t count = 0;
		for (const auto& pair : vec_)
			if (pair.first > 0)
				count++;

		return count;
	}

	WeightedVector(const std::vector<_Ty>& other, ptrdiff_t initWeight = 1)
	{
		for (const auto& item : other)
			vec_.emplace_back(initWeight, item);

		totalWeight_ = initWeight * (ptrdiff_t)other.size();
	}

	WeightedVector() = default;
	~WeightedVector() = default;
};

