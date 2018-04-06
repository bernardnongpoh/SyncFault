#include "System.h"

System::System(std::vector<unsigned int> tid_to_domain,unsigned int lineSize,unsigned int numLines,unsigned int assoc){

    assert(numLines % assoc == 0);

   statistics.hits = statistics.local_reads = statistics.remote_reads = 
      statistics.othercache_reads = statistics.local_writes = 
      statistics.remote_writes = statistics.compulsory = 0;

    LINE_MASK = ((uint64_t) lineSize)-1;
   SET_SHIFT = log2(lineSize);
   SET_MASK = ((numLines / assoc) - 1) << SET_SHIFT;
   TAG_MASK = ~(SET_MASK | LINE_MASK);

   this->tid_to_domain = tid_to_domain;
     cache = new Cache(numLines, assoc);


}

   void System::memAccess(uint64_t address,char rw,unsigned int tid){

      statistics.access++;
       uint64_t set,tag;
       bool hit;
       State state;

        set = (address & SET_MASK) >> SET_SHIFT;
        tag = address & TAG_MASK;
        state = cache->findTag(set,tag);
        hit = (state!=INVALID);

        if(rw=='W' && hit){
            cache->changeState(set,tag,MODIFIED);
        }

        if(hit){
            cache->updateLRU(set,tag);
            statistics.hits++;
            return;
        }

        State new_state=INVALID;
        uint64_t evicted_tag=0; // FIX ME this cannot be zero
        bool writeBack = cache->checkWriteBack(set,evicted_tag);
       
        if(writeBack){
            statistics.local_writes++;
        }

        if(rw=='R'){
            new_state=EXCLUSIVE;
        }
        else{
            new_state=MODIFIED;
        }

        cache->insertCacheLine(set,tag,new_state);

   }
   


System::~System()
{
   delete cache;
}

