#pragma once
#include "Entity.h"
#include "WeightedVector.h"

class Part;

class Connection :
	public Entity
{
public:
	std::string connectstr;

	Part *parent = nullptr;
	Connection *neighbour = nullptr;

	void connectTo(Connection *targetCon);

	bool isConnected() const;

	//Only move constructor for now.
	Connection(Entity&& entity);
	~Connection() override = default;
};

