#pragma once

#include "CppUnitTest.h"

#include <locale>
#include <codecvt>
#include <string>
#include <cstring>

#include "Matrix.h"
#include "Vertex.h"
#include "Plane.h"
#include "Polygon.h"

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
	inline static std::wstring str_to_wstr(std::string_view str)
	{
		std::mbtowc(NULL, 0, 0);

		std::wstring ret;

		auto beg = str.data(), end = str.data() + str.size();

		int result = 0;

		for (wchar_t wc; (result = std::mbtowc(&wc, beg, end - beg)) > 0; beg += result)
			ret += wc;

		return ret;
	}

	template<> inline static std::wstring ToString<Vertex>(const Vertex& v)
	{
		return str_to_wstr(v.toStr());
	}

	template<> inline static std::wstring ToString<Plane>(const Plane& p)
	{
		return str_to_wstr(p.toStr());
	}

	template<>
	inline static std::wstring ToString(const Polygon::classification &c)
	{
		switch (c)
		{
		case(Polygon::classification::back):
			return std::wstring(L"back");
			break;
		case(Polygon::classification::front):
			return std::wstring(L"front");
			break;
		case(Polygon::classification::onPlane):
			return std::wstring(L"onPlane");
			break;
		case(Polygon::classification::spanning):
			return std::wstring(L"spanning");
			break;
		}
		return std::wstring();
	}

	template<class _Ty, size_t _X, size_t _Y>
	inline static std::wstring ToString(const Matrix<_Ty, _X, _Y> &mat)
	{
		//std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		//return converter.from_bytes(mat.toStr());

		return str_to_wstr(mat.toStr());
	}
}

