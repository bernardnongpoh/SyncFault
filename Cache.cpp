#include "Cache.h"

Cache::Cache(unsigned int numLines,unsigned int associativity){

	assert(numLines % associativity ==0);
	sets.resize(numLines/associativity);
	lruLists.resize(numLines/associativity);
	lruMaps.resize(numLines/associativity);

	for(unsigned int i=0;i<sets.size();i++){
		for(unsigned int j=0;j<associativity;j++){
			CacheLine temp;
			temp.setTag(j);
			temp.setState(INVALID);
			sets[i].insert(temp);
			lruLists[i].push_front(j);
			lruMaps[i].insert(make_pair(j,lruLists[i].begin()));

		}
	}
	

}
// TODO: FIXME Invalid verses Not found
State Cache::findTag(ADDRINT set, ADDRINT tag) const{
	CacheLine temp;
	temp.setTag(tag);
	std::set<CacheLine>::const_iterator it = sets[set].find(temp);
	if(it!=sets[set].end()){
		return it->getState();
	}

	return INVALID;
}

void Cache::changeState(ADDRINT set,ADDRINT tag,State state){
	CacheLine temp;
	temp.setTag(tag);
	// Find all cachelines in a Set
	std::set<CacheLine>::const_iterator it=sets[set].find(temp);

	if(it!=sets[set].end()){
		CacheLine *target;
		target=(CacheLine *) & *it;
		target->setState(state);
	}
}	
// A complete LRU is mantained for each set, using a separate
// list and map. The front of the list is considered most recently used.

void Cache::updateLRU(ADDRINT set,ADDRINT tag){
	//This will look for the tag in the set
	std::unordered_map<ADDRINT,std::list<ADDRINT>::iterator>::iterator map_it;
	std::list<ADDRINT>::iterator it;
	ADDRINT temp;
	map_it = lruMaps[set].find(tag);


	it = map_it->second;
	temp = *it;

	lruLists[set].erase(it);
	lruLists[set].push_front(temp);
	lruMaps[set].erase(map_it);
	lruMaps[set].insert(make_pair(tag,lruLists[set].begin()));

}

bool Cache::checkWriteBack(ADDRINT set,ADDRINT tag) const{
	CacheLine evict, temp;
   tag = lruLists[set].back();
   temp.setTag(tag);
   evict = *sets[set].find(temp);

   return (evict.getState() == MODIFIED || evict.getState() == OWNER);
	
}


void Cache::insertCacheLine(ADDRINT set,ADDRINT tag, State state){

	ADDRINT to_evict = lruLists[set].back();
	CacheLine newLine,temp;
	newLine.setTag(tag);
	newLine.setState(state);
	temp.setTag(to_evict);
	sets[set].erase(temp);
	sets[set].insert(newLine);

	lruMaps[set].erase(to_evict);
	lruLists[set].pop_back();
	lruLists[set].push_front(tag);
	lruMaps[set].insert(make_pair(tag,lruLists[set].begin()));

}

