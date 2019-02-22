#ifndef COMBINADIC_H
#define COMBINADIC_H

#include <vector>

#include "PascalTriangle.h"

class Combinadic {
public:
  std::vector<unsigned> begin(const unsigned size) const;
  void next(std::vector<unsigned> &sortedSubset) const;
  void previous(std::vector<unsigned> &sortedSubset) const;
  unsigned encode(const std::vector<unsigned> &sortedSubset) const;

private:
};

extern Combinadic combinadic;
#endif /* end of include guard: COMBINADIC_H */
