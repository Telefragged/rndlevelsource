#include "stdafx.h"
#include "CppUnitTest.h"
#include "WeightedVector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace rndlevelsourceUnitTests
{
	TEST_CLASS(WeightedVectorTest)
	{
	public:

		TEST_METHOD(TestWeightedVectorPushBack)
		{
			WeightedVector<size_t> vector;
			vector.push_back(1, 3);

			Assert::AreEqual(size_t{ 1 }, vector.size());
			Assert::AreEqual(ptrdiff_t{ 3 }, vector.totalWeight());

			vector.push_back(2, 2);

			Assert::AreEqual(size_t{ 2 }, vector.size());
			Assert::AreEqual(ptrdiff_t{ 5 }, vector.totalWeight());
		}

		TEST_METHOD(TestWeightedVectorGetWeighted)
		{
			WeightedVector<size_t> vector;
			vector.push_back(1);
			vector.push_back(2, 2);
			vector.push_back(3);

			Assert::AreEqual(size_t{ 1 }, vector.getWeighted(0));
			Assert::AreEqual(size_t{ 2 }, vector.getWeighted(2));
			Assert::AreEqual(size_t{ 3 }, vector.getWeighted(3));
		}

		TEST_METHOD(TestWeightedVectorErase)
		{
			WeightedVector<size_t> vector;

			vector.push_back(1);
			vector.push_back(2, 2);
			vector.push_back(3, 3);
			vector.push_back(4, 4);

			vector.erase(vector.begin() + 1, vector.begin() + 3);

			Assert::AreEqual(size_t{ 4 }, vector[1]);
			Assert::AreEqual(size_t{ 2 }, vector.size());
			Assert::AreEqual(ptrdiff_t{ 5 }, vector.totalWeight());
		}
	};
}
