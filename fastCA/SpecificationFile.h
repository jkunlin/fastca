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
