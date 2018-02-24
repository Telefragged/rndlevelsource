#include "stdafx.h"
#include "CppUnitTest.h"
#include "Solid.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace rndlevelsourceUnitTests
{
	TEST_CLASS(SolidTests)
	{
		// A solid stretching from {0, 0, 0} to {128, 128, 128}
		// Created from intersecting planes
		const Solid TestSolid = {
			{{0, 128, 128}, {128, 128, 128}, {128, 0, 128}},
			{{0, 0, 0}, {128, 0, 0}, {128, 128, 0}},
			{{0, 128, 128}, {0, 0, 128}, {0, 0, 0}},
			{{128, 128, 0}, {128, 0, 0}, {128, 0, 128}},
			{{128, 128, 128}, {0, 128, 128}, {0, 128, 0}},
			{{128, 0, 0}, {0, 0, 0}, {0, 0, 128}}
		};

	public:
		TEST_METHOD(TestSolidOrigin)
		{
			Assert::AreEqual({ 64, 64, 64 }, TestSolid.origin());

			Solid solid = TestSolid;

			solid.move({ -64, -64, -64 });

			Assert::AreEqual({ 0, 0, 0 }, solid.origin());
		}

		TEST_METHOD(TestSolidSlice)
		{
			Solid solid = TestSolid;

			Solid front, back;

			solid.slice({ { 0, 0, 64 }, { 0, 64, 64 }, { 64, 64, 64 } }, front, back);

			Assert::AreEqual(solid.sides.size(), front.sides.size());
			Assert::AreEqual(solid.sides.size(), back.sides.size());
		}
	};
}
