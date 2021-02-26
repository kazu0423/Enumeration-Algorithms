#ifndef __ENUM__
#define __ENUM__

#include <vector>
#include <set>
#include <utility>
#include "graph.hpp"

#include <boost/dynamic_bitset.hpp>


void Enumerate(Graph &G, boost::dynamic_bitset<> &matching, int k, std::set<boost::dynamic_bitset<> > &output);

#endif //__ENUM__