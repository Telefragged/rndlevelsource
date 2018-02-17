#include "Connection.h"

#include "KeyVal.h"

Connection::Connection(Entity&& entity) : isConnected(false)
{
	this->solids = std::move(entity.solids);
	this->keyvals = std::move(entity.keyvals);
	if (keyvals.count("classname") > 0)
	{
		std::string val = keyvals["classname"];
		if (val.length() > 12)
			connectstr = val.substr(12);
	}
}

Connection::~Connection(void)
{
}

