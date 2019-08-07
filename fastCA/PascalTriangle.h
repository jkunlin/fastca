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

#ifndef PASCALTRIANGLE_H
#define PASCALTRIANGLE_H

#include <vector>

class PascalTriangle {
public:
  PascalTriangle();
  void rowsGrowTo(const unsigned rows);
  unsigned nCr(const unsigned n, const unsigned r) {
    if (n >= table.size()) {
      rowsGrowTo(n);
    }
    if (r > n) {
      return 0;
    }
    return table[n][r];
  }

private:
  std::vector<std::vector<unsigned>> table;
};

extern PascalTriangle pascalTriangle;
#endif /* end of include guard: PASCALTRIANGLE_H */
