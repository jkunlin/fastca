#include "Coverage.h"

Coverage::Coverage(const SpecificationFile &specificationFile)
    : specificationFile(specificationFile) {}

void Coverage::initialize(SATSolver &satSovler) {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  offsets.resize(pascalTriangle.nCr(options.size(), strength));
  unsigned size = 0;
  unsigned offsetIndex = 0;

  std::vector<unsigned> tuple(strength);
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < options.size(); combinadic.next(columns)) {

    offsets[offsetIndex++] = size;
    this->columns.push_back(columns);

    for (unsigned i = 0; i < strength; ++i) {
      tuple[i] = options.firstSymbol(columns[i]);
    }
    tuples.push_back(tuple);
    InputKnown known;
    for (auto var : tuple) {
      known.append(InputTerm(false, var));
    }
    if (satSovler(known)) {
      contents.push_back(0);
    } else {
      contents.push_back(-1);
    }
    ++size;
    for (unsigned i = strength; i--;) {
      if (++tuple[i] > options.lastSymbol(columns[i])) {
        tuple[i] = options.firstSymbol(columns[i]);
      } else {
        tuples.push_back(tuple);
        InputKnown known;
        for (auto var : tuple) {
          known.append(InputTerm(false, var));
        }
        if (satSovler(known)) {
          contents.push_back(0);
        } else {
          contents.push_back(-1);
        }
        ++size;
        i = strength;
      }
    }
  }
//	contents = std::vector<unsigned> (size, 0);

#ifndef NDEBUG
  std::cout << "********Debuging Coverage********" << std::endl;
  std::cout << "offset : " << std::endl;
  for (auto x : offsets) {
    std::cout << x << ' ';
  }
  std::cout << std::endl << "contents size : " << contents.size() << std::endl;

  for (unsigned i = 0; i < tuples.size(); ++i) {
    std::cout << i;
    for (auto x : tuples[i]) {
      std::cout << ' ' << x;
    }
    std::cout << std::endl;
  }
  std::cout << "********end of Debuging Coverage********" << std::endl;
#endif
}

unsigned Coverage::encode(const std::vector<unsigned> &sortedColumns,
                          const std::vector<unsigned> &sortedSubset) {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();

  unsigned base = offsets[combinadic.encode(sortedColumns)];
  unsigned offset = sortedSubset[0] - options.firstSymbol(sortedColumns[0]);
  for (unsigned i = 1; i < strength; ++i) {
    offset *= options.symbolCount(sortedColumns[i]);
    offset += sortedSubset[i] - options.firstSymbol(sortedColumns[i]);
  }
  return base + offset;
}

const std::vector<unsigned> &Coverage::getColumns(const unsigned encode) const {
  auto iter = std::upper_bound(offsets.begin(), offsets.end(), encode);
  return columns[iter - offsets.begin() - 1];
}

bool Coverage::allIsCovered() {
  for (unsigned i = 0; i < contents.size(); ++i) {
    if (contents[i] == 0) {
      std::cerr << "encode: " << i << " tuple: ";
      std::vector<unsigned> &tuple = tuples[i];
      for (auto var : tuple) {
        std::cerr << var << "-";
      }
      std::cerr << "\b "
                << "columns: ";
      std::vector<unsigned> &tupleColumns = columns[i];
      for (auto column : tupleColumns) {
        std::cerr << column << "-";
      }
      std::cerr << "\b " << std::endl;
      return false;
    }
  }
  return true;
}

void Coverage::print() {
  for (unsigned i = 0; i < tuples.size(); ++i) {
    auto &tuple = tuples[i];
    std::cerr << i;
    if (i < 10) {
      std::cerr << "  : ";
    } else if (i < 100) {
      std::cerr << " : ";
    } else {
      std::cerr << ": ";
    }
    for (auto var : tuple) {
      std::cerr << var << '-';
    }
    std::cerr << "\b  * " << contents[i];
    if (i % 10 == 9) {
      std::cerr << std::endl;
    } else {
      std::cerr << "    ";
    }
  }
  std::cerr << std::endl;
}
