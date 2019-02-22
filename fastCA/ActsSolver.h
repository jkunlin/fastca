#ifndef ACTSSOLVER_H_CQDYTNB9
#define ACTSSOLVER_H_CQDYTNB9

#include "ConstraintFile.H"
#include "SpecificationFile.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

class ActsSolver {
public:
  void solve(const SpecificationFile &specificationFile,
             const ConstraintFile &constraintFile,
             const std::string &res_file_name);

private:
  void generate_input_file(const SpecificationFile &specificationFile,
                           const ConstraintFile &constraintFile,
                           const std::string &acts_inputfile_name);
};

#endif /* end of include guard: ACTSSOLVER_H_CQDYTNB9 */
