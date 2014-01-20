#pragma once
#include <ios>
#include "Editor.h"
#include "LinkedList.h"
#include "Side.h"

class BoundingBox;
class Matrix;
class Vector;
class Vertex;

class Solid
{
private:
	LinkedList<Side> sides;
	unsigned int id_;
	mutable unsigned int depth_;
	Editor edt;
public:

	static bool testCollision(const Solid &, const Solid &);

	unsigned int parse(std::istream &);
	void rotate(const Vertex &, const Matrix &);
	void move(const Vector &);
	void reID(unsigned int *, unsigned int *);

	unsigned int depth() const {return depth_;}
	void depth(unsigned int newDepth) const {depth_ = newDepth;}

	BoundingBox bbox() const;

	friend std::ostream &operator<<(std::ostream &, const Solid &);

	Solid(std::istream &);
	Solid(void);
	~Solid(void);
};

inline std::ostream &operator<<(std::ostream &os,const Solid &s) {
	os<<std::setw(s.depth())<<""<<"solid\n";
	os<<std::setw(s.depth())<<""<<"{\n";
	os<<std::setw(s.depth())<<""<<"\t\"id\" \""<<s.id_<<"\"\n";
	for(const Side &sd : s.sides) {
		sd.depth(s.depth() + TABDEPTH);
		os<<sd<<"\n";
	}
	s.edt.depth(s.depth() + TABDEPTH);
	os<<s.edt<<"\n";
	os<<std::setw(s.depth())<<""<<"}";
	return os;
}
