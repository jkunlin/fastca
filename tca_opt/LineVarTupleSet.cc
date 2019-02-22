#include "LineVarTupleSet.h"

void LineVarTupleSet::initialize(const SpecificationFile &specificationFile,
                                 const unsigned array_size) {

  const Options &options = specificationFile.getOptions();
  unsigned strength = specificationFile.getStrenth();

  unsigned MaxSize = 0;
  for (std::vector<unsigned> tuple = combinadic.begin(strength);
       tuple[strength - 1] < options.size(); combinadic.next(tuple)) {
    unsigned blockSize = 1;
    for (unsigned i = 0; i < strength; ++i) {
      blockSize *= options.symbolCount(tuple[i]);
    }
    MaxSize += blockSize;
  }
  mapping.resize(MaxSize);
  varMapping.resize(MaxSize, std::vector<size_t>(strength));
  lineVarTupleSet.resize(array_size);
  for (unsigned i = 0; i < array_size; ++i) {
    lineVarTupleSet[i].resize(options.allSymbolCount());
  }

  lineOneCoveredCount.resize(array_size, 0);
}

void LineVarTupleSet::pushOneCoveredTuple(
    const Coverage &coverage, const std::vector<size_t> &coverByLineindex) {
  for (unsigned encode = 0; encode < coverage.tupleCount(); ++encode) {
    if (coverage.coverCount(encode) == 1) {
      push(encode, coverByLineindex[encode], coverage.getTuple(encode));
    }
  }
}

void LineVarTupleSet::push(const unsigned encode, const unsigned lineIndex,
                           const std::vector<unsigned> &tuple) {
  mapping[encode] = tupleSet.size();
  tupleSet.push_back(encode);

  lineOneCoveredCount[lineIndex]++;
  for (size_t i = 0; i < tuple.size(); ++i) {
    unsigned var = tuple[i];
    varMapping[encode][i] = lineVarTupleSet[lineIndex][var].size();
    lineVarTupleSet[lineIndex][var].push_back({encode, i});
  }
}

void LineVarTupleSet::pop(const unsigned encode, const unsigned lineIndex,
                          const std::vector<unsigned> &tuple) {
  tupleSet[mapping[encode]] = tupleSet[tupleSet.size() - 1];
  mapping[tupleSet[tupleSet.size() - 1]] = mapping[encode];
  tupleSet.pop_back();

  lineOneCoveredCount[lineIndex]--;
  for (size_t i = 0; i < tuple.size(); ++i) {
    unsigned var = tuple[i];
    std::vector<ECEntry> &varTS = lineVarTupleSet[lineIndex][var];

    varTS[varMapping[encode][i]] = *varTS.rbegin();
    varMapping[varTS.rbegin()->encode][varTS.rbegin()->column_index] =
        varMapping[encode][i];
    varTS.pop_back();
  }
}
