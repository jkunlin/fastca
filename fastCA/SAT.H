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

#ifndef SAT_H
#define SAT_H

#include <vector>

// jkunlin
#include "./minisat/solver/Solver.H"
//#include "Array.H"
//#include "Solver.H"

// A literal in a SAT clause.
class InputTerm {
protected:
  int encoding;

public:
  InputTerm() : encoding(0) {}
  InputTerm(int encoding) : encoding(encoding) {}
  InputTerm(bool negated, int variable)
      : encoding((variable << 1) | (int)negated) {}

  operator int() const { return encoding; }
  InputTerm &operator=(int encoding) {
    this->encoding = encoding;
    return *this;
  }

  bool isNegated() const { return encoding & 1; }
  int getVariable() const { return encoding >> 1; }
};

// A SAT clause.
class InputClause {
protected:
  int maxVariable;
  vec<Lit> literals;

public:
  InputClause();
  // jkunlin
  InputClause(const std::vector<InputTerm> &terms);
  InputClause(const std::vector<unsigned> &symbols);
  bool empty() { return literals.size() == 0; }
  //  InputClause(Array<InputTerm>terms);
  //  InputClause(Array<unsigned>symbols);
  virtual ~InputClause();

  operator vec<Lit> &();
  operator const vec<Lit> &() const;

  int getMaxVariable() const { return maxVariable; }

  void clear();
  void append(InputTerm term);
  void undoAppend();
};

// A partial assignment.
typedef InputClause InputKnown;

// A solver-wrapping class.
class SATSolver {
protected:
  const bool disable;
  // The miniSAT implementation.
  Solver solver;

public:
  SATSolver(bool ds = false) : disable(ds) {}
  void reserve(int variables);
  void addClause(InputClause &clause);
  bool operator()(const InputKnown &known);
};

#endif
