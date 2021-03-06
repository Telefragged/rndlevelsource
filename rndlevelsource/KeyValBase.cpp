#include "KeyValBase.h"

#include <iostream>
#include <array>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/range/algorithm.hpp>

#include "utils.h"

size_t KeyValBase::depth() const
{
	return depth_;
}

void KeyValBase::depth(size_t depth) const
{
	depth_ = depth;
}

std::string_view KeyValBase::operator[](std::string_view key) const
{
	auto it = boost::find_if(keyvals, [&key](const auto& p) { return boost::iequals(p.first, key); });

	if (it != keyvals.end())
		return it->second;

	return std::string_view();
}

std::string& KeyValBase::operator[](std::string_view key)
{
	auto it = boost::find_if(keyvals, [&key](const auto& p) { return boost::iequals(p.first, key); });
	if (it != keyvals.end())
		return it->second;
	
	auto[constructedIt, success] = keyvals.emplace(key, "");

	if(success)
		return constructedIt->second;

	throw std::exception("Failed to insert key");
}

std::string_view KeyValBase::get(std::string_view key) const
{
	auto it = boost::find_if(keyvals, [&key](const auto& p) { return boost::iequals(p.first, key); });

	if (it != keyvals.end())
		return it->second;

	return std::string_view();
}

bool KeyValBase::hasKey(std::string_view key) const
{
	auto it = boost::find_if(keyvals, [&key](const auto& p) { return boost::iequals(p.first, key); });

	return it != keyvals.end();
}

std::pair<std::string, std::string> KeyValBase::parseKeyval(std::string_view keyval)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	
	auto trimmed = trim(keyval);

	//auto fsplit = trimmed.find_first_of("\"", 1);
	//if (fsplit > trimmed.length())
	//	fsplit = trimmed.length();

	//auto left = trimmed.substr(0, fsplit);
	//left = trim(left, "\"");

	//if (fsplit == trimmed.length())
	//	return std::make_pair("", "");

	//auto esplit = trimmed.find_first_of("\"", fsplit + 1);
	//if (esplit == 0 || esplit >= trimmed.length())
	//	esplit = fsplit;

	//auto right = trimmed.substr(esplit + 1);
	//right = trim(right, "\"");

	//std::string key{ right }, value{ left };

	std::pair<std::string, std::string> ret;

	if (!qi::parse(trimmed.cbegin(), trimmed.cend(), '"' >> +(qi::char_ - '"') >> '"' >> ' ' >> '"' >> +(qi::char_ - '"') >> '"', ret))
		throw std::exception("Failed to parse keyval");

	return ret;
}

std::string KeyValBase::toStr(const std::pair<std::string, std::string>& pair, char enclose)
{
	std::string ret;

	ret.reserve(pair.first.size() + pair.second.size() + 6);

	ret.push_back(enclose);
	boost::copy(pair.first, std::back_inserter(ret));
	ret.push_back(enclose);

	ret.push_back(' ');

	ret.push_back(enclose);
	boost::copy(pair.second, std::back_inserter(ret));
	ret.push_back(enclose);

	return ret;
	//return enclose + pair.first + enclose + ' ' + enclose + pair.second + enclose;
}

size_t KeyValBase::parse(std::istream& stream)
{
	size_t numparsed = 0;
	std::string curline;

	while (trim(curline) != "{")
	{
		getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;

	while (getline(stream, curline))
	{
		numparsed++;

		auto trimmedCurline = trim(curline);

		if (trimmedCurline == "}")
		{
			if (--depth == 0)
				break;
		}
		else if (trimmedCurline == "{")
		{
			++depth;
		}
		else if (trimmedCurline.find_first_of('"') != std::string::npos)
		{
			auto [k, v] = parseKeyval(curline);
			keyvals[k] = std::move(v);
		}
		else
		{
			numparsed += parseSpecial(stream, trimmedCurline);
		}
	}
	if (auto it = keyvals.find("id"); it != keyvals.end())
	{
		this->id_ = atoi(it->second.c_str());
		keyvals.erase("id");
	}
	return numparsed;
}

size_t KeyValBase::parseSpecial(std::istream & stream, std::string_view type)
{
	return 0;
}

bool KeyValBase::empty() const
{
	return keyvals.empty();
}

void KeyValBase::extraOutput(std::ostream & os) const
{
}

KeyValBase::KeyValBase(std::initializer_list<decltype(keyvals)::value_type> init)
	: keyvals(init)
{
}
