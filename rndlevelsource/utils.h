#pragma once
#include <limits>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

inline std::string nextword(size_t& pos, const std::string& str, std::string delim = " \t\n\r")
{
	if (pos >= str.length()) return "";
	size_t spos = pos;
	size_t to = str.find_first_of(delim, pos);
	if (to == std::string::npos) to = str.length();
	pos = to + 1;
	return str.substr(spos, to - spos);
}

inline std::string trim(const std::string& str, const std::string delim = " \t\n\r")
{
	size_t bpos = str.find_first_not_of(delim), epos = str.find_last_not_of(delim);
	if (epos < bpos || epos >= str.length()) return "";
	return str.substr(bpos, epos - bpos + 1);
}

inline std::string reverse_string(const std::string& input)
{
	return std::string(input.crbegin(), input.crend());
}

inline bool doubleeq(double lhs, double rhs, double epsilon = 1e-6)
{
	return fabs(lhs - rhs) < epsilon;
}

template <typename T>
inline std::string formatNum(T num, bool printDec = true, char sep = ',')
{
	std::ostringstream os;
	os << std::fixed;
	os.precision(2);
	os << num;
	std::string unpretty = os.str(), pretty;
	bool neg = false;
	if (unpretty.at(0) == '-')
	{
		neg = true;
		unpretty = unpretty.substr(1, unpretty.length() - 1);
	}
	unsigned int n = 0;
	while (n < unpretty.length())
	{
		char c = unpretty.at(n);
		if (c == '.') break;
		n++;
	}
	n--;
	unsigned int j = n, i = 0;
	while (j != 0xffffffff)
	{
		if (i == 3)
		{
			pretty += sep;
			i = 0;
		}
		i++;
		pretty += unpretty.at(j--);
	}
	pretty = reverse_string(pretty);
	if (neg) pretty = "-" + pretty;
	if (n < unpretty.length() && printDec) for (j = n + 1; j < unpretty.length(); j++) pretty += unpretty.at(j);
	return pretty;
}

template <typename T>
inline std::string numToPretty(T num, bool onlyPretty = false, bool appTxt = true, bool printDec = true, char sep = ',')
{
	if (onlyPretty) appTxt = true;
	std::string pretty;
	if (!onlyPretty) pretty = formatNum(num, printDec);
	if (appTxt)
	{
		std::string appStr;
		const char* suffixes[] = {"nanosecs", "microsecs", "milliseconds", "seconds", "minutes", "hours", "days", "years", "decades", "centuries", "milenniums"};
		const double div[] = {1000.0, 1000.0, 1000.0, 60.0, 60.0, 24.0, 365.24, 10.0, 10.0, 10.0, std::numeric_limits<double>::max()};
		std::ostringstream os;
		os.precision(3);
		os << std::fixed;
		double appNum = num;
		int sufPos = 0;
		if (appNum > 0.0)
		{
			while (appNum >= div[sufPos])
			{
				appNum /= div[sufPos];
				sufPos++;
			}
		}
		else
		{
			while (appNum <= -div[sufPos])
			{
				appNum /= div[sufPos];
				sufPos++;
			}
		}
		if (sufPos)
		{
			if (onlyPretty) os << appNum << " " << suffixes[sufPos];
			else os << " (" << appNum << " " << suffixes[sufPos] << ")";
			appStr = os.str();
			if (onlyPretty) pretty = appStr;
			else pretty += appStr;
		}
	}
	return pretty;
}

inline std::vector<std::string> splitstr(const std::string &orig, std::string delim = " \n\r\t")
{
	size_t pos = 0;
	std::vector<std::string> ret;
	while (pos < orig.length())
	{
		ret.push_back(nextword(pos, orig, delim));
	}
	return ret;
}

