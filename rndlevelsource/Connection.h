#pragma once
#include "Entity.h"

class Connection :
	public Entity
{
public:
	std::string connectstr;
	bool isConnected;

	//Only move constructor for now.
	Connection(Entity&&);
	~Connection();
};

