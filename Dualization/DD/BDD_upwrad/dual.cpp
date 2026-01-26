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

ZBDD complement_family(const ZBDD& f, int n) {
  ZBDD comp = f;
  for (int i = 1; i <= n; ++i) {
    comp = comp.Change(i);
  }
  return comp;
}

ZBDD dual_family(const ZBDD& f, int n) {
  ZBDD all = sbddh::getPowerSet(n);
  ZBDD comp = complement_family(f, n);
  return all - comp;
}

ZBDD minimalize_upward_closed(const ZBDD& f, int n) {
  // Remove any set that has a one-element deletion still in f.
  ZBDD result = f;
  for (int i = 1; i <= n; ++i) {
    ZBDD removed = result.OnSet0(i);  // Remove i from sets that contain i.
    ZBDD redundant_base = removed & result;
    ZBDD redundant_sets = redundant_base.Change(i);
    result = result - redundant_sets;
  }
  return result;
}
