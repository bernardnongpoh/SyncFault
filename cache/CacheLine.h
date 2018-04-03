/*
This file represent a cache line in a cache
**/
#ifndef CACHELINE_H
#define CACHELINE_H

enum State{MODIFIED,OWNER,EXCLUSIVE,SHARED,INVALID};

class CacheLine
{

private:
	uint64_t tag;
	State state;


public:
	CacheLine(){
		tag=0;
		state=INVALID;	
	}
	~CacheLine();

	uint64_t getTag() const{
		return tag;
		}

	State getState() const{
		return state;
	}

	void setTag(uint64_t tag){
		this->tag=tag;
	}

	void setState(State state){
		this->state=state;
	}


	bool operator<(const CacheLine& rhs) const{
		return tag < rhs.getTag();
	}
	bool operator==(const CacheLine& rhs) const{
		return tag==rhs.getTag();
	}
	
};

#endif