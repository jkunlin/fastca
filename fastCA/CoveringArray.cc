#include "CoveringArray.h"

CoveringArray::CoveringArray(const SpecificationFile &specificationFile,
                             const ConstraintFile &constraintFile,
                             unsigned long long maxT, int seed)
    : validater(specificationFile), satSolver(constraintFile.isEmpty()),
      specificationFile(specificationFile), coverage(specificationFile),
      entryTabu(4), maxTime(maxT) {

  clock_start = clock();
  const Options &options = specificationFile.getOptions();
  // add constraint into satSolver
  const std::vector<InputClause> &clauses = constraintFile.getClauses();
  for (unsigned i = 0; i < clauses.size(); ++i) {
    satSolver.addClause(const_cast<InputClause &>(clauses[i]));
  }
  const Valid::Formula &formula = constraintFile.getFormula();
#ifndef NVISIBLE
  formula.print();
#endif
  for (auto &c : formula) {
    validater.addClause(c);
  }

  option_constrained_indicator.clear();
  option_constrained_indicator.resize(options.size(), false);
  for (auto &c : formula) {
    for (auto &lit : c) {
      option_constrained_indicator[options.option(lit.variable())] = true;
    }
  }
  for (unsigned option = 0; option < options.size(); ++option) {
    if (!option_constrained_indicator[option]) {
      continue;
    }
    InputClause atLeast;
    for (unsigned j = options.firstSymbol(option),
                  limit = options.lastSymbol(option);
         j <= limit; ++j) {
      atLeast.append(InputTerm(false, j));
    }
    satSolver.addClause(atLeast);
    for (unsigned j = options.firstSymbol(option),
                  limit = options.lastSymbol(option);
         j <= limit; ++j) {
      for (unsigned k = j + 1; k <= limit; ++k) {
        InputClause atMost;
        atMost.append(InputTerm(true, j));
        atMost.append(InputTerm(true, k));
        satSolver.addClause(atMost);
      }
    }
  }

  //		coverage.initialize(satSolver, option_constrained_indicator);
  //		uncoveredTuples.initialize(specificationFile, coverage, true);
  coverage.unconstrained_initialize();
  uncoveredTuples.initialize(specificationFile, coverage);

  mersenne.seed(seed);
  step = 0;
}

void CoveringArray::actsInitialize(const std::string file_name) {
  const Options &options = specificationFile.getOptions();
  const unsigned &strenth = specificationFile.getStrenth();
  std::ifstream res_file(file_name);
  if (!res_file.is_open()) {
    std::cerr << "file open failed" << std::endl;
    exit(0);
  }
  std::string line;
  while (getline(res_file, line)) {
    if (line.find("Test Cases") != std::string::npos) {
      break;
    }
  }
  while (true) {
    bool begin = false;
    while (getline(res_file, line)) {
      if (line[0] == '1') {
        begin = true;
        break;
      }
    }
    if (!begin) {
      break;
    }
    array.push_back(std::vector<unsigned>(options.size()));
    std::vector<unsigned> &newRow = *array.rbegin();
    for (unsigned option = 0; option < options.size(); ++option) {
      unsigned value;
      std::string value_str(
          line.substr(line.find_last_of('=') + 1, line.size() - 1));
      value = atoi(value_str.c_str());
      newRow[option] = value + options.firstSymbol(option);
      getline(res_file, line);
    }
  }
  res_file.close();

  clock_t start = clock();
  std::vector<size_t> coverByLineIndex(coverage.tupleSize());
  std::vector<unsigned> tuple(strenth);
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    auto &line = array[lineIndex];
    for (std::vector<unsigned> columns = combinadic.begin(strenth);
         columns[strenth - 1] < line.size(); combinadic.next(columns)) {
      for (unsigned i = 0; i < strenth; ++i) {
        tuple[i] = line[columns[i]];
      }
      //			cover(coverage.encode(columns, tuple));
      unsigned encode = coverage.encode(columns, tuple);
      coverByLineIndex[encode] = lineIndex;
      coverage.cover(encode);
    }
  }
  coverage.set_zero_invalid();
  std::cout << "actsInitialize: " << double(clock() - start) / CLOCKS_PER_SEC
            << std::endl;
  entryTabu.initialize(Entry(array.size(), array.size()));
  validater.initialize(array);
  tmpPrint();
  oneCoveredTuples.initialize(specificationFile, array.size());
  oneCoveredTuples.pushOneCoveredTuple(coverage, coverByLineIndex);
#ifndef NDEBUG
  int i = 0;
  for (auto &line : array) {
    std::cout << i++ << ": ";
    for (auto v : line) {
      std::cout << v << ' ';
    }
    std::cout << std::endl;
  }
#endif
}

// void CoveringArray::call_acts(const ConstraintFile &constraintFile) {
//	std::string acts_inputfile_name = "input_acts";
//	std::ofstream acts_infile(acts_inputfile_name);
//	if (!acts_infile.is_open()) {
//		std::cerr << "open failed" << std::endl;
//		exit(0);
//	}
//
//	acts_infile << "[System]" << std::endl;
//	acts_infile << "Name: " << acts_inputfile_name << std::endl <<
// std::endl;
//	acts_infile << "[Parameter]" << std::endl;
//
//	const Options &options = specificationFile.getOptions();
//	for (unsigned option = 0; option < options.size(); ++option) {
//		acts_infile << 'p' << option << "(int): ";
//		acts_infile << 0;
//		for (unsigned var = 1; var < options.symbolCount(option); ++var)
//{
//			acts_infile << ',' << var;
//		}
//		acts_infile << std::endl;
//	}
//
//	acts_infile << std::endl << "[Constraint]" << std::endl;
//	const Valid::Formula &formula = constraintFile.getFormula();
//	for (auto &c : formula) {
//		const unsigned option = options.option(c[0].variable());
//		acts_infile << 'p' << option;
//		c[0].is_negative() ? acts_infile << "!=" : acts_infile << "=";
//		acts_infile << c[0].variable() - options.firstSymbol(option);
//		for (unsigned i = 1; i < c.size(); ++i) {
//			acts_infile <<  " || ";
//			const unsigned option = options.option(c[i].variable());
//			acts_infile << 'p' << option;
//			c[i].is_negative() ? acts_infile << "!=" : acts_infile
//<<
//"=";
//			acts_infile << c[i].variable() -
// options.firstSymbol(option);
//		}
//		acts_infile << std::endl;
//	}
//}

void CoveringArray::produceSatRow(std::vector<unsigned> &newLine,
                                  const unsigned encode) {
  const unsigned strength = specificationFile.getStrenth();
  const Options &options = specificationFile.getOptions();
  const unsigned width = options.size();
  assert(width == newLine.size());

  InputKnown known;
  const std::vector<unsigned> &ranTuple = coverage.getTuple(encode);
  const std::vector<unsigned> &ranTupleColumns = coverage.getColumns(encode);
  for (unsigned i = 0; i < strength; ++i) {
    newLine[ranTupleColumns[i]] = ranTuple[i];
    if (option_constrained_indicator[ranTupleColumns[i]]) {
      known.append(InputTerm(false, ranTuple[i]));
    }
  }
  std::vector<bool> columnStarted(width, false);
  std::vector<unsigned> columnBases(width);
  for (unsigned column = 0, passing = 0; column < width; ++column) {
    if (passing < strength && column == ranTupleColumns[passing]) {
      passing++;
      continue;
    }
    columnBases[column] = mersenne.next(options.symbolCount(column));
    newLine[column] = options.firstSymbol(column) + columnBases[column] - 1;
  }
  for (long column = 0, passing = 0; column < width; ++column) {
    if (passing < strength && column == ranTupleColumns[passing]) {
      passing++;
      continue;
    }
    const unsigned firstSymbol = options.firstSymbol(column);
    const unsigned lastSymbol = options.lastSymbol(column);
    while (true) {
      newLine[column]++;
      if (newLine[column] > lastSymbol) {
        newLine[column] = firstSymbol;
      }
      if (newLine[column] == firstSymbol + columnBases[column]) {
        // backtrack
        if (columnStarted[column]) {
          std::cout << "backtracking" << std::endl;
          columnStarted[column] = false;
          // assign it the value before starting
          newLine[column]--;
          column--;
          while (passing > 0 && column == ranTupleColumns[passing - 1]) {
            column--;
            passing--;
          }
          if (option_constrained_indicator[column]) {
            known.undoAppend();
          }
          // undo column++ of the "for" loop
          column--;
          // the var of parent column is now unabled
          break;
        } else {
          columnStarted[column] = true;
        }
      }
      if (option_constrained_indicator[column]) {
        known.append(InputTerm(false, newLine[column]));
        if (satSolver(known)) {
          break;
        }
        known.undoAppend();
      } else {
        break;
      }
    }
  }
}

void CoveringArray::replaceRow(const unsigned lineIndex,
                               const unsigned encode) {
  std::vector<unsigned> &ranLine = array[lineIndex];
  const unsigned strength = specificationFile.getStrenth();
  std::vector<unsigned> tmpTuple(strength);
  // uncover the tuples
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < ranLine.size(); combinadic.next(columns)) {
    for (unsigned i = 0; i < strength; ++i) {
      tmpTuple[i] = ranLine[columns[i]];
    }
    uncover(coverage.encode(columns, tmpTuple), lineIndex);
  }
  produceSatRow(ranLine, encode);
  // cover the tuples
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < ranLine.size(); combinadic.next(columns)) {
    for (unsigned i = 0; i < strength; ++i) {
      tmpTuple[i] = ranLine[columns[i]];
    }
    cover(coverage.encode(columns, tmpTuple), lineIndex);
  }
  entryTabu.initialize(
      Entry(array.size(), specificationFile.getOptions().size()));
  validater.change_row(lineIndex, ranLine);
}

void CoveringArray::replaceRow2(const unsigned lineIndex,
                                const unsigned encode) {
  std::vector<unsigned> &ranLine = array[lineIndex];
  const unsigned strength = specificationFile.getStrenth();
  std::vector<unsigned> tmpTuple(strength);
  // uncover the tuples
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < ranLine.size(); combinadic.next(columns)) {
    for (unsigned i = 0; i < strength; ++i) {
      tmpTuple[i] = ranLine[columns[i]];
    }
    uncover(coverage.encode(columns, tmpTuple), lineIndex);
  }

  const std::vector<unsigned> &target_tuple = coverage.getTuple(encode);
  const std::vector<unsigned> &target_columns = coverage.getColumns(encode);
  for (auto &line : bestArray) {
    bool match = true;
    for (size_t i = 0; i < target_columns.size(); ++i) {
      if (line[target_columns[i]] != target_tuple[i]) {
        match = false;
        break;
      }
    }
    if (match) {
      ranLine = line;
      break;
    }
  }

  // cover the tuples
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < ranLine.size(); combinadic.next(columns)) {
    for (unsigned i = 0; i < strength; ++i) {
      tmpTuple[i] = ranLine[columns[i]];
    }
    cover(coverage.encode(columns, tmpTuple), lineIndex);
  }
  entryTabu.initialize(
      Entry(array.size(), specificationFile.getOptions().size()));
  validater.change_row(lineIndex, ranLine);
}

void CoveringArray::removeUselessRows2() {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  std::vector<unsigned> tmpTuple(strength);

  for (size_t lineIndex = 0; lineIndex < array.size();) {
    if (oneCoveredTuples.oneCoveredCount(lineIndex) == 0) {
      const std::vector<unsigned> &line = array[lineIndex];
      for (std::vector<unsigned> columns = combinadic.begin(strength);
           columns[strength - 1] < options.size(); combinadic.next(columns)) {
        for (unsigned i = 0; i < strength; ++i) {
          tmpTuple[i] = line[columns[i]];
        }
        unsigned encode = coverage.encode(columns, tmpTuple);
        uncover(encode, lineIndex);
      }
      std::swap(array[lineIndex], array[array.size() - 1]);
      for (auto &entry : entryTabu) {
        if (entry.getRow() == array.size() - 1) {
          entry.setRow(lineIndex);
        }
        if (entry.getRow() == lineIndex) {
          entry.setRow(array.size() - 1);
        }
      }
      validater.exchange_row(lineIndex, array.size() - 1);
      validater.pop_back_row();
      oneCoveredTuples.exchange_row(lineIndex, array.size() - 1);
      oneCoveredTuples.pop_back_row();
      array.pop_back();
    } else {
      ++lineIndex;
    }
  }
}

void CoveringArray::removeUselessRows() {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  std::vector<unsigned> tmpTuple(strength);

  for (unsigned i = 0; i < array.size(); ++i) {
    bool useless = true;
    const std::vector<unsigned> &line = array[i];
    for (std::vector<unsigned> columns = combinadic.begin(strength);
         columns[strength - 1] < options.size(); combinadic.next(columns)) {
      for (unsigned j = 0; j < strength; ++j) {
        tmpTuple[j] = line[columns[j]];
      }
      unsigned encode = coverage.encode(columns, tmpTuple);
      if (coverage.coverCount(encode) == 1) {
        useless = false;
        break;
      }
    }
    if (useless) {
      for (std::vector<unsigned> columns = combinadic.begin(strength);
           columns[strength - 1] < options.size(); combinadic.next(columns)) {
        for (unsigned j = 0; j < strength; ++j) {
          tmpTuple[j] = line[columns[j]];
        }
        unsigned encode = coverage.encode(columns, tmpTuple);
        uncover(encode, i);
      }
      std::swap(array[i], array[array.size() - 1]);
      for (auto &entry : entryTabu) {
        if (entry.getRow() == array.size() - 1) {
          entry.setRow(i);
        }
        if (entry.getRow() == i) {
          entry.setRow(array.size() - 1);
        }
      }
      validater.exchange_row(i, array.size() - 1);
      validater.pop_back_row();
      oneCoveredTuples.exchange_row(i, array.size() - 1);
      oneCoveredTuples.pop_back_row();
      array.pop_back();
      --i;
    }
  }
}

void CoveringArray::removeOneRow() {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  unsigned minUncoverCount = std::numeric_limits<unsigned>::max();
  std::vector<unsigned> bestRowIndex;
  std::vector<unsigned> tmpTuple(strength);
  clock_t start = clock();
  for (unsigned i = 0; i < array.size(); ++i) {
    unsigned uncoverCount = 0;
    const std::vector<unsigned> &line = array[i];
    for (std::vector<unsigned> columns = combinadic.begin(strength);
         columns[strength - 1] < options.size(); combinadic.next(columns)) {
      for (unsigned j = 0; j < strength; ++j) {
        tmpTuple[j] = line[columns[j]];
      }
      unsigned encode = coverage.encode(columns, tmpTuple);
      if (coverage.coverCount(encode) == 1) {
        uncoverCount++;
      }
    }
    if (uncoverCount < minUncoverCount) {
      minUncoverCount = uncoverCount;
      bestRowIndex.clear();
      bestRowIndex.push_back(i);
    } else if (uncoverCount == minUncoverCount) {
      bestRowIndex.push_back(i);
    }
  }
  std::cout << "select time: " << double(clock() - start) / CLOCKS_PER_SEC
            << std::endl;

  unsigned rowToremoveIndex = bestRowIndex[mersenne.next(bestRowIndex.size())];
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < options.size(); combinadic.next(columns)) {
    for (unsigned j = 0; j < strength; ++j) {
      tmpTuple[j] = array[rowToremoveIndex][columns[j]];
    }
    unsigned encode = coverage.encode(columns, tmpTuple);
    uncover(encode, rowToremoveIndex);
  }

  std::swap(array[array.size() - 1], array[rowToremoveIndex]);
  validater.exchange_row(rowToremoveIndex, array.size() - 1);
  validater.pop_back_row();
  for (auto &entry : entryTabu) {
    if (entry.getRow() == array.size() - 1) {
      entry.setRow(rowToremoveIndex);
    }
    if (entry.getRow() == rowToremoveIndex) {
      entry.setRow(array.size() - 1);
    }
  }
  array.pop_back();
}

void CoveringArray::removeOneRowGreedy() {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();

  unsigned rowToremoveIndex = 0;
  unsigned minOneCoveredCount = oneCoveredTuples.oneCoveredCount(0);
  for (size_t lineIndex = 1; lineIndex < array.size(); ++lineIndex) {
    unsigned oneCoveredCount = oneCoveredTuples.oneCoveredCount(lineIndex);
    if (minOneCoveredCount > oneCoveredCount) {
      minOneCoveredCount = oneCoveredCount;
      rowToremoveIndex = lineIndex;
    }
  }

  std::vector<unsigned> tmpTuple(strength);
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < options.size(); combinadic.next(columns)) {
    for (unsigned j = 0; j < strength; ++j) {
      tmpTuple[j] = array[rowToremoveIndex][columns[j]];
    }
    unsigned encode = coverage.encode(columns, tmpTuple);
    uncover(encode, rowToremoveIndex);
  }

  std::swap(array[array.size() - 1], array[rowToremoveIndex]);
  validater.exchange_row(rowToremoveIndex, array.size() - 1);
  validater.pop_back_row();
  oneCoveredTuples.exchange_row(rowToremoveIndex, array.size() - 1);
  oneCoveredTuples.pop_back_row();
  for (auto &entry : entryTabu) {
    if (entry.getRow() == array.size() - 1) {
      entry.setRow(rowToremoveIndex);
    }
    if (entry.getRow() == rowToremoveIndex) {
      entry.setRow(array.size() - 1);
    }
  }
  array.pop_back();
}

void CoveringArray::removeOneRowRandom() {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();

  unsigned rowToremoveIndex = mersenne.next(array.size());
  std::vector<unsigned> tmpTuple(strength);
  for (std::vector<unsigned> columns = combinadic.begin(strength);
       columns[strength - 1] < options.size(); combinadic.next(columns)) {
    for (unsigned j = 0; j < strength; ++j) {
      tmpTuple[j] = array[rowToremoveIndex][columns[j]];
    }
    unsigned encode = coverage.encode(columns, tmpTuple);
    uncover(encode, rowToremoveIndex);
  }

  std::swap(array[array.size() - 1], array[rowToremoveIndex]);
  validater.exchange_row(rowToremoveIndex, array.size() - 1);
  validater.pop_back_row();
  oneCoveredTuples.exchange_row(rowToremoveIndex, array.size() - 1);
  oneCoveredTuples.pop_back_row();
  for (auto &entry : entryTabu) {
    if (entry.getRow() == array.size() - 1) {
      entry.setRow(rowToremoveIndex);
    }
    if (entry.getRow() == rowToremoveIndex) {
      entry.setRow(array.size() - 1);
    }
  }
  array.pop_back();
}

void CoveringArray::optimize() {

  while (true) {
    if ((double)(clock() - clock_start) / CLOCKS_PER_SEC > maxTime) {
      break;
    }
    if (uncoveredTuples.size() == 0) {
      removeUselessRows2();
      bestArray = array;
      tmpPrint();
      //			removeOneRow();
      removeOneRowRandom();
      // removeOneRowGreedy();
    }

    //		clock_t start = clock();
    // tabuStep();
    //		tabuStep2();
    //		tabuStep3();
    // tabuStep4();
    // tabuZero();
    tabugw();
    //		std::cout << "tabuStep time: " <<  double (clock() - start) /
    // CLOCKS_PER_SEC << std::endl;

    step++;
    continue;
  }

  if (uncoveredTuples.size() == 0) {
    removeUselessRows2();
    bestArray = array;
    tmpPrint();
  }

  if (!verify(bestArray)) {
    std::cout << "wrong answer!!!!!" << std::endl;
    return;
  }
  // std::cout << "total steps: " << step << std::endl;

  //#ifndef NDEBUG
  std::cerr << "********Debuging CoveringArray::optimize*********" << std::endl;
  //	std::cerr << "printing bestArray..." << std::endl;
  //	for (unsigned i = 0; i < bestArray.size(); ++i) {
  //		std::cerr << i << "th  ";
  //		for (auto x : bestArray[i]) {
  //			std::cerr << ' ' << x;
  //		}
  //		std::cerr << std::endl;
  //	}
  std::cerr << "total size : " << bestArray.size() << std::endl;
  std::cerr << "********End of Debuing CoveringArray::optimize********"
            << std::endl;
  //#endif
}

void CoveringArray::tabuZero() {
  const unsigned tupleEncode =
      uncoveredTuples.encode(mersenne.next(uncoveredTuples.size()));
  const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
  const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
  if (mersenne.next(1000) < 1) {
    replaceRow2(mersenne.next(array.size()), tupleEncode);
    return;
  }
  std::vector<unsigned> bestRows;
  std::vector<unsigned> bestVars;
  long long bestScore = std::numeric_limits<long long>::min();
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    std::vector<unsigned> &line = array[lineIndex];
    unsigned diffCount = 0;
    unsigned diffVar;
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        diffCount++;
        diffVar = tuple[i];
      }
    }
    if (diffCount > 1) {
      continue;
    }
    unsigned diffOption = specificationFile.getOptions().option(diffVar);
    // Tabu
    if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
      continue;
    }
    // my check
    if (!validater.valida_change(lineIndex, diffOption, line[diffOption],
                                 diffVar)) {
      continue;
    }
    long long tmpScore = varScoreOfRow2(diffVar, lineIndex);
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
      bestVars.clear();
      bestVars.push_back(diffVar);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
      bestVars.push_back(diffVar);
    }
  }
  if (bestRows.size() != 0) {
    unsigned ran = mersenne.next(bestRows.size());
    replace(bestVars[ran], bestRows[ran]);
    return;
  }
  replaceRow2(mersenne.next(array.size()), tupleEncode);
}

void CoveringArray::tabugw() {
  unsigned base = mersenne.next(uncoveredTuples.size());
  std::vector<unsigned> firstBestRows;
  std::vector<unsigned> firstBestVars;
  std::vector<unsigned> bestRows;
  std::vector<unsigned> bestVars;
  long long bestScore = std::numeric_limits<long long>::min();
  for (size_t i = 0; i < uncoveredTuples.size(); ++i) {
    const unsigned tupleEncode =
        uncoveredTuples.encode((base + i) % uncoveredTuples.size());
    const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
    const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
    for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
      std::vector<unsigned> &line = array[lineIndex];
      unsigned diffCount = 0;
      unsigned diffVar;
      for (unsigned i = 0; i < tuple.size(); ++i) {
        if (line[columns[i]] != tuple[i]) {
          diffCount++;
          diffVar = tuple[i];
        }
      }
      if (diffCount > 1) {
        continue;
      }
      unsigned diffOption = specificationFile.getOptions().option(diffVar);
      // Tabu
      if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
        continue;
      }
      // my check
      if (!validater.valida_change(lineIndex, diffOption, line[diffOption],
                                   diffVar)) {
        continue;
      }
      long long tmpScore = varScoreOfRow3(diffVar, lineIndex);
      if (bestScore < tmpScore) {
        bestScore = tmpScore;
        if (i == 0) {
          firstBestRows.clear();
          firstBestRows.push_back(lineIndex);
          firstBestVars.clear();
          firstBestVars.push_back(diffVar);
        }
        bestRows.clear();
        bestRows.push_back(lineIndex);
        bestVars.clear();
        bestVars.push_back(diffVar);
      } else if (bestScore == tmpScore) {
        bestRows.push_back(lineIndex);
        bestVars.push_back(diffVar);
        if (i == 0) {
          firstBestRows.push_back(lineIndex);
          firstBestVars.push_back(diffVar);
        }
      }
    }
  }
  if (bestScore > 0) {
    unsigned ran = mersenne.next(bestRows.size());
    replace(bestVars[ran], bestRows[ran]);
    return;
  }

  if (mersenne.nextClosed() < 0.001) {
    replaceRow2(mersenne.next(array.size()), uncoveredTuples.encode(base));
    return;
  }
  if (firstBestRows.size() != 0) {
    unsigned ran = mersenne.next(firstBestRows.size());
    replace(firstBestVars[ran], firstBestRows[ran]);
    return;
  }

  const unsigned tupleEncode =
      uncoveredTuples.encode(base % uncoveredTuples.size());
  const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
  const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);

  bestRows.clear();

  std::vector<unsigned> changedVars;
  std::vector<unsigned> org_vars;
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    org_vars.clear();
    changedVars.clear();
    std::vector<unsigned> &line = array[lineIndex];
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        org_vars.push_back(line[columns[i]]);
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() == 0) {
      continue;
    }
    // Tabu
    bool isTabu = false;
    for (auto v : changedVars) {
      unsigned diffOption = specificationFile.getOptions().option(v);
      if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
        isTabu = true;
        break;
      }
    }
    if (isTabu) {
      continue;
    }
    // check constraint, before tmpScore or after it?
    bool need_to_check = false;
    for (auto v : changedVars) {
      const Options &options = specificationFile.getOptions();
      if (option_constrained_indicator[options.option(v)]) {
        need_to_check = true;
        break;
      }
    }

    // my check
    if (need_to_check && !validater.valida_row(array[lineIndex], changedVars)) {
      continue;
    }
    // greedy
    long long tmpScore;
    if (changedVars.size() > 1) {
      tmpScore = multiVarScoreOfRow2(changedVars, lineIndex);
    } else {
      tmpScore = varScoreOfRow3(changedVars[0], lineIndex);
    }
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
    }
  }
  // need to handle when bestRows.size() == 0
  if (bestRows.size() != 0) {
    // std::cout << "change_mutivar" << std::endl;
    unsigned lineIndex = bestRows[mersenne.next(bestRows.size())];
    changedVars.clear();
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (array[lineIndex][columns[i]] != tuple[i]) {
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() > 1) {
      // multiVarReplace(changedVars, lineIndex);
      for (auto v : changedVars) {
        replace(v, lineIndex);
      }
    } else {
      replace(changedVars[0], lineIndex);
    }
    return;
  }
  replaceRow2(mersenne.next(array.size()), tupleEncode);
}

void CoveringArray::tabuStep() {
  const unsigned tupleEncode =
      uncoveredTuples.encode(mersenne.next(uncoveredTuples.size()));
  const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
  const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
  if (mersenne.next(1000) < 1) {
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }
  std::vector<unsigned> bestRows;
  std::vector<unsigned> bestVars;
  //	unsigned count = 0;
  long long bestScore = std::numeric_limits<long long>::min();
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    std::vector<unsigned> &line = array[lineIndex];
    unsigned diffCount = 0;
    unsigned diffVar;
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        diffCount++;
        diffVar = tuple[i];
      }
    }
    if (diffCount > 1) {
      continue;
    }
    unsigned diffOption = specificationFile.getOptions().option(diffVar);
    // Tabu
    if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
      continue;
    }
    // check if the new assignment will follow the constraints
    // if (option_constrained_indicator[diffOption]) {
    //   InputKnown known;
    //   for (unsigned i = 0; i < line.size(); ++i) {
    //     if (i == diffOption) {
    //       known.append(InputTerm(false, diffVar));
    //     } else {
    //       known.append(InputTerm(false, line[i]));
    //     }
    //   }
    //   if (satSolver(known) !=
    //       validater.valida_change(lineIndex, diffOption, line[diffOption],
    //                               diffVar)) {
    //     //			validater.print(array);
    //     std::cout << "lineIndex: " << lineIndex << std::endl;
    //     for (auto v : array[lineIndex]) {
    //       std::cout << v << ' ';
    //     }
    //     std::cout << std::endl << "new_var: " << diffVar << std::endl;
    //     exit(0);
    //   }
    //   if (!satSolver(known)) {
    //     continue;
    //   }
    // }
    // my check
    if (!validater.valida_change(lineIndex, diffOption, line[diffOption],
                                 diffVar)) {
      continue;
    }
    long long tmpScore = varScoreOfRow3(diffVar, lineIndex);
    // long long tmpScore2 = varScoreOfRow(diffVar, lineIndex);
    // if (tmpScore != tmpScore2) {
    //   std::cout << "abort" << std::endl;
    //   abort();
    // }
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
      bestVars.clear();
      bestVars.push_back(diffVar);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
      bestVars.push_back(diffVar);
    }
  }
  if (bestRows.size() != 0) {
    //		std::cout << "tabuStep: valueRow.size() = " << count  <<
    //"\tbestRows.size() = " << bestRows.size() << "\tbestScore = " <<
    // bestScore << std::endl;
    unsigned ran = mersenne.next(bestRows.size());
    replace(bestVars[ran], bestRows[ran]);
    return;
  }

  if (mersenne.next(100) < 1) {
    // std::cout << "second replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }

  std::vector<unsigned> changedVars;
  std::vector<unsigned> org_vars;
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    org_vars.clear();
    changedVars.clear();
    std::vector<unsigned> &line = array[lineIndex];
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        org_vars.push_back(line[columns[i]]);
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() == 0) {
      continue;
    }
    // Tabu
    bool isTabu = true;
    for (auto v : changedVars) {
      unsigned diffOption = specificationFile.getOptions().option(v);
      if (!entryTabu.isTabu(Entry(lineIndex, diffOption))) {
        isTabu = false;
        break;
      }
    }
    if (isTabu) {
      continue;
    }
    // check constraint, before tmpScore or after it?
    bool need_to_check = false;
    for (auto v : changedVars) {
      const Options &options = specificationFile.getOptions();
      if (option_constrained_indicator[options.option(v)]) {
        need_to_check = true;
        break;
      }
    }
    // if (need_to_check) {
    //   InputKnown known;
    //   for (unsigned column = 0, passing = 0; column < line.size(); ++column)
    //   {
    //     if (passing < tuple.size() && column == columns[passing]) {
    //       known.append(InputTerm(false, tuple[passing++]));
    //     } else {
    //       known.append(InputTerm(false, line[column]));
    //     }
    //   }
    //   if (satSolver(known) !=
    //       validater.valida_row(array[lineIndex], changedVars)) {
    //
    //     // validater.print(array);
    //     std::cout << "check error lineIndex: " << lineIndex << std::endl;
    //     for (auto v : array[lineIndex]) {
    //       std::cout << v << ' ';
    //     }
    //     std::cout << std::endl << "new_vars: ";
    //     for (auto v : changedVars) {
    //       std::cout << v << ' ';
    //     }
    //     std::cout << std::endl;
    //     abort();
    //   }
    //   if (!satSolver(known)) {
    //     continue;
    //   }
    // }

    // my check
    if (need_to_check && !validater.valida_row(array[lineIndex], changedVars)) {
      continue;
    }
    // greedy
    long long tmpScore;
    if (changedVars.size() > 1) {
      tmpScore = multiVarScoreOfRow2(changedVars, lineIndex);
    } else {
      tmpScore = varScoreOfRow2(changedVars[0], lineIndex);
    }
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
    }
  }
  // need to handle when bestRows.size() == 0
  if (bestRows.size() != 0) {
    // std::cout << "change_mutivar" << std::endl;
    unsigned lineIndex = bestRows[mersenne.next(bestRows.size())];
    changedVars.clear();
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (array[lineIndex][columns[i]] != tuple[i]) {
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() > 1) {
      // multiVarReplace(changedVars, lineIndex);
      for (auto v : changedVars) {
        replace(v, lineIndex);
      }
    } else {
      replace(changedVars[0], lineIndex);
    }
    return;
  }
  // std::cout << "third replaceRow" << std::endl;
  // std::cout << bestScore << std::endl;
  replaceRow(mersenne.next(array.size()), tupleEncode);
}

void CoveringArray::tabuStep2() {
  const unsigned tupleEncode =
      uncoveredTuples.encode(mersenne.next(uncoveredTuples.size()));

  const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
  const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
  if (mersenne.next(1000) < 1) {
    //		std::cout << "first replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }
  std::vector<unsigned> bestRows;
  std::vector<unsigned> bestVars;
  long long bestScore = std::numeric_limits<long long>::min();
  for (unsigned i = 0, base = mersenne.next(array.size()); i < array.size();
       ++i) {
    unsigned lineIndex = (base + i) % array.size();
    std::vector<unsigned> &line = array[lineIndex];
    unsigned diffCount = 0;
    unsigned diffVar;
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        diffCount++;
        diffVar = tuple[i];
      }
    }
    if (diffCount > 1) {
      continue;
    }
    unsigned diffOption = specificationFile.getOptions().option(diffVar);
    // Tabu
    if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
      continue;
    }
    // check if the new assignment will follow the constraints
    //		if (option_constrained_indicator[diffOption]) {
    //			InputKnown known;
    //			for (unsigned i = 0; i < line.size(); ++i) {
    //				if (i == diffOption) {
    //					known.append(InputTerm(false, diffVar));
    //				}
    //				else {
    //					known.append(InputTerm(false, line[i]));
    //				}
    //			}
    //			if (!satSolver(known)) {
    //				continue;
    //			}
    //		}
    //		if (satSolver(known) != validater.valida_change(lineIndex,
    // diffOption,
    // line[diffOption], diffVar)) {
    //
    ////			validater.print(array);
    //			std::cout << "lineIndex: " << lineIndex << std::endl;
    //			for (auto v : array[lineIndex]) {
    //				std::cout << v << ' ';
    //			}
    //			std::cout << std::endl << "new_var: " << diffVar <<
    // std::endl;
    //			exit(0);
    //		}
    // my check
    if (!validater.valida_change(lineIndex, diffOption, line[diffOption],
                                 diffVar)) {
      continue;
    }
    long long tmpScore = varScoreOfRow(diffVar, lineIndex);
    //		if (tmpScore > -1) {
    //			replace(diffVar, lineIndex);
    //			return;
    //		}
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
      bestVars.clear();
      bestVars.push_back(diffVar);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
      bestVars.push_back(diffVar);
    }
  }
  if (bestRows.size() != 0) {
    unsigned ran = mersenne.next(bestRows.size());
    replace(bestVars[ran], bestRows[ran]);
    return;
  }

  if (mersenne.next(100) < 1) {
    std::cout << "second replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }

  std::vector<unsigned> changedVars;
  std::vector<unsigned> org_vars;
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    org_vars.clear();
    changedVars.clear();
    std::vector<unsigned> &line = array[lineIndex];
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        org_vars.push_back(line[columns[i]]);
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() == 0) {
      continue;
    }
    // check constraint, before tmpScore or after it?
    bool need_to_check = false;
    for (auto v : changedVars) {
      const Options &options = specificationFile.getOptions();
      if (option_constrained_indicator[options.option(v)]) {
        need_to_check = true;
        break;
      }
    }
    // my check
    if (need_to_check && !validater.valida_row(array[lineIndex], changedVars)) {
      continue;
    }
    // greedy
    long long tmpScore = multiVarScoreOfRow(changedVars, lineIndex);
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
    }
  }
  // need to handle when bestRows.size() == 0
  if (bestRows.size() != 0) {
    unsigned lineIndex = bestRows[mersenne.next(bestRows.size())];
    changedVars.clear();
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (array[lineIndex][columns[i]] != tuple[i]) {
        changedVars.push_back(tuple[i]);
      }
    }
    std::cout << "multiVarReplace" << std::endl;
    multiVarReplace(changedVars, lineIndex);
    return;
  }
  std::cout << "third replaceRow" << std::endl;
  replaceRow(mersenne.next(array.size()), tupleEncode);
}

// give priority to candidates with score > 0
void CoveringArray::tabuStep3() {
  const unsigned tupleEncode =
      uncoveredTuples.encode(mersenne.next(uncoveredTuples.size()));

  const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
  const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
  if (mersenne.next(1000) < 1) {
    std::cout << "first replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }
  std::vector<unsigned> bestRows;
  std::vector<unsigned> bestVars;
  long long bestScore = std::numeric_limits<long long>::min();
  for (unsigned i = 0, base = mersenne.next(array.size()); i < array.size();
       ++i) {
    unsigned lineIndex = (base + i) % array.size();
    std::vector<unsigned> &line = array[lineIndex];
    unsigned diffCount = 0;
    unsigned diffVar;
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        diffCount++;
        diffVar = tuple[i];
      }
    }
    if (diffCount > 1) {
      continue;
    }
    unsigned diffOption = specificationFile.getOptions().option(diffVar);
    // Tabu
    if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
      continue;
    }
    // my check
    if (!validater.valida_change(lineIndex, diffOption, line[diffOption],
                                 diffVar)) {
      continue;
    }
    long long tmpScore = varScoreOfRow(diffVar, lineIndex);
    if (tmpScore > 0) {
      //			std::cout << "score = " << tmpScore <<
      // std::endl;
      replace(diffVar, lineIndex);
      return;
    }
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
      bestVars.clear();
      bestVars.push_back(diffVar);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
      bestVars.push_back(diffVar);
    }
  }
  if (bestRows.size() != 0) {
    //		std::cout << "score = " << bestScore << std::endl;
    unsigned ran = mersenne.next(bestRows.size());
    replace(bestVars[ran], bestRows[ran]);
    return;
  }

  if (mersenne.next(100) < 1) {
    std::cout << "second replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }

  std::vector<unsigned> changedVars;
  std::vector<unsigned> org_vars;
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    org_vars.clear();
    changedVars.clear();
    std::vector<unsigned> &line = array[lineIndex];
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        org_vars.push_back(line[columns[i]]);
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() == 0) {
      continue;
    }
    // check constraint, before tmpScore or after it?
    bool need_to_check = false;
    for (auto v : changedVars) {
      const Options &options = specificationFile.getOptions();
      if (option_constrained_indicator[options.option(v)]) {
        need_to_check = true;
        break;
      }
    }
    // my check
    if (need_to_check && !validater.valida_row(array[lineIndex], changedVars)) {
      continue;
    }
    // greedy
    long long tmpScore = multiVarScoreOfRow(changedVars, lineIndex);
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
    }
  }
  // need to handle when bestRows.size() == 0
  if (bestRows.size() != 0) {
    unsigned lineIndex = bestRows[mersenne.next(bestRows.size())];
    changedVars.clear();
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (array[lineIndex][columns[i]] != tuple[i]) {
        changedVars.push_back(tuple[i]);
      }
    }
    std::cout << "multiVarReplace, score = " << bestScore << std::endl;
    multiVarReplace(changedVars, lineIndex);
    return;
  }
  std::cout << "third replaceRow" << std::endl;
  replaceRow(mersenne.next(array.size()), tupleEncode);
}

// candidate with score > 0; then randomly switch between all candidate and
// bestScore candidate
void CoveringArray::tabuStep4() {
  const unsigned tupleEncode =
      uncoveredTuples.encode(mersenne.next(uncoveredTuples.size()));

  const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
  const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
  if (mersenne.next(1000) < 1) {
    //		std::cout << "first replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }
  std::vector<unsigned> bestRows;
  std::vector<unsigned> bestVars;
  std::vector<unsigned> candRows;
  std::vector<unsigned> candVars;
  std::vector<long long> candScores;
  long long bestScore = std::numeric_limits<long long>::min();
  for (unsigned i = 0, base = mersenne.next(array.size()); i < array.size();
       ++i) {
    unsigned lineIndex = (base + i) % array.size();
    std::vector<unsigned> &line = array[lineIndex];
    unsigned diffCount = 0;
    unsigned diffVar;
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        diffCount++;
        diffVar = tuple[i];
      }
    }
    if (diffCount > 1) {
      continue;
    }
    unsigned diffOption = specificationFile.getOptions().option(diffVar);
    // my check
    if (!validater.valida_change(lineIndex, diffOption, line[diffOption],
                                 diffVar)) {
      continue;
    }
    candRows.push_back(lineIndex);
    candVars.push_back(diffVar);
    // Tabu
    if (entryTabu.isTabu(Entry(lineIndex, diffOption))) {
      continue;
    }
    // long long tmpScore = varScoreOfRow(diffVar, lineIndex);
    long long tmpScore = varScoreOfRow2(diffVar, lineIndex);
    // if (tmpScore != tmpScore2) {
    //   std::cerr << "error" << std::endl;
    //   abort();
    // }
    if (tmpScore > 0) {
      //			std::cout << "descend score: " << tmpScore <<
      // std::endl;
      replace(diffVar, lineIndex);
      return;
    }
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
      bestVars.clear();
      bestVars.push_back(diffVar);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
      bestVars.push_back(diffVar);
    }
    //		candRows.push_back(lineIndex);
    //		candVars.push_back(diffVar);
    //		candScores.push_back(tmpScore);
  }
  if (mersenne.next(1000) < 10) {
    // TODO: add tabu cand
    if (candRows.size() != 0) {
      unsigned ran = mersenne.next(candRows.size());
      //			std::cout << "random cand Score: " <<
      // candScores[ran] << "\tsize: " << candRows.size();
      //			std::cout << "\tbestScore" << bestScore <<
      //"\tsize: " << bestRows.size() << std::endl;
      replace(candVars[ran], candRows[ran]);
      return;
    }
  } else {
    if (bestRows.size() != 0) {
      //			std::cout << "best score: " << bestScore <<
      // std::endl;
      unsigned ran = mersenne.next(bestRows.size());
      replace(bestVars[ran], bestRows[ran]);
      return;
    }
  }

  if (mersenne.next(100) < 1) {
    //		std::cout << "second replaceRow" << std::endl;
    replaceRow(mersenne.next(array.size()), tupleEncode);
    return;
  }

  std::vector<unsigned> changedVars;
  std::vector<unsigned> org_vars;
  for (unsigned lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
    org_vars.clear();
    changedVars.clear();
    std::vector<unsigned> &line = array[lineIndex];
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        org_vars.push_back(line[columns[i]]);
        changedVars.push_back(tuple[i]);
      }
    }
    if (changedVars.size() == 0) {
      continue;
    }
    // check constraint, before tmpScore or after it?
    bool need_to_check = false;
    for (auto v : changedVars) {
      const Options &options = specificationFile.getOptions();
      if (option_constrained_indicator[options.option(v)]) {
        need_to_check = true;
        break;
      }
    }
    // my check
    if (need_to_check && !validater.valida_row(array[lineIndex], changedVars)) {
      continue;
    }
    // greedy
    long long tmpScore = multiVarScoreOfRow(changedVars, lineIndex);
    if (bestScore < tmpScore) {
      bestScore = tmpScore;
      bestRows.clear();
      bestRows.push_back(lineIndex);
    } else if (bestScore == tmpScore) {
      bestRows.push_back(lineIndex);
    }
  }
  // need to handle when bestRows.size() == 0
  if (bestRows.size() != 0) {
    unsigned lineIndex = bestRows[mersenne.next(bestRows.size())];
    changedVars.clear();
    for (unsigned i = 0; i < tuple.size(); ++i) {
      if (array[lineIndex][columns[i]] != tuple[i]) {
        changedVars.push_back(tuple[i]);
      }
    }
    //		std::cout << "multiVarReplace score: " << bestScore <<
    // std::endl;
    multiVarReplace(changedVars, lineIndex);
    return;
  }
  //	std::cout << "third replaceRow" << std::endl;
  replaceRow(mersenne.next(array.size()), tupleEncode);
}

long long
CoveringArray::multiVarScoreOfRow2(const std::vector<unsigned> &sortedMultiVars,
                                   const unsigned lineIndex) {
  const Options &options = specificationFile.getOptions();
  std::vector<unsigned> &line = array[lineIndex];
  std::vector<unsigned> changedColumns;
  for (auto var : sortedMultiVars) {
    int c = options.option(var);
    changedColumns.push_back(c);
  }

  long long coverChangeCount = 0;
  for (auto tupleEncode : uncoveredTuples) {
    const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
    const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);

    bool needChange = true;
    size_t i = 0, j = 0;
    while (i != columns.size() && j != changedColumns.size()) {
      if (columns[i] == changedColumns[j]) {
        if (tuple[i] != sortedMultiVars[j]) {
          needChange = false;
          break;
        }
        i++;
        j++;
      } else {
        if (columns[i] < changedColumns[j]) {
          if (tuple[i] != line[columns[i]]) {
            needChange = false;
            break;
          }
          i++;
        } else {
          j++;
        }
      }
    }
    for (; i != columns.size(); ++i) {
      if (tuple[i] != line[columns[i]]) {
        needChange = false;
        break;
      }
    }
    if (needChange) {
      coverChangeCount++;
    }
  }
  std::vector<long long> overlapCount(changedColumns.size() + 1, 0);
  for (auto cc : changedColumns) {
    for (auto &ecEntry : oneCoveredTuples.getECbyLineVar(lineIndex, line[cc])) {
      unsigned tupleEncode = ecEntry.encode;
      const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
      const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
      bool needChange = true;
      for (size_t i = 0; i < columns.size(); ++i) {
        if (line[columns[i]] != tuple[i]) {
          needChange = false;
          break;
        }
      }
      if (needChange) {
        long long olc = 0;
        size_t j = 0, k = 0;
        while (j != columns.size() && k != changedColumns.size()) {
          if (columns[j] == changedColumns[k]) {
            olc++;
            j++;
            k++;
          } else {
            columns[j] < changedColumns[k] ? j++ : k++;
          }
        }
        overlapCount[olc]++;
      }
    }
  }
  for (size_t olc = 1; olc < overlapCount.size(); ++olc) {
    coverChangeCount -= overlapCount[olc] / olc;
  }
  return coverChangeCount;
}

long long
CoveringArray::multiVarRow(const std::vector<unsigned> &sortedMultiVars,
                           const unsigned lineIndex, const bool change) {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  long long score = 0;

  std::vector<unsigned> varColumns;
  for (auto var : sortedMultiVars) {
    varColumns.push_back(options.option(var));
  }
  if (change) {
    for (auto column : varColumns) {
      entryTabu.insert(Entry(lineIndex, column));
    }
  }
  std::vector<unsigned> &line = array[lineIndex];

  // must from the end to the begining
  for (unsigned i = sortedMultiVars.size(); i--;) {
    std::swap(line[line.size() - sortedMultiVars.size() + i],
              line[varColumns[i]]);
  }

  std::vector<unsigned> tmpSortedColumns(strength);
  std::vector<unsigned> tmpSortedTupleToCover(strength);
  std::vector<unsigned> tmpSortedTupleToUncover(strength);
  unsigned tmpToCoverEncode;
  unsigned tmpToUncoverEncode;

  if (sortedMultiVars.size() >= strength) {
    for (std::vector<unsigned> changedColums = combinadic.begin(strength);
         changedColums[strength - 1] < sortedMultiVars.size();
         combinadic.next(changedColums)) {
      for (unsigned i = 0; i < strength; ++i) {
        tmpSortedTupleToCover[i] = sortedMultiVars[changedColums[i]];
        tmpSortedTupleToUncover[i] =
            line[line.size() - sortedMultiVars.size() + changedColums[i]];
      }
      std::sort(tmpSortedTupleToCover.begin(), tmpSortedTupleToCover.end());
      std::sort(tmpSortedTupleToUncover.begin(), tmpSortedTupleToUncover.end());
      for (unsigned i = 0; i < strength; ++i) {
        tmpSortedColumns[i] = options.option(tmpSortedTupleToCover[i]);
      }
      tmpToCoverEncode =
          coverage.encode(tmpSortedColumns, tmpSortedTupleToCover);
      tmpToUncoverEncode =
          coverage.encode(tmpSortedColumns, tmpSortedTupleToUncover);
      if (change) {
        cover(tmpToCoverEncode, lineIndex);
        uncover(tmpToUncoverEncode, lineIndex);
      } else {
        if (coverage.coverCount(tmpToCoverEncode) == 0) {
          ++score;
        }
        if (coverage.coverCount(tmpToUncoverEncode) == 1) {
          --score;
        }
      }
    }
  }

  for (unsigned curRelevantCount = 1,
                maxRelevantCount = std::min(
                    strength - 1,
                    (const unsigned)(line.size() - sortedMultiVars.size()));
       curRelevantCount <= maxRelevantCount; ++curRelevantCount) {
    for (std::vector<unsigned> relevantColumns =
             combinadic.begin(curRelevantCount);
         relevantColumns[curRelevantCount - 1] <
         line.size() - sortedMultiVars.size();
         combinadic.next(relevantColumns)) {
      for (std::vector<unsigned> changedColums =
               combinadic.begin(strength - curRelevantCount);
           changedColums[strength - curRelevantCount - 1] <
           sortedMultiVars.size();
           combinadic.next(changedColums)) {

        for (unsigned i = 0; i < curRelevantCount; ++i) {
          tmpSortedTupleToCover[i] = tmpSortedTupleToUncover[i] =
              line[relevantColumns[i]];
        }
        for (unsigned i = 0; i < strength - curRelevantCount; ++i) {
          tmpSortedTupleToCover[curRelevantCount + i] =
              sortedMultiVars[changedColums[i]];
          tmpSortedTupleToUncover[curRelevantCount + i] =
              line[line.size() - sortedMultiVars.size() + changedColums[i]];
        }
        std::sort(tmpSortedTupleToCover.begin(), tmpSortedTupleToCover.end());
        std::sort(tmpSortedTupleToUncover.begin(),
                  tmpSortedTupleToUncover.end());
        for (unsigned i = 0; i < strength; ++i) {
          tmpSortedColumns[i] = options.option(tmpSortedTupleToCover[i]);
        }
        tmpToCoverEncode =
            coverage.encode(tmpSortedColumns, tmpSortedTupleToCover);
        tmpToUncoverEncode =
            coverage.encode(tmpSortedColumns, tmpSortedTupleToUncover);
        if (change) {
          cover(tmpToCoverEncode, lineIndex);
          uncover(tmpToUncoverEncode, lineIndex);
        } else {
          if (coverage.coverCount(tmpToCoverEncode) == 0) {
            ++score;
          }
          if (coverage.coverCount(tmpToUncoverEncode) == 1) {
            --score;
          }
        }
      }
    }
  }
  // must from the begining to the end
  for (unsigned i = 0; i < sortedMultiVars.size(); ++i) {
    std::swap(line[line.size() - sortedMultiVars.size() + i],
              line[varColumns[i]]);
  }

  if (change) {
    for (unsigned i = 0; i < sortedMultiVars.size(); ++i) {
      line[varColumns[i]] = sortedMultiVars[i];
    }
  }
  return score;
}

long long
CoveringArray::multiVarScoreOfRow(const std::vector<unsigned> &sortedMultiVars,
                                  const unsigned lineIndex) {
  return multiVarRow(sortedMultiVars, lineIndex, false);
}
void CoveringArray::multiVarReplace(
    const std::vector<unsigned> &sortedMultiVars, const unsigned lineIndex) {
  const Options &options = specificationFile.getOptions();
  std::vector<unsigned> org_vars;
  for (auto var : sortedMultiVars) {
    auto option = options.option(var);
    org_vars.push_back(array[lineIndex][option]);
  }
  validater.change_mutivar(lineIndex, org_vars, sortedMultiVars);
  multiVarRow(sortedMultiVars, lineIndex, true);
}

long long CoveringArray::varScoreOfRow(const unsigned var,
                                       const unsigned lineIndex) {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  std::vector<unsigned> &line = array[lineIndex];
  const unsigned varOption = options.option(var);
  if (line[varOption] == var) {
    return 0;
  }
  std::swap(line[line.size() - 1], line[varOption]);

  long long coverChangeCount = 0;
  std::vector<unsigned> tmpSortedColumns(strength);
  std::vector<unsigned> tmpSortedTupleToCover(strength);
  std::vector<unsigned> tmpSortedTupleToUncover(strength);
  for (std::vector<unsigned> columns = combinadic.begin(strength - 1);
       columns[strength - 2] < line.size() - 1; combinadic.next(columns)) {
    for (unsigned i = 0; i < columns.size(); ++i) {
      tmpSortedTupleToUncover[i] = tmpSortedTupleToCover[i] = line[columns[i]];
    }
    tmpSortedTupleToCover[strength - 1] = var;
    tmpSortedTupleToUncover[strength - 1] = line[line.size() - 1];
    std::sort(tmpSortedTupleToCover.begin(), tmpSortedTupleToCover.end());
    std::sort(tmpSortedTupleToUncover.begin(), tmpSortedTupleToUncover.end());
    for (unsigned i = 0; i < tmpSortedTupleToCover.size(); ++i) {
      tmpSortedColumns[i] = options.option(tmpSortedTupleToCover[i]);
    }
    unsigned tmpTupleToCoverEncode =
        coverage.encode(tmpSortedColumns, tmpSortedTupleToCover);
    unsigned tmpTupleToUncoverEncode =
        coverage.encode(tmpSortedColumns, tmpSortedTupleToUncover);
    if (coverage.coverCount(tmpTupleToCoverEncode) == 0) {
      coverChangeCount++;
    }
    if (coverage.coverCount(tmpTupleToUncoverEncode) == 1) {
      coverChangeCount--;
    }
  }

  std::swap(line[line.size() - 1], line[varOption]);

  return coverChangeCount;
}

long long CoveringArray::varScoreOfRow2(const unsigned var,
                                        const unsigned lineIndex) {
  const Options &options = specificationFile.getOptions();
  std::vector<unsigned> &line = array[lineIndex];
  const unsigned varOption = options.option(var);
  if (line[varOption] == var) {
    return 0;
  }
  long long coverChangeCount = 0;
  for (auto tupleEncode : uncoveredTuples) {
    const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
    const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
    bool match = false;
    bool needChange = true;
    for (size_t i = 0; i < columns.size(); ++i) {
      if (columns[i] == varOption) {
        match = true;
        if (tuple[i] != var) {
          needChange = false;
          break;
        }
      } else if (line[columns[i]] != tuple[i]) {
        needChange = false;
        break;
      }
    }
    if (match && needChange) {
      coverChangeCount++;
    }
  }
  for (auto &ecEntry :
       oneCoveredTuples.getECbyLineVar(lineIndex, line[varOption])) {
    unsigned tupleEncode = ecEntry.encode;
    const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
    const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
    bool needChange = true;
    for (size_t i = 0; i < columns.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        needChange = false;
        break;
      }
    }
    if (needChange) {
      coverChangeCount--;
    }
  }
  return coverChangeCount;
}

long long CoveringArray::varScoreOfRow3(const unsigned var,
                                        const unsigned lineIndex) {
  const Options &options = specificationFile.getOptions();
  std::vector<unsigned> &line = array[lineIndex];
  const unsigned varOption = options.option(var);
  if (line[varOption] == var) {
    return 0;
  }
  long long coverChangeCount = 0;
  for (auto tupleEncode : uncoveredTuples) {
    const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
    const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
    bool match = false;
    bool needChange = true;
    for (size_t i = 0; i < columns.size(); ++i) {
      if (columns[i] == varOption) {
        match = true;
        if (tuple[i] != var) {
          needChange = false;
          break;
        }
      } else if (line[columns[i]] != tuple[i]) {
        needChange = false;
        break;
      }
    }
    if (match && needChange) {
      coverChangeCount++;
    }
  }
  return coverChangeCount -
         oneCoveredTuples.getECbyLineVar(lineIndex, line[varOption]).size();
  for (auto &ecEntry :
       oneCoveredTuples.getECbyLineVar(lineIndex, line[varOption])) {
    unsigned tupleEncode = ecEntry.encode;
    const std::vector<unsigned> &tuple = coverage.getTuple(tupleEncode);
    const std::vector<unsigned> &columns = coverage.getColumns(tupleEncode);
    bool needChange = true;
    for (size_t i = 0; i < columns.size(); ++i) {
      if (line[columns[i]] != tuple[i]) {
        needChange = false;
        break;
      }
    }
    if (needChange) {
      coverChangeCount--;
    }
  }
  return coverChangeCount;
}
bool CoveringArray::checkCovered(unsigned encode) {
  for (auto ec : uncoveredTuples) {
    if (ec == encode) {
      return true;
    }
  }
  return false;
}

// quite similar to varScoreOfRow function
void CoveringArray::replace(const unsigned var, const unsigned lineIndex) {
  const Options &options = specificationFile.getOptions();
  const unsigned strength = specificationFile.getStrenth();
  std::vector<unsigned> &line = array[lineIndex];
  const unsigned varOption = options.option(var);

  entryTabu.insert(Entry(lineIndex, varOption));

  if (line[varOption] == var) {
    return;
  }
  validater.change_var(lineIndex, varOption, line[varOption], var);
  std::swap(line[line.size() - 1], line[varOption]);

  std::vector<unsigned> tmpSortedColumns(strength);
  std::vector<unsigned> tmpSortedTupleToCover(strength);
  std::vector<unsigned> tmpSortedTupleToUncover(strength);
  for (std::vector<unsigned> columns = combinadic.begin(strength - 1);
       columns[strength - 2] < line.size() - 1; combinadic.next(columns)) {
    for (unsigned i = 0; i < columns.size(); ++i) {
      tmpSortedTupleToUncover[i] = tmpSortedTupleToCover[i] = line[columns[i]];
    }
    tmpSortedTupleToCover[strength - 1] = var;
    tmpSortedTupleToUncover[strength - 1] = line[line.size() - 1];
    std::sort(tmpSortedTupleToCover.begin(), tmpSortedTupleToCover.end());
    std::sort(tmpSortedTupleToUncover.begin(), tmpSortedTupleToUncover.end());
    for (unsigned i = 0; i < tmpSortedTupleToCover.size(); ++i) {
      tmpSortedColumns[i] = options.option(tmpSortedTupleToCover[i]);
    }
    unsigned tmpTupleToCoverEncode =
        coverage.encode(tmpSortedColumns, tmpSortedTupleToCover);
    unsigned tmpTupleToUncoverEncode =
        coverage.encode(tmpSortedColumns, tmpSortedTupleToUncover);
    // need not check coverCount, cover(encode) will do this
    cover(tmpTupleToCoverEncode, lineIndex);
    uncover(tmpTupleToUncoverEncode, lineIndex);
  }
  std::swap(line[line.size() - 1], line[varOption]);
  line[varOption] = var;
}

void CoveringArray::cover(const unsigned encode, const unsigned oldLineIndex) {
  coverage.cover(encode);
  unsigned coverCount = coverage.coverCount(encode);
  if (coverCount == 1) {
    uncoveredTuples.pop(encode);
    oneCoveredTuples.push(encode, oldLineIndex, coverage.getTuple(encode));
  }
  if (coverCount == 2) {
    const std::vector<unsigned> &tuple = coverage.getTuple(encode);
    const std::vector<unsigned> &columns = coverage.getColumns(encode);
    for (size_t lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
      if (lineIndex == oldLineIndex) {
        continue;
      }
      auto &line = array[lineIndex];
      bool match = true;
      for (size_t i = 0; i < columns.size(); ++i) {
        if (tuple[i] != line[columns[i]]) {
          match = false;
          break;
        }
      }
      if (match) {
        oneCoveredTuples.pop(encode, lineIndex, tuple);
        break;
      }
    }
  }
}

void CoveringArray::uncover(const unsigned encode,
                            const unsigned oldLineIndex) {
  coverage.uncover(encode);
  unsigned coverCount = coverage.coverCount(encode);
  if (coverCount == 0) {
    uncoveredTuples.push(encode);
    oneCoveredTuples.pop(encode, oldLineIndex, coverage.getTuple(encode));
  }
  if (coverCount == 1) {
    const std::vector<unsigned> &tuple = coverage.getTuple(encode);
    const std::vector<unsigned> &columns = coverage.getColumns(encode);
    for (size_t lineIndex = 0; lineIndex < array.size(); ++lineIndex) {
      if (lineIndex == oldLineIndex) {
        continue;
      }
      auto &line = array[lineIndex];
      bool match = true;
      for (size_t i = 0; i < columns.size(); ++i) {
        if (tuple[i] != line[columns[i]]) {
          match = false;
          break;
        }
      }
      if (match) {
        oneCoveredTuples.push(encode, lineIndex, tuple);
        break;
      }
    }
  }
}

void CoveringArray::tmpPrint() {
  std::cout << (double)(clock() - clock_start) / CLOCKS_PER_SEC << '\t'
            << array.size() << '\t' << step << std::endl;
}

bool CoveringArray::verify(
    const std::vector<std::vector<unsigned>> &resultArray) {
  const unsigned strength = specificationFile.getStrenth();
  const Options &options = specificationFile.getOptions();
  Coverage tmpCoverage(specificationFile);
  tmpCoverage.initialize(satSolver, option_constrained_indicator);

  std::vector<unsigned> tuple(strength);
  unsigned lineIndex = 0;
  for (auto &line : resultArray) {
    for (unsigned column = 0; column < line.size(); ++column) {
      if (line[column] < options.firstSymbol(column) ||
          line[column] > options.lastSymbol(column)) {
        std::cerr << "error line: " << lineIndex;
        std::cerr << " option: " << column << std::endl;
        std::cerr << "should be " << options.firstSymbol(column)
                  << " <= var <= " << options.lastSymbol(column) << std::endl;
        for (auto var : line) {
          std::cerr << var << ' ';
        }
        std::cerr << std::endl;
        return false;
      }
    }
    for (std::vector<unsigned> columns = combinadic.begin(strength);
         columns[strength - 1] < line.size(); combinadic.next(columns)) {
      for (unsigned i = 0; i < strength; ++i) {
        tuple[i] = line[columns[i]];
      }
      unsigned encode = tmpCoverage.encode(columns, tuple);
      if (tmpCoverage.coverCount(encode) < 0) {
        std::cerr << "violate constraints" << std::endl;
        return false;
      }
      tmpCoverage.cover(encode);
    }
    ++lineIndex;
  }
  return tmpCoverage.allIsCovered();
}
