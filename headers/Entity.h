#pragma once
#include "Solid.h"
#include "KeyVal.h"
#include "LinkedList.h"
#include "Editor.h"

class Vertex;
class Angle;

class Entity
{
private:
	mutable unsigned int depth_;
	Editor edt;
public:
	unsigned int id_;
	LinkedList<Solid> solids;
	LinkedList<KeyVal> keyvals;

	Vertex origin();
	Vertex originKV();
	Angle angles();

	unsigned int depth() const {return depth_;}
	void depth(unsigned int newDepth) const {depth_ = newDepth;}

	unsigned int parse(std::istream &);

	static bool testCollision(const Entity &, const Entity &);
	static bool entclasscmp(const Entity &, const std::string &);

	void rotate(const Matrix &, const Vertex &);
	void move(const Vector &);
	void mergeSolids(const Entity &);
	void reID(unsigned int *, unsigned int *, unsigned int *);

	BoundingBox bbox() const;

	std::string &operator[](std::string);
	std::string operator[](std::string) const;

	friend std::ostream &operator<<(std::ostream &, const Entity &);

	Entity(void);
	Entity(const std::string &);
	~Entity(void);
};

inline std::ostream &operator<<(std::ostream &os, const Entity &e) {
	if(e["classname"] == "world") {
		os<<std::setw(e.depth())<<""<<"world\n";
	} else {
		os<<std::setw(e.depth())<<""<<"entity\n";
	}
	os<<std::setw(e.depth())<<""<<"{\n";
	os<<std::setw(e.depth())<<""<<"\t\"id\" \""<<e.id_<<"\"\n";
	for(const KeyVal &k : e.keyvals) {
		if(k.keyc() == "classname" && k.valc() == "world") continue;
		os<<std::setw(e.depth())<<""<<"\t"<<k.toStr()<<"\n";
	}
	for(const Solid &solid : e.solids) {
		solid.depth(e.depth() + TABDEPTH);
		os<<solid<<"\n";
	}
	e.edt.depth(e.depth() + TABDEPTH);
	os<<e.edt<<"\n";
	os<<std::setw(e.depth())<<""<<"}";
	return os;
}
