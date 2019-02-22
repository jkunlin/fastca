#include "Combinadic.h"
#include <cassert>

std::vector<unsigned> Combinadic::begin(const unsigned size) const {
  std::vector<unsigned> vec(size);
  for (unsigned i = 0; i < size; ++i) {
    vec[i] = i;
  }
  return vec;
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

// unsigned Combinadic::encode(const std::vector<unsigned> &sortedSubset) const
// {
//	unsigned result = 0;
//	for (unsigned i = 0; i < sortedSubset.size(); ++i) {
//		result += pascalTriangle.nCr(sortedSubset[i], i+1);
//	}
//	return result;
//}

// unsigned Combinadic::encode(const std::vector<unsigned> &sortedSubset) const
// {
//	unsigned result = 0;
//	result += sortedSubset[0];
//	result += sortedSubset[1] * (sortedSubset[1] - 1) / 2;
//	result += sortedSubset[2] * (sortedSubset[2] - 1) * (sortedSubset[2] -
// 2) / 6;
//	return result;
//}

// unsigned Combinadic::encode(const std::vector<unsigned> &sortedSubset) const
// {
//	unsigned result = 0;
//	for (unsigned i = 0; i < sortedSubset.size(); ++i) {
//		unsigned nCr = 1;
//		for (unsigned j = 0; j <= i; ++j) {
//			nCr *= sortedSubset[i] - j;
//		}
//		for (unsigned j = 2; j <= i + 1; ++j) {
//			nCr /= j;
//		}
//		result += nCr;
//	}
//	return result;
//}

unsigned Combinadic::encode(const std::vector<unsigned> &sortedSubset) const {
  switch (sortedSubset.size()) {
  case 2:
    return encode_2way(sortedSubset);
  case 3:
    return encode_3way(sortedSubset);
  default:
    return encode_anyway(sortedSubset);
  }
}

Combinadic combinadic;
