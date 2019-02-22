#include "SpecificationFile.h"

SpecificationFile::SpecificationFile(const std::string &filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    std::cerr << "cann't open file" << std::endl;
  }
  unsigned option_count;
  infile >> strength >> option_count;
  std::vector<unsigned> values(option_count);
  for (unsigned i = 0; i < option_count; ++i) {
    infile >> values[i];
  }
  options.initialize(values);
#ifndef NDEBUG
  options.print();
#endif
  infile.close();
}
