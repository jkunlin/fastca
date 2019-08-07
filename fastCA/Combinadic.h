// Copyright 2008, 2009 Brady J. Garvin

// This file is part of Covering Arrays by Simulated Annealing (CASA).

// CASA is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// CASA is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with CASA.  If not, see <http://www.gnu.org/licenses/>.

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
