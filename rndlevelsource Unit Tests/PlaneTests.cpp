#include "stdafx.h"
#include "CppUnitTest.h"
#include "Plane.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace rndlevelsourceUnitTests
{
	TEST_CLASS(PlaneTests)
	{
		const Plane testPlaneZ = { {0, 0, 0}, {0, 64, 0}, {64, 64, 0} };
		const Plane testPlaneX = { {0, 64, 64}, {0, 64, 0}, {0, 0, 0} };
	public:

		TEST_METHOD(TestPlaneFlip)
		{
			Plane flippedPlane = Plane::flip(testPlaneZ);
			Assert::AreEqual(testPlaneZ.p3, flippedPlane.p1);
		}

		TEST_METHOD(TestPlaneNormal)
		{
			Assert::AreEqual({ 0, 0, 1 }, testPlaneZ.normal());
			Assert::AreEqual({ 0, 0, -1 }, Plane::flip(testPlaneZ).normal());
			Assert::AreEqual({ 1, 0, 0 }, testPlaneX.normal());
		}

		TEST_METHOD(TestPlaneClosestAxisToNormal)
		{
			Assert::AreEqual({ 0, 0, 1 }, testPlaneZ.closestAxisToNormal());
			Assert::AreEqual({ 0, 0, 1 }, Plane::flip(testPlaneZ).closestAxisToNormal());
			Assert::AreEqual({ 1, 0, 0 }, testPlaneX.closestAxisToNormal());
		}

		TEST_METHOD(TestPlaneEquation)
		{
			Assert::AreEqual({ 0, 0, 1, 0 }, testPlaneZ.equation());
			Vertex translation = { 0, 0, 64 };
			Plane plane = {
				testPlaneZ.p1 + translation,
				testPlaneZ.p2 + translation,
				testPlaneZ.p3 + translation
			};
			Assert::AreEqual({ 0, 0, 1, -64 }, plane.equation());
		}

		TEST_METHOD(TestPlaneEvaluate)
		{
			Vertex overVertex = { 0, 0, 64 };
			Vertex onVertex = { 0, 0, 0 };
			Vertex underVertex = { 0, 0, -64 };
			Assert::AreEqual(char(1), testPlaneZ.evaluate(overVertex));
			Assert::AreEqual(char(-1), testPlaneZ.evaluate(underVertex));
			Assert::AreEqual(char(0), testPlaneZ.evaluate(onVertex));
		}

		TEST_METHOD(TestPlaneParse)
		{
			constexpr char * parseStr = "(32 32 32) (0.5 -0.5 3) (0 0 0)";

			Plane p{ parseStr };

			Assert::AreEqual({ 32, 32, 32 }, p.p1);
			Assert::AreEqual({ 0.5, -0.5, 3 }, p.p2);
			Assert::AreEqual({ 0, 0, 0 }, p.p3);
		}
	};
}
