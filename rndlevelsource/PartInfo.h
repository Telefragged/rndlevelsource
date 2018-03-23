#pragma once

class Entity;

class PartInfo
{
public:
	bool scaleable = false;
	bool canBeRoot = false;

	PartInfo(const Entity &entity);

	PartInfo() = default;
};
