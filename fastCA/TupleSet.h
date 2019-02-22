#ifndef TUPLESET_H
#define TUPLESET_H

#include <vector>

#include "Combinadic.h"
#include "Coverage.h"
#include "SpecificationFile.h"

class TupleSet {
public:
  TupleSet(){};
  void initialize(const SpecificationFile &specificationFile,
                  const Coverage &coverage, bool fullfill = false);
  void pop(const unsigned encode);
  void push(const unsigned encode);
  unsigned encode(const unsigned index) { return tupleSet[index]; }
  unsigned size() const { return tupleSet.size(); }
  std::vector<unsigned>::const_iterator begin() const {
    return tupleSet.begin();
  }
  std::vector<unsigned>::const_iterator end() const { return tupleSet.end(); }
  unsigned min();
#ifndef NDEBUG
  void print();
#endif

  void pushKcoveredTuple(const Coverage &coverage, int coverCount);

private:
  std::vector<unsigned> tupleSet; // contents of tuple encode
  std::vector<std::vector<unsigned>::size_type>
      mapping; // encode -> index in tupleSet
};

#endif /* end of include guard: TUPLESET_H */
