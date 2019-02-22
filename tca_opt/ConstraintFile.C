#include "ConstraintFile.H"

using namespace std;

ConstraintFile::ConstraintFile(const string &filename) {
  if (!filename.size()) {
    return;
  }
  ifstream fileInputStream(filename.data());
  unsigned clauseCount;
  fileInputStream >> clauseCount;
  clauses = std::move(std::vector<InputClause>(clauseCount));
  //  clauses.resize(clauseCount);
  //  clauses = Array<InputClause>(clauseCount);
  for (unsigned i = 0; i < clauseCount; ++i) {
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
    }
  }
  fileInputStream.close();
}

const std::vector<InputClause> &ConstraintFile::getClauses() const {
  return clauses;
}
