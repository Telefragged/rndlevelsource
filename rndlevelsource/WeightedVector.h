#pragma once

#include <vector>
#include <random>
#include <cstddef>
#include <iostream>
#include <type_traits>

#include <boost/iterator_adaptors.hpp>
#include <boost/range/adaptors.hpp>

template <class _Ty, class _WeightTy>
class WeightedVectorIterator
	: public boost::iterator_facade<
		WeightedVectorIterator<_Ty, _WeightTy>,
		_Ty,
		boost::random_access_traversal_tag,
		_Ty&,
		ptrdiff_t>
{
private:
	using _vectorIteratorType = typename std::conditional<
		std::is_const<_Ty>::value,
		typename std::vector<std::pair<_WeightTy, typename std::remove_const<_Ty>::type>>::const_iterator,
		typename std::vector<std::pair<_WeightTy, _Ty>>::iterator>
		::type;

	_vectorIteratorType iter;

public:

	WeightedVectorIterator() = default;

	WeightedVectorIterator(_vectorIteratorType iter)
		: iter(iter)
	{
	}

	template <class _Ty2, class = typename std::enable_if<
		   std::is_convertible_v<_Ty2*, _Ty*>>::type>
	WeightedVectorIterator(const WeightedVectorIterator<_Ty2, _WeightTy>& other )
		: iter(other.iter)
	{
	}

private:
	friend class boost::iterator_core_access;
	template<class _Ty, class _WeightTy> friend class WeightedVector;

	_Ty& dereference() const { return iter->second; }

	template <class _Ty2>
	bool equal(WeightedVectorIterator<_Ty2, _WeightTy> const& other) const { return this->iter == other.iter; }

	void increment() { ++iter; }
	void decrement() { --iter; }
	void advance(ptrdiff_t n) { iter += n; }
	ptrdiff_t distance_to(WeightedVectorIterator<_Ty, _WeightTy> const& other) const { return other.iter - this->iter; }
};

template <class _Ty, class _WeightTy = ptrdiff_t>
class WeightedVector
{
	std::vector<std::pair<_WeightTy, _Ty>> vec_;
	_WeightTy totalWeight_ = 0;
public:
	using value_type = _Ty;
	using reference = _Ty &;
	using pointer = _Ty *;

	using iterator = WeightedVectorIterator<_Ty, _WeightTy>;
	using const_iterator = WeightedVectorIterator<const _Ty, _WeightTy>;
	using difference_type = ptrdiff_t;

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

	auto reachableRange()
	{
		return vec_ | boost::adaptors::filtered([](auto& pair) { return pair.first > _WeightTy{ 0 }; })
					| boost::adaptors::transformed([](auto& pair) -> reference { return pair.second; });
	}

	void push_back(const _Ty& elem, _WeightTy weight = 1)
	{
		totalWeight_ += weight;
		vec_.push_back(std::make_pair(weight, elem));
	}

	void push_back(_Ty&& elem, _WeightTy weight = 1)
	{
		totalWeight_ += weight;
		vec_.push_back(std::make_pair(weight, std::forward<_Ty>(elem)));
	}

	iterator erase(iterator first, iterator last)
	{
		for (auto it = first.iter; it != last.iter; it++)
			totalWeight_ -= it->first;

		return iterator(vec_.erase(first.iter, last.iter));
	}

	iterator getWeightedIter(_WeightTy weight)
	{
		if (weight >= totalWeight_ || weight < 0)
			throw std::exception("Weight out of range");

		_WeightTy cWeight = 0;
		for (size_t n = 0; n < vec_.size(); n++)
		{
			auto& pair = vec_[n];
			if (pair.first + cWeight > weight)
				return iterator(vec_.begin() + n);
			cWeight += pair.first;
		}

		throw std::exception("Weight out of range");
	}

	_Ty& getWeighted(_WeightTy weight)
	{
		return *getWeightedIter(weight);
	}

	template<class _Eng, typename = typename std::enable_if<std::is_integral<_WeightTy>::value>::type>
	iterator getWeightedAndRedistribute(_WeightTy weight, _Eng engine)
	{
		if (weight >= totalWeight_ || weight < 0)
			throw std::exception("Weight out of range");

		if (vec_.size() < 2)
			return getWeightedIter(weight);

		_WeightTy cWeight = 0;
		for (size_t n = 0; n < vec_.size(); n++)
		{
			auto &pair = vec_[n];

			if (pair.first + cWeight > weight)
			{
				_WeightTy redistWeight = pair.first;

				pair.first = 0;

				totalWeight_ -= redistWeight;

				addToRandomWeights(redistWeight, engine, n);

				return iterator(vec_.begin() + n);
			}
			cWeight += pair.first;
		}

		throw std::exception("Weight out of range");
	}

	template <class _Eng>
	iterator getRandomWeightedIter(_Eng eng)
	{
		if constexpr (std::is_integral_v<_WeightTy>)
		{
			std::uniform_int_distribution<_WeightTy> dist(0, totalWeight_ - 1);
			return getWeightedIter(dist(eng));
		}

		return end();
	}

	_Ty& at(size_t index)
	{
		return vec_.at(index).second;
	}

	const _Ty& at(size_t index) const
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
	_WeightTy changeWeight(iterator iter, _WeightTy diff)
	{
		_WeightTy aDiff = (-diff > iter.iter->first) ? -iter.iter->first : diff;
		totalWeight_ += aDiff;
		iter.iter->first += aDiff;
		return aDiff;
	}

	_WeightTy setWeight(iterator iter, _WeightTy newWeight)
	{
		auto oldWeight = iter.iter->first;
		totalWeight_ -= (iter.iter->first - newWeight);
		iter.iter->first = newWeight;
		return oldWeight;
	}

	void addToAllWeights(_WeightTy diff)
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

	template <typename _Eng, typename = typename std::enable_if<std::is_integral<_WeightTy>::value>::type>
	void addToRandomWeights(_WeightTy distWeight, _Eng& engine, size_t skipIndex = std::numeric_limits<size_t>::max())
	{
		if (distWeight < 0)
			throw std::exception("distWeight must be zero or greater");

		std::uniform_int_distribution<size_t> dist(0, vec_.size() - (skipIndex >= vec_.size() ? 1 : 2));
		for (_WeightTy n = 0; n < distWeight; n++)
		{
			size_t index = dist(engine);
			if (index >= skipIndex)
				++index;
			auto& pair = vec_[index];
			++pair.first;
		}
		totalWeight_ += distWeight;
	}

	void distributeWeight(_WeightTy distWeight)
	{
		if (size() == 0 || distWeight < 0)
			return;

		if constexpr (std::is_integral<_WeightTy>::value)
		{

			_WeightTy add = distWeight / (_WeightTy)size();
			_WeightTy remain = distWeight % (_WeightTy)size();

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
		else if constexpr(std::is_floating_point<_WeightTy>::value)
		{
			_WeightTy add = distWeight / static_cast<_WeightTy>(size());

			for (auto &pair : vec_)
			{
				pair.first += add;
				totalWeight_ += add;
			}
		}
	}

	size_t size() const
	{
		return vec_.size();
	}

	_WeightTy totalWeight() const
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

	WeightedVector(const std::vector<_Ty>& other, _WeightTy initWeight = 1)
	{
		for (const auto& item : other)
			vec_.emplace_back(initWeight, item);

		totalWeight_ = initWeight * (_WeightTy)other.size();
	}

	WeightedVector() = default;
	~WeightedVector() = default;
};
