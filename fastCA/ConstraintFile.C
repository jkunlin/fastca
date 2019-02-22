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
