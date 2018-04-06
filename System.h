#ifndef SYSTEM_H
#define SYSTEM_H
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <set>
#include <stdio.h>
#include <vector>
#include <map>
#include "Cache.h"



// In separate struct so ThreadedSim
// doesn't need to repeat
// writes are counted when prefetching,
// other stats are not
struct SystemStats {

  unsigned long long access;
   unsigned long long hits;
   unsigned long long local_reads;
   unsigned long long remote_reads;
   unsigned long long othercache_reads;
   unsigned long long local_writes;
   unsigned long long remote_writes;
   unsigned long long compulsory;
   unsigned long long prefetched;
   
   
   SystemStats()
   {
      hits = local_reads = remote_reads = othercache_reads = 
         local_writes = remote_writes = compulsory = prefetched = 0;
   }
};


class System{

private:
    Cache *cache;
    protected:
      uint64_t SET_MASK;
      uint64_t TAG_MASK;
      uint64_t LINE_MASK;
      unsigned int SET_SHIFT;
      std::vector<unsigned int> tid_to_domain;

        public:
        ~System();
            System(std::vector<unsigned int> tid_to_domain,unsigned int lineSize,unsigned int numLines,unsigned int assoc);
             void memAccess(ADDRINT address,char rw,unsigned int tid);
            SystemStats statistics;

};

#endif