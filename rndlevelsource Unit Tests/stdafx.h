// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
#include <locale>
#include <codecvt>
#include <string>
#include "Vertex.h"
#include "Plane.h"

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> inline static std::wstring ToString<Vertex>(const Vertex& v)
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				return converter.from_bytes(v.toStr());
			}

			template<> inline static std::wstring ToString<Plane>(const Plane& p)
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				return converter.from_bytes(p.toStr());
			}
		}
	}
}
