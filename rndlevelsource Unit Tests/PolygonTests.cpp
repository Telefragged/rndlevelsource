#include "stdafx.h"

#include "CppUnitTest.h"
#include "Polygon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace rndlevelsourceUnitTests
{
	TEST_CLASS(PolygonTests)
	{
		const Polygon TestPolygonUp{ {0, 0, 0}, {0, 64, 0}, {64, 64, 0}, {64, 0, 0} };
		const Plane TestPlaneUp{ {0, 0, 0}, {0, 64, 0}, {64, 64, 0} };
	public:

		TEST_METHOD(TestPolygonOrigin)
		{
			Assert::AreEqual({ 32, 32, 0 }, TestPolygonUp.origin());

			Polygon polygon = TestPolygonUp;

			polygon.moveTo({ 0, 0, 32 });
			Assert::AreEqual({ 0, 0, 32 }, polygon.origin());
		}

		TEST_METHOD(TestPolygonClassify)
		{
			Assert::AreEqual(Polygon::classification::onPlane, TestPolygonUp.classify(TestPlaneUp));
			Polygon polygon = TestPolygonUp;
			polygon.rotate(polygon.origin(), Matrix3d::rotmatx(90));
			Assert::AreEqual(Polygon::classification::spanning, polygon.classify(TestPlaneUp));
		}

		TEST_METHOD(TestPolygonSliceOnPlane)
		{
			auto [back, front] = TestPolygonUp.slice(TestPlaneUp);

			Assert::AreEqual(size_t{ 0 }, front.points.size());
			Assert::AreEqual(size_t{ 0 }, back.points.size());
		}

		TEST_METHOD(TestPolygonMoveTo)
		{
			Polygon polygon = TestPolygonUp;
			polygon.moveTo({ 0, 0, 0 });

			Assert::AreEqual({ 0, 0, 0 }, polygon.origin());
			Vertex p0{ 4, 13, 85 }, p1{ 10, -10, 40 };

			polygon.moveTo(p0);
			Assert::AreEqual(p0, polygon.origin());

			polygon.moveTo(p1);
			Assert::AreEqual(p1, polygon.origin());
		}
		
		TEST_METHOD(TestPolygonRotate)
		{
			Polygon polygon = TestPolygonUp;
			polygon.moveTo({ 0, 0, 0 });
			polygon.rotate(polygon.origin(), Matrix3d::rotmatx(90));

			Assert::AreEqual({ 0, 0, 0 }, polygon.origin());
		}

		TEST_METHOD(TestPolygonSlice)
		{
			Polygon polygon = TestPolygonUp;
			polygon.rotate(polygon.origin(), Matrix3d::rotmatx(90));

			auto [back, front] = polygon.slice(TestPlaneUp);

			Assert::AreEqual(size_t{ 4 }, back.points.size());
			Assert::AreEqual(size_t{ 4 }, front.points.size());
		}
	};
}
