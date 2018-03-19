#include "PartInfo.h"
#include "Entity.h"

#include <boost/algorithm/string/predicate.hpp>

PartInfo::PartInfo(const Entity &entity)
{
	if (auto str = entity.get("Scaleable"); boost::iequals(str, "true"))
		scaleable = true;

	if (auto str = entity.get("CanBeRoot"); boost::iequals(str, "true"))
		canBeRoot = true;
}
