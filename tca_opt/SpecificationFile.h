#ifndef SPECIFICATIONFILE_H
#define SPECIFICATIONFILE_H

#include <fstream>
#include <string>

#include "Options.h"

class SpecificationFile {
public:
  SpecificationFile(const std::string &filename);
  const Options &getOptions() const { return options; }
  unsigned getStrenth() const { return strength; }

private:
  Options options;
  unsigned strength;
};

#endif /* end of include guard: SPECIFICATIONFILE_H */
