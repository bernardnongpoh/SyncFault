#ifndef CACHE_H
#define CACHE_H
#include <cassert>
#include <vector>
#include <set>
#include <list>
#include <deque>
#include <set>
#include <unordered_map>
#include "pin.H"
#include "CacheLine.h"

/* This cache represent set associative cache */


class Cache
{

private:
	std::vector<std::set<CacheLine> > sets;
	std::vector<std::list<ADDRINT> > lruLists;
	std::vector<std::unordered_map<ADDRINT,std::list<ADDRINT>::iterator> > lruMaps;

public:
	Cache(unsigned int numLines,unsigned int associativity);
	
	State findTag(ADDRINT set,ADDRINT tag) const;
	void changeState(ADDRINT set,ADDRINT tag,State state);
	void updateLRU(ADDRINT set,ADDRINT tag);
	bool checkWriteBack(ADDRINT set,ADDRINT tag) const;
	void insertCacheLine(ADDRINT set,ADDRINT tag, State state);
	
};
#endif