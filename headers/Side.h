#pragma once
#include <ios>
#include <iomanip>
#include "Axis.h"
#include "KeyVal.h"
#include "LinkedList.h"
#include "Plane.h"

#ifndef TABDEPTH
#define TABDEPTH 1U
#endif

class BoundingBox;

class Side
{
private:
	unsigned int id_;
	mutable unsigned int depth_;
	Plane p;
	Axis uaxis, vaxis;
public:
	LinkedList<KeyVal> keyvals;

	unsigned int parse(std::istream &);

	unsigned int id() const {return id_;}
	unsigned int depth() const {return depth_;}
	void depth(unsigned int newDepth) const {depth_ = newDepth;}

	BoundingBox bbox() const;
	
	void popuvars();
	void rotate(const Vertex &, const Matrix &);
	void move(const Vector &);
	void reID(unsigned int *);

	static bool testCollision(const Side &, const Side &);

	friend std::ostream &operator<<(std::ostream&, const Side &s);

	Side(void);
	~Side(void);
};

inline std::ostream &operator<<(std::ostream &os, const Side &s) {
	os<<std::setw(s.depth())<<""<<"side\n";
	os<<std::setw(s.depth())<<""<<"{\n";
	os<<std::setw(s.depth())<<""<<"\t\"id\" \""<<s.id_<<"\"\n";
	os<<std::setw(s.depth())<<""<<"\t\"Plane\" \""<<s.p.toStr()<<"\"\n";
	os<<std::setw(s.depth())<<""<<"\t\"uaxis\" \""<<s.uaxis.toStr()<<"\"\n";
	os<<std::setw(s.depth())<<""<<"\t\"vaxis\" \""<<s.vaxis.toStr()<<"\"\n";
	for(const KeyVal &kv : s.keyvals) {
		os<<std::setw(s.depth())<<""<<"\t"<<kv.toStr()<<"\n";
	}
	os<<std::setw(s.depth())<<""<<"}";
	return os;
}
