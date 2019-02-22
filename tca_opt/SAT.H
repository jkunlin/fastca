#ifndef SAT_H
#define SAT_H

#include <vector>

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
  InputClause(const std::vector<InputTerm> &terms);
  InputClause(const std::vector<unsigned> &symbols);
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
