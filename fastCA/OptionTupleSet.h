#ifndef OPTIONTUPLESET_H_MXQYOTTU
#define OPTIONTUPLESET_H_MXQYOTTU

#include <vector>

#include "Combinadic.h"
#include "Coverage.h"
#include "SpecificationFile.h"

struct ECEntry {
public:
  unsigned encode;
  size_t column_index;
};

class OptionTupleSet {
public:
  OptionTupleSet(){};
  void initialize(const SpecificationFile &specificationFile,
                  const Coverage &coverage, bool fullfill = false);
  void pop(const unsigned encode, const std::vector<unsigned> &columns);
  void push(const unsigned encode, const std::vector<unsigned> &columns);
  unsigned encode(const unsigned index) { return tupleSet[index]; }
  unsigned size() const { return tupleSet.size(); }
  std::vector<unsigned>::const_iterator begin() const {
    return tupleSet.begin();
  }
  std::vector<unsigned>::const_iterator end() const { return tupleSet.end(); }

  void pushKcoveredTuple(const Coverage &coverage, int coverCount);

  const std::vector<ECEntry> &getECbyOpt(unsigned opt) {
    return optTupleSet[opt];
  }

private:
  std::vector<unsigned> tupleSet; // contents of tuple encode
  std::vector<std::vector<unsigned>::size_type>
      mapping; // encode -> index in tupleSet

  std::vector<std::vector<ECEntry>> optTupleSet;
  std::vector<std::vector<size_t>> optMapping;
};

#endif /* end of include guard: OPTIONTUPLESET_H_MXQYOTTU */
