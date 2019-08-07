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

#include "ConstraintFile.H"

using namespace std;

ConstraintFile::ConstraintFile(const string &filename) {
  if (!filename.size()) {
    return;
  }
  ifstream fileInputStream(filename.data());
  unsigned clauseCount;
  fileInputStream >> clauseCount;
  std::vector<InputClause>(clauseCount).swap(clauses);
  //  clauses.resize(clauseCount);
  //  clauses = Array<InputClause>(clauseCount);
  for (unsigned i = 0; i < clauseCount; ++i) {
    Valid::Clause valid_clause;
    InputClause &clause = clauses[i];
    unsigned termCount;
    fileInputStream >> termCount;
    while (termCount--) {
      char sign;
      unsigned symbol;
      do {
        fileInputStream >> sign;
      } while (sign != '-' && sign != '+');
      fileInputStream >> symbol;
      clause.append(InputTerm(sign == '-', symbol));
      valid_clause.addLiteral(Valid::Literal(sign == '-', symbol));
    }
    formula.addClause(std::move(valid_clause));
  }
  fileInputStream.close();
}

const std::vector<InputClause> &ConstraintFile::getClauses() const {
  return clauses;
}
