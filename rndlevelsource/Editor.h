#pragma once
#include <iostream>
#include <iomanip>
#include <vector>

#include "KeyVal.h"
#include "KeyValBase.h"

class Editor :
	public KeyValBase
{
private:
public:
	unsigned int parse(std::istream& stream);

	std::string getName() const override;

	Editor() = default;
	~Editor() override = default;
};
