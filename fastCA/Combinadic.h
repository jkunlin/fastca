#ifndef COMBINADIC_H
#define COMBINADIC_H

#include <vector>

#include "PascalTriangle.h"

class Combinadic {
public:
  //	Combinadic() {
  //		encode_fun.push_back(encode_2way);
  //		encode_fun.push_back(encode_3way);
  //		encode_fun.push_back(encode_anyway);
  //	}
  std::vector<unsigned> begin(const unsigned size) const;
  void next(std::vector<unsigned> &sortedSubset) const;
  void previous(std::vector<unsigned> &sortedSubset) const;
  unsigned encode(const std::vector<unsigned> &sortedSubset) const;

private:
  unsigned encode_2way(const std::vector<unsigned> &sortedSubset) const {
    unsigned result = 0;
    result += sortedSubset[0];
    result += sortedSubset[1] * (sortedSubset[1] - 1) / 2;
    return result;
  }

  unsigned encode_3way(const std::vector<unsigned> &sortedSubset) const {
    unsigned result = 0;
    result += sortedSubset[0];
    result += sortedSubset[1] * (sortedSubset[1] - 1) / 2;
    result +=
        sortedSubset[2] * (sortedSubset[2] - 1) * (sortedSubset[2] - 2) / 6;
    return result;
  }

  unsigned encode_anyway(const std::vector<unsigned> &sortedSubset) const {
    unsigned result = 0;
    for (unsigned i = 0; i < sortedSubset.size(); ++i) {
      result += pascalTriangle.nCr(sortedSubset[i], i + 1);
    }
    return result;
  }

  //	std::vector<unsigned (*) (const std::vector<unsigned> &)> encode_fun;
  //	unsigned ((*encode_fun)[3]) (const std::vector<unsigned> &sortedSubset);
};

extern Combinadic combinadic;
#endif /* end of include guard: COMBINADIC_H */
