#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <set>
#include <list>
#include <deque>
#include <set>
#include <unordered_map>

#include "CacheLine.h"

class Cache
{
public:
	Cache();
	virtual ~Cache(){}
	virtual State findTag(uint64_t set,uint64_t tag) const = 0;
	virtual void changeState(uint64_t set,uint64_t tag,State state)=0;
	virtual void updateLRU(uint64_t set,uint64_t tag) = 0;
	virtual bool checkWriteBack(uint64_t set,uint64_t tag) const =0;
	virtual void insertCacheLine(uint64_t set,uint64_t tag, State state) = 0;
	
};

class SetCache : public Cache{
	std::vector<std::set<CacheLine> > sets;
	std::vector<std::list<uint64_t> > lruLists;
	std::vector<std::unordered_map<uint64_t,std::list<uint64_t>::iterator> > lruMaps;

public:
	SetCache(unsigned int numLines, unsigned int associativity);
	 State findTag(uint64_t set,uint64_t tag) const;
	 void changeState(uint64_t set,uint64_t tag,State state)=0;
	 void updateLRU(uint64_t set,uint64_t tag);
	 bool checkWriteBack(uint64_t set,uint64_t tag) const;
	 void insertCacheLine(uint64_t set,uint64_t tag, State state);


};


#endif
