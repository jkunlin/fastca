#include <cmath>
#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ConstraintFile.H"
#include "Coverage.h"
#include "LineVarTupleSet.h"
#include "SAT.H"
#include "Tabu.h"
#include "TupleSet.h"
#include "mersenne.h"

class CoveringArray {
public:
  CoveringArray(const SpecificationFile &specificationFile,
                const ConstraintFile &constraintFile, unsigned long long maxT,
                int seed);
  void greedyConstraintInitialize();
  void optimize();

private:
  SATSolver satSolver;
  Mersenne mersenne;
  const SpecificationFile &specificationFile;
  std::vector<std::vector<unsigned>> array;
  Coverage coverage;
  TupleSet uncoveredTuples;
  std::set<unsigned> varInUncovertuples;
  LineVarTupleSet oneCoveredTuples;
  Tabu<Entry> entryTabu;

  unsigned long long maxTime;
  clock_t clock_start;

  long long step;

  void cover(const unsigned encode, unsigned lineIndex);
  void uncover(const unsigned encode, unsigned lineIndex);
  // produce one row at least cover one uncovered tuple.
  // Producing the row without update coverage
  void produceSatRow(std::vector<unsigned> &newLine, const unsigned encode);
  // greedily produce one row at least cover one uncovered tuple.
  // producing the row AND updating coverage
  void mostGreedySatRow(const unsigned lineIndex, const unsigned encode);
  void replaceRow(const unsigned lineIndex, const unsigned encode);
  void removeUselessRows();
  void removeOneRow();
  long long varScoreOfRow(const unsigned var, const unsigned lineIndex);
  long long varScoreOfRow3(const unsigned var, const unsigned lineIndex);
  void replace(const unsigned var, const unsigned lineIndex);

  long long multiVarRow(const std::vector<unsigned> &sortedMultiVars,
                        const unsigned lineIndex, const bool change = false);
  long long multiVarScoreOfRow(const std::vector<unsigned> &sortedMultiVars,
                               const unsigned lineIndex);
  void multiVarReplace(const std::vector<unsigned> &sortedMultiVars,
                       const unsigned lineIndex);

  void tabuStep();
  void tmpPrint();
  bool verify(const std::vector<std::vector<unsigned>> &resultArray);
#ifndef NDEBUG
  void print();
#endif
  void t();
};
