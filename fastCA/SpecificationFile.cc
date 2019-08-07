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
