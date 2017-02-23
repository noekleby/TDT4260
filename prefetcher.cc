/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"
#include <stdlib.h>
#include <stdio.h>
#include <queue>
using namespace std;

enum State{init,transient,steady,noPred};
struct RPTEntry{//Reference prediction table entry
	RPTEntry(Addr pc);
	void miss(Addr missAddr);//Function for handling a cache miss
	Addr tag;
	Addr lastMissAddr; //address of last cachemiss
	int delta; //address difference between two consecutive cachemisses, initialized to 0
	State currState;
	void stateHandler(int currentDiff,int delta);
};
RPTEntry::RPTEntry(Addr pc) : tag(pc), lastMissAddr(0),delta(0),currState(init){}

void RPTEntry::miss(Addr missAddr){
	int currentDiff = missAddr - lastMissAddr;//calculate stride
	if(currentDiff == delta && !in_cache(missAddr+delta) && currState == steady && !in_mshr_queue(missAddr+delta)){
	//The RPT entry should detect a stride and be in the steady state after consecutive strides
	//The detected stride should not already be in cache or in a queue to be cached 
		issue_prefetch(missAddr+delta); 
	}
	if (currState!=steady && currentDiff != delta){delta = currentDiff;} 
	lastMissAddr = missAddr;
	stateHandler(currentDiff,delta);
}

void RPTEntry::stateHandler(int currentDiff, int delta){
	switch(currState){
		case init:
			if(currentDiff == delta){
				currState = transient;
			}; //first stride, not sure if prefetch
			break;
		case transient:
			if(currentDiff == delta){
				currState = steady;
			}
			else{
				currState = noPred;
			}
			break;
		case steady:
			if(currentDiff != delta){
				currState = init;
			}
			break;
		case noPred:
			if(currentDiff == delta){
				currState = transient;
			}
			break;
		default:
			currState = init;
			break;
}
}

void prefetch_init(void)
{
    //initialise the RPT table/array which will be used to hold all the RPT entries
	list<RPTEntry*> RPTtable;	
}

void prefetch_access(AccessStat stat)
{
  	//a cache access was done, prefetcher is notified
	//check if the current pc is in the RPT table
	//if it is, move the entry to MRU
	//if not, add entry
	//check if the list is full, remove LRU if full
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
