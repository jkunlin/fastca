#include "LocalSearch.h"
#include "TupleSet.h"

#include "ActsSolver.h"
#include "CoveringArray.h"
#include <unistd.h>

void localSearch(const SpecificationFile &specificationFile,
                 const ConstraintFile &constraintFile,
                 const unsigned long long maxTime, int seed) {
  CoveringArray c(specificationFile, constraintFile, maxTime, seed);
  //	c.greedyConstraintInitialize2();
  ActsSolver ActsSolver;
  char filename[L_tmpnam];
  if (!tmpnam(filename)) {
    std::cerr << "tmp file name error" << std::endl;
    abort();
  }
  std::string acts_res_filename = filename;
  acts_res_filename += std::to_string(getpid());
  ActsSolver.solve(specificationFile, constraintFile, acts_res_filename);
  c.actsInitialize(acts_res_filename);
  std::string cmd = (std::string) "rm " + acts_res_filename;
  if (system(cmd.c_str()) != 0) {
    std::cerr << "can't remove acts result file" << std::endl;
    exit(0);
  };
  //	return ;
  c.optimize();
}
