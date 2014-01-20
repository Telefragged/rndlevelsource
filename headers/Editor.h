#pragma once
#include <iostream>
#include <iomanip>
#include "KeyVal.h"
#include "LinkedList.h"

class Editor
{
private:
	mutable unsigned int depth_;
	LinkedList<KeyVal> keyvals;
public:
	unsigned int parse(std::istream &);

	unsigned int depth() const {return depth_;}
	void depth(unsigned int newDepth) const {depth_ = newDepth;}

	friend std::ostream &operator<<(std::ostream &, const Editor &);

	Editor(void);
	~Editor(void);
};

inline std::ostream &operator<<(std::ostream &os, const Editor &e) {
	if(e.keyvals.size() == 0U) return os;
	os<<std::setw(e.depth())<<""<<"Editor\n";
	os<<std::setw(e.depth())<<""<<"{\n";
	for(const KeyVal &kv : e.keyvals) {
		os<<std::setw(e.depth())<<""<<"\t"<<kv.toStr()<<"\n";
	}
	os<<std::setw(e.depth())<<""<<"}";
	return os;
}
