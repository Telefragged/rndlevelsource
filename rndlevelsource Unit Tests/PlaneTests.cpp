#include "stdafx.h"
#include "CppUnitTest.h"
#include "Plane.h"
#include "utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace rndlevelsourceUnitTests
{		
	TEST_CLASS(PlaneTests)
	{
	public:
		
		TEST_METHOD(TestPlaneFlip)
		{
			Plane plane({ 0, 0, 0 }, { 0, 64, 0 }, { 64, 64, 0 });
			Plane flippedPlane = Plane::flip(plane);
			Assert::AreEqual(flippedPlane.p1, plane.p3);
		}

		TEST_METHOD(TestPlaneNormal)
		{
			Plane plane({ 0, 0, 0 }, { 0, 64, 0 }, { 64, 64, 0 });
			Assert::AreEqual(plane.normal(), { 0, 0, 1 });
			Assert::AreEqual(Plane::flip(plane).normal(), { 0, 0, -1 });
		}

	};
}