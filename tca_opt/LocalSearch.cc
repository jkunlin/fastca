#include "LocalSearch.h"
#include "TupleSet.h"

#include "CoveringArray.h"

void localSearch(const SpecificationFile &specificationFile,
                 const ConstraintFile &constraintFile,
                 const unsigned long long maxTime, int seed) {
  CoveringArray c(specificationFile, constraintFile, maxTime, seed);
  c.greedyConstraintInitialize();
  c.optimize();
}
