/*
This file represent a cache line in a cache
**/
#ifndef CACHELINE_H
#define CACHELINE_H
#include "pin.H"


enum State{MODIFIED,OWNER,EXCLUSIVE,SHARED,INVALID};

class CacheLine
{

private:
	ADDRINT tag;
	State state;


public:
	CacheLine();
	
	ADDRINT getTag() const;

	State getState() const;

	void setTag(ADDRINT tag);

	void setState(State state);


	bool operator<(const CacheLine& rhs) const;
	bool operator==(const CacheLine& rhs) const;
	
};

#endif