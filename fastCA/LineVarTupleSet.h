#ifndef LINEVARTUPLESET_H_MBOTX5KJ
#define LINEVARTUPLESET_H_MBOTX5KJ

#include <vector>

#include "Combinadic.h"
#include "Coverage.h"
#include "SpecificationFile.h"

struct ECEntry {
public:
  unsigned encode;
  size_t column_index;
};

class LineVarTupleSet {
public:
  LineVarTupleSet(){};
  void initialize(const SpecificationFile &specificationFile,
                  const unsigned array_size);
  void pop(const unsigned encode, const unsigned lineIndex,
           const std::vector<unsigned> &tuple);
  void push(const unsigned encode, const unsigned lineIndex,
            const std::vector<unsigned> &tuple);
  unsigned encode(const unsigned index) { return tupleSet[index]; }
  unsigned size() const { return tupleSet.size(); }
  std::vector<unsigned>::const_iterator begin() const {
    return tupleSet.begin();
  }
  std::vector<unsigned>::const_iterator end() const { return tupleSet.end(); }

  void pushOneCoveredTuple(const Coverage &coverage,
                           const std::vector<size_t> &coverByLineindex);

  void exchange_row(unsigned lineIndex1, unsigned lineIndex2) {
    lineVarTupleSet[lineIndex1].swap(lineVarTupleSet[lineIndex2]);
    std::swap(lineOneCoveredCount[lineIndex1], lineOneCoveredCount[lineIndex2]);
  }
  void pop_back_row() {
    lineVarTupleSet.pop_back();
    lineOneCoveredCount.pop_back();
  }

  const std::vector<ECEntry> &getECbyLineVar(unsigned lineIndex, unsigned var) {
    return lineVarTupleSet[lineIndex][var];
  }

  unsigned oneCoveredCount(unsigned lineIndex) {
    return lineOneCoveredCount[lineIndex];
  }

private:
  std::vector<unsigned> tupleSet; // contents of tuple encode
  std::vector<std::vector<unsigned>::size_type>
      mapping; // encode -> index in tupleSet

  std::vector<size_t> lineOneCoveredCount;

  std::vector<std::vector<std::vector<ECEntry>>> lineVarTupleSet;
  std::vector<std::vector<size_t>> varMapping;
};

#endif /* end of include guard: LINEVARTUPLESET_H_MBOTX5KJ */
