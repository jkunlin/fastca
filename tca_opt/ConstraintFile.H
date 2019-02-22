#ifndef CONSTRAINT_FILE_H
#define CONSTRAINT_FILE_H

#include <fstream>
#include <string>
#include <vector>

#include "SAT.H"

class ConstraintFile {
  std::vector<InputClause> clauses;

public:
  ConstraintFile(const std::string &filename);
  bool isEmpty() const { return clauses.size() == 0; }
  const std::vector<InputClause> &getClauses() const;
};

#endif
