#include "OptionTupleSet.h"

void OptionTupleSet::initialize(const SpecificationFile &specificationFile,
                                const Coverage &coverage, bool fullfill) {

  const Options &options = specificationFile.getOptions();
  unsigned strength = specificationFile.getStrenth();

  unsigned MaxSize = 0;
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < options.size(); combinadic.next(columns)) {
    unsigned blockSize = 1;
    for (unsigned i = 0; i < strength; ++i) {
      blockSize *= options.symbolCount(columns[i]);
    }
    MaxSize += blockSize;
  }
  mapping.resize(MaxSize);
  optMapping.resize(MaxSize, std::vector<size_t>(strength));
  optTupleSet.resize(options.size());
  if (fullfill) {
    for (unsigned encode = 0; encode < coverage.tupleCount(); ++encode) {
      if (coverage.coverCount(encode) != -1) {
        push(encode, coverage.getColumns(encode));
      }
    }
  }
}

void OptionTupleSet::pushKcoveredTuple(const Coverage &coverage,
                                       int coverCount) {
  for (unsigned encode = 0; encode < coverage.tupleCount(); ++encode) {
    if (coverage.coverCount(encode) == coverCount) {
      push(encode, coverage.getColumns(encode));
    }
  }
}

void OptionTupleSet::push(const unsigned encode,
                          const std::vector<unsigned> &columns) {
  mapping[encode] = tupleSet.size();
  tupleSet.push_back(encode);

  for (size_t i = 0; i < columns.size(); ++i) {
    unsigned opt = columns[i];
    optMapping[encode][i] = optTupleSet[opt].size();
    optTupleSet[opt].push_back({encode, i});
  }
}

void OptionTupleSet::pop(const unsigned encode,
                         const std::vector<unsigned> &columns) {
  tupleSet[mapping[encode]] = tupleSet[tupleSet.size() - 1];
  mapping[tupleSet[tupleSet.size() - 1]] = mapping[encode];
  tupleSet.pop_back();

  for (size_t i = 0; i < columns.size(); ++i) {
    unsigned opt = columns[i];
    std::vector<ECEntry> &optTS = optTupleSet[opt];

    optTS[optMapping[encode][i]] = *optTS.rbegin();
    optMapping[optTS.rbegin()->encode][optTS.rbegin()->column_index] =
        optMapping[encode][i];
    optTS.pop_back();
  }
}
