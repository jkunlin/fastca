#ifndef VALID_CHECK_H_ONAP2ASG
#define VALID_CHECK_H_ONAP2ASG

#include "SpecificationFile.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace Valid {

#define NVISIBLE

class Literal {
private:
  bool neg;
  int var;

public:
  Literal() {}
  Literal(bool is_negative, int v) : neg(is_negative), var(v) {}

  void assign(bool is_negative, int v) {
    neg = is_negative;
    var = v;
  }
  //			const Literal & operator = (const Literal &other) {
  //				neg = other.neg;
  //				var = other.var;
  //				return *this;
  //			}
  bool is_negative() { return neg; }
  bool is_negative() const { return neg; }
  unsigned int variable() { return var; }
  unsigned int variable() const { return var; }
  int operator<(const Literal &other) const { return var < other.var; }
  bool operator==(const Literal &other) const {
    return neg == other.neg && var == other.var;
  }

#ifndef NVISIBLE
  void print() const {
    if (neg) {
      std::cout << '-';
    }
    std::cout << var;
  }
#endif
};

typedef std::vector<Literal> Clause;

typedef std::vector<Clause> Formula;

class Validater {
private:
  using Formula_size_type = std::vector<Clause>::size_type;
  const SpecificationFile &specificationFile;
  std::vector<std::vector<Formula_size_type>> option_clasue;
  Formula formula;

public:
  Validater(const SpecificationFile &specificationFile)
      : specificationFile(specificationFile) {
    option_clasue.resize(specificationFile.getOptions().size());
  }

  bool valida_changes(std::vector<unsigned> &row,
                      std::vector<unsigned> &changedVars) {
    const Options &options = specificationFile.getOptions();
    for (size_t i = 0; i < changedVars.size(); ++i) {
      unsigned opt = options.option(changedVars[i]);
      std::swap(row[opt], changedVars[i]);
    }
    bool sat = valida_row(row);
    for (size_t i = 0; i < changedVars.size(); ++i) {
      unsigned opt = options.option(changedVars[i]);
      std::swap(row[opt], changedVars[i]);
    }
    return sat;
  }

  bool valida_change(std::vector<unsigned> &row, unsigned changedVar) {
    const Options &options = specificationFile.getOptions();
    unsigned changeOpt = options.option(changedVar);

    std::swap(row[changeOpt], changedVar);
    bool sat = valida_row(row);
    std::swap(row[changeOpt], changedVar);

    return sat;
  }

  bool valida_row(std::vector<unsigned> &row) {
    const Options &options = specificationFile.getOptions();
    bool sat = true;
    for (auto cl : formula) {
      bool cl_unsat = true;
      for (auto lt : cl) {
        unsigned v = lt.variable();
        unsigned opt = options.option(v);
        if (lt.is_negative()) {
          if (v != row[opt]) {
            cl_unsat = false;
            break;
          }
        } else {
          if (v == row[opt]) {
            cl_unsat = false;
            break;
          }
        }
      }
      if (cl_unsat) {
        sat = false;
        break;
      }
    }
    return sat;
  }

  // TODO: same variables in one clause, such as "a != 0 || a != 1"
  void addClause(Clause &&c) {
    std::sort(c.begin(), c.end());
    if (c.empty()) {
      return;
    }
    const Options &options = specificationFile.getOptions();
    Formula_size_type index = formula.size();
    for (const Literal &l : c) {
      option_clasue[options.option(l.variable())].push_back(index);
    }
    formula.push_back(c);
  }

  void addClause(const Clause &c) {
    formula.push_back(c);
    Formula_size_type index = formula.size() - 1;
    std::sort(formula[index].begin(), formula[index].end());
    if (formula[index].empty()) {
      formula.pop_back();
      return;
    }
    const Options &options = specificationFile.getOptions();
    for (const Literal &l : formula[index]) {
      option_clasue[options.option(l.variable())].push_back(index);
    }
  }

#ifndef NVISIBLE
  void print(const std::vector<std::vector<unsigned>> &array) const {
    std::cout << "***********valid_print()**************" << std::endl;
    for (size_t row_index = 0; row_index < array.size(); ++row_index) {
      for (auto v : array[row_index]) {
        std::cout << v << ' ';
      }
      std::cout << "sat_count: ";
      for (auto count : rows_sat_count[row_index]) {
        std::cout << count << ' ';
      }
      std::cout << std::endl;
    }
    for (size_t option = 0; option < option_clasue.size(); ++option) {
      std::cout << option << ": ";
      for (auto clause_index : option_clasue[option]) {
        std::cout << clause_index << ' ';
      }
      std::cout << std::endl;
    }
    formula.print();
    std::cout << "***********end of valid_print******************" << std::endl;
  }
#endif
};
} // namespace Valid
#endif /* end of include guard: VALID_CHECK_H_ONAP2ASG */
