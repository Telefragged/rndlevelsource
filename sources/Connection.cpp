#include "stdafx.h"
#include "Connection.h"
#include "KeyVal.h"

Connection::Connection(Entity &&entity)
{
	this->solids = std::move(entity.solids);
	this->keyvals = std::move(entity.keyvals);
	auto kv = keyvals.get_first_match<std::string>("classname", KeyVal::keycmp);
	if(kv != nullptr && kv->val.length() > 12) connectstr = kv->val.substr(12);
}

Connection::~Connection(void)
{
}
