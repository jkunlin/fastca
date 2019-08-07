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

// Other SAT solvers can be substituted by altering SAT.H and SAT.C.

//#include "sat/SAT.H"

#include "SAT.H"

InputClause::InputClause() : maxVariable(-1) {}

// jkunlin
InputClause::InputClause(const std::vector<InputTerm> &terms)
    : maxVariable(-1) {
  unsigned size = terms.size();
  for (unsigned i = 0; i < size; ++i) {
    append(terms[i]);
  }
}

// InputClause::InputClause(Array<InputTerm>terms) :
//  maxVariable(-1) {
//  unsigned size = terms.getSize();
//  for (unsigned i = 0; i < size; ++i) {
//    append(terms[i]);
//  }
//}

// jkunlin
InputClause::InputClause(const std::vector<unsigned> &symbols)
    : maxVariable(-1) {
  unsigned size = symbols.size();
  for (unsigned i = 0; i < size; ++i) {
    append(InputTerm(false, symbols[i]));
  }
}

// InputClause::InputClause(Array<unsigned>symbols) :
//  maxVariable(-1) {
//  unsigned size = symbols.getSize();
//  for (unsigned i = 0; i < size; ++i) {
//    append(InputTerm(false, symbols[i]));
//  }
//}

InputClause::~InputClause() {}

InputClause::operator vec<Lit> &() { return literals; }
InputClause::operator const vec<Lit> &() const { return literals; }

void InputClause::clear() {
  maxVariable = -1; // jkunlin
  literals.clear();
}

void InputClause::append(InputTerm term) {
  int variable = term.getVariable();
  if (variable > maxVariable) {
    maxVariable = variable;
  }
  literals.push(term.isNegated() ? ~Lit(variable) : Lit(variable));
}

void InputClause::undoAppend() { literals.pop(); }

void SATSolver::reserve(int variables) {
  while (variables >= solver.nVars()) {
    solver.newVar();
  }
}

void SATSolver::addClause(InputClause &clause) {
  reserve(clause.getMaxVariable());
  solver.addClause(clause);
}

bool SATSolver::operator()(const InputKnown &known) {
  if (disable) {
    return true;
  }
  reserve(known.getMaxVariable());
  return solver.simplify() && solver.solve(known);
}
