#include "Combinadic.h"
#include <cassert>

std::vector<unsigned> Combinadic::begin(const unsigned size) const {
  std::vector<unsigned> vec(size);
  for (unsigned i = 0; i < size; ++i) {
    vec[i] = i;
  }
  return std::move(vec);
}

void Combinadic::next(std::vector<unsigned> &sortedSubset) const {
  assert(sortedSubset.size());
  unsigned limit = sortedSubset.size() - 1, ceiling = sortedSubset[0];
  for (unsigned i = 0; i < limit; ++i) {
    unsigned entry = ceiling + 1;
    ceiling = sortedSubset[i + 1];
    if (entry < ceiling) {
      sortedSubset[i] = entry;
      return;
    }
    sortedSubset[i] = i;
  }
  ++sortedSubset[limit];
}

void Combinadic::previous(std::vector<unsigned> &sortedSubset) const {
  assert(sortedSubset.size());
  unsigned limit = sortedSubset.size();
  for (unsigned i = 0; i < limit; ++i) {
    unsigned entry = sortedSubset[i];
    if (entry > i) {
      do {
        sortedSubset[i] = --entry;
      } while (i-- > 0);
      return;
    }
  }
}

unsigned Combinadic::encode(const std::vector<unsigned> &sortedSubset) const {
  unsigned result = 0;
  for (unsigned i = 0; i < sortedSubset.size(); ++i) {
    result += pascalTriangle.nCr(sortedSubset[i], i + 1);
  }
  return result;
}

Combinadic combinadic;
