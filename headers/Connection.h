#pragma once
#include "Entity.h"

class Connection :
	public Entity
{
public:
	std::string connectstr;

	//Only move constructor for now.
	Connection(Entity &&);
	~Connection(void);
};

