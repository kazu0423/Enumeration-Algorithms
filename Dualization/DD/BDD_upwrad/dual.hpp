// Hypergraph independent set ZDD construction (simple version)
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <tdzdd/DdSpec.hpp>
#include <tdzdd/DdStructure.hpp>
#include <vector>

// SAPPOROBDD headers (adjust include path/name to your local installation).
#include "ZBDD.h"
#include "SBDD_helper.h"


using namespace std;
ZBDD complement_family(const ZBDD& f, int n);
ZBDD dual_family(const ZBDD& f, int n);
ZBDD minimalize_upward_closed(const ZBDD& f, int n);