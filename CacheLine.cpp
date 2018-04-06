#include "CacheLine.h"

CacheLine::CacheLine(){
		this->tag=0;
		this->state=INVALID;	
}

State CacheLine::getState() const{
		return this->state;
}

ADDRINT CacheLine::getTag() const{
	return this->tag;
}

void CacheLine::setState(State state){
	this->state=state;
}
void CacheLine::setTag(ADDRINT tag){
	this->tag=tag;
}

bool CacheLine::operator<(const CacheLine& rhs) const{
		return this->tag < rhs.getTag();
}

bool CacheLine::operator==(const CacheLine& rhs) const{
	return this->tag==rhs.getTag();
}
