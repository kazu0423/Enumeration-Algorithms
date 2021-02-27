#ifndef __ENUM__
#define __ENUM__

#include <vector>
#include <set>
#include <utility>
#include "bipartite.hpp"

#include <boost/dynamic_bitset.hpp>


void KBest(BipartiteGraph &G, int k, std::set<boost::dynamic_bitset<> > &output);
void EnumerateLMM(BipartiteGraph &G, int k, std::set<boost::dynamic_bitset<> > &output);

#endif //__ENUM__