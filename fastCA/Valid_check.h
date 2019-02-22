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

class Clause {
public:
  Clause() {}
  Clause(Clause &&c) : clause(std::move(c.clause)) {}
  Clause(const Clause &c) : clause(c.clause) {}
  void addLiteral(const Literal &l) { clause.push_back(l); }
  const Literal &operator[](std::vector<Literal>::size_type i) const {
    return clause[i];
  }
  Literal &operator[](std::vector<Literal>::size_type i) { return clause[i]; }
  void clear() { clause.clear(); }
  void resize(std::vector<Literal>::size_type size) { clause.resize(size); }
  std::vector<Literal>::size_type size() { return clause.size(); }
  std::vector<Literal>::size_type size() const { return clause.size(); }
  bool empty() const { return clause.empty(); }
  bool empty() { return clause.empty(); }
  std::vector<Literal>::const_iterator begin() const { return clause.begin(); }
  std::vector<Literal>::iterator begin() { return clause.begin(); }
  std::vector<Literal>::const_iterator end() const { return clause.end(); }
  std::vector<Literal>::iterator end() { return clause.end(); }
#ifndef NVISIBLE
  void print() const {
    for (const Literal &l : clause) {
      l.print();
      std::cout << ",";
    }
    std::cout << '\b' << ' ';
  }
#endif
private:
  std::vector<Literal> clause;
};

class Formula {
public:
  void addClause(Clause &&c) {
    if (c.empty()) {
      return;
    }
    clauses.push_back(std::move(c));
  }
  void addClause(const Clause &c) {
    if (c.empty()) {
      return;
    }
    clauses.push_back(c);
  }
  std::vector<Clause>::size_type size() { return clauses.size(); }
  const Clause &operator[](std::vector<Clause>::size_type index) const {
    return clauses[index];
  }
  Clause &operator[](std::vector<Clause>::size_type index) {
    return clauses[index];
  }
  void pop_back() { clauses.pop_back(); }
  std::vector<Clause>::const_iterator begin() const { return clauses.begin(); }
  std::vector<Clause>::const_iterator end() const { return clauses.end(); }

#ifndef NVISIBLE
  void print() const {
    for (const Clause &c : clauses) {
      c.print();
      std::cout << '\t';
    }
    std::cout << '\b' << ' ' << std::endl;
  }
#endif
private:
  std::vector<Clause> clauses;
};

class Validater {
private:
  using Formula_size_type = std::vector<Clause>::size_type;
  using Clauses_sat_count = std::vector<unsigned>;
  const SpecificationFile &specificationFile;
  std::vector<std::vector<Formula_size_type>> option_clasue;
  Formula formula;
  std::vector<Clauses_sat_count> rows_sat_count;

public:
  Validater(const SpecificationFile &specificationFile)
      : specificationFile(specificationFile) {
    option_clasue.resize(specificationFile.getOptions().size());
  }

  bool valida_row(std::vector<unsigned> &row,
                  std::vector<unsigned> &changedVars) {
    const Options &options = specificationFile.getOptions();
    for (size_t i = 0; i < changedVars.size(); ++i) {
      unsigned opt = options.option(changedVars[i]);
      std::swap(row[opt], changedVars[i]);
    }
    bool pass = true;
    for (auto cl : formula) {
      bool cl_failed = true;
      for (auto lt : cl) {
        unsigned v = lt.variable();
        unsigned opt = options.option(v);
        if (lt.is_negative()) {
          if (v != row[opt]) {
            cl_failed = false;
            break;
          }
        } else {
          if (v == row[opt]) {
            cl_failed = false;
            break;
          }
        }
      }
      if (cl_failed) {
        pass = false;
        break;
      }
    }
    for (size_t i = 0; i < changedVars.size(); ++i) {
      unsigned opt = options.option(changedVars[i]);
      std::swap(row[opt], changedVars[i]);
    }
    return pass;
  }

  void addClause(Clause &&c) {
    compact_clause(c);
    if (c.empty()) {
      return;
    }
    const Options &options = specificationFile.getOptions();
    Formula_size_type index = formula.size();
    for (const Literal &l : c) {
      option_clasue[options.option(l.variable())].push_back(index);
    }
    formula.addClause(std::move(c));
  }

  void addClause(const Clause &c) {
    formula.addClause(c);
    Formula_size_type index = formula.size() - 1;
    compact_clause(formula[index]);
    if (formula[index].empty()) {
      formula.pop_back();
      return;
    }
    const Options &options = specificationFile.getOptions();
    for (const Literal &l : formula[index]) {
      option_clasue[options.option(l.variable())].push_back(index);
    }
  }

  void compact_clause(Clause &c) {
    const Options &options = specificationFile.getOptions();
    std::sort(c.begin(), c.end());
    std::vector<Literal>::size_type i = 0, j = 1;
    while (j < c.size()) {
      auto i_var = c[i].variable(), j_var = c[j].variable();
      auto i_option = options.option(i_var), j_option = options.option(j_var);
      if (i_option != j_option) {
        c[++i] = c[j++];
        continue;
      }
      if (c[i].is_negative() && c[j].is_negative()) {
        if (i_var == j_var) {
          ++j;
        } else {
          c.clear();
          return;
        }
      } else if (!c[i].is_negative() && !c[j].is_negative()) {
        if (i_var == j_var) {
          ++j;
        } else {
          c[++i] = c[j++];
        }
      } else if (!c[i].is_negative() && c[j].is_negative()) {
        if (i_var == j_var) {
          c.clear();
          return;
        } else {
          if (i == 0) {
            c[i] = c[j++];
          } else {
            --i;
          }
        }
      } else {
        if (i_var == j_var) {
          c.clear();
          return;
        } else {
          ++j;
        }
      }
    }
    c.resize(i + 1);
  }

  void initialize(const std::vector<std::vector<unsigned int>> &array) {
    rows_sat_count.resize(array.size());
    for (std::vector<std::vector<unsigned int>>::size_type row_index = 0;
         row_index < array.size(); ++row_index) {
      change_row(row_index, array[row_index]);
    }
#ifndef NVISIBLE
    print(array);
#endif
  }

  void change_row(size_t row_index, const std::vector<unsigned int> &row) {
    const Options &options = specificationFile.getOptions();
    auto &clauses_sat_count = rows_sat_count[row_index];
    clauses_sat_count.clear();
    clauses_sat_count.resize(formula.size(), 0);
    for (size_t option = 0; option < options.size(); ++option) {
      auto var = row[option];
      for (auto clause_index : option_clasue[option]) {
        for (auto &l : formula[clause_index]) {
          auto v = l.variable();
          auto opt = options.option(v);
          if (opt != option) {
            continue;
          }
          if (l.is_negative()) {
            if (v != var)
              clauses_sat_count[clause_index]++;
          } else if (v == var) {
            clauses_sat_count[clause_index]++;
          }
        }
      }
    }
  }

  bool valida_change(size_t row_index, unsigned int option,
                     unsigned int org_var, unsigned int new_var) {

    const Options &options = specificationFile.getOptions();
    Clauses_sat_count &clauses_sat_count = rows_sat_count[row_index];
    for (auto clause_index : option_clasue[option]) {
      if (clauses_sat_count[clause_index] > 1) {
        continue;
      }
      const Clause &c = formula[clause_index];
      for (size_t i = 0; i < c.size(); ++i) {
        auto &l = c[i];
        auto lvar = l.variable();
        if (options.option(lvar) != option) {
          continue;
        }
        if (l.is_negative()) {
          if (lvar != org_var && lvar == new_var) {
            return false;
          }
        } else {
          if (lvar == org_var && lvar != new_var) {
            for (++i; i < c.size(); ++i) {
              if (options.option(c[i].variable()) != option) {
                return false;
              }
              if (c[i].variable() == new_var) {
                return true;
              }
            }
            return false;
          }
        }
      }
    }
    return true;
  }

  void change_var(size_t row_index, unsigned int option, unsigned int org_var,
                  unsigned int new_var) {

    const Options &options = specificationFile.getOptions();
    Clauses_sat_count &clauses_sat_count = rows_sat_count[row_index];
    for (auto clause_index : option_clasue[option]) {
      const Clause &c = formula[clause_index];
      // TODO: if there is not duplicate literal, break when the var is meet
      for (auto &l : c) {
        auto lvar = l.variable();
        if (options.option(lvar) != option) {
          continue;
        }
        if (l.is_negative()) {
          if (lvar == org_var && lvar != new_var) {
            clauses_sat_count[clause_index]++;
          } else if (lvar != org_var && lvar == new_var) {
            clauses_sat_count[clause_index]--;
          }
        } else {
          if (lvar == org_var && lvar != new_var) {
            clauses_sat_count[clause_index]--;
          } else if (lvar != org_var && lvar == new_var) {
            clauses_sat_count[clause_index]++;
          }
        }
      }
    }
  }

  // Request: org_vars and new_vars are sorted
  void change_mutivar(size_t row_index, std::vector<unsigned int> org_vars,
                      std::vector<unsigned int> new_vars) {

    const Options &options = specificationFile.getOptions();
    for (std::vector<unsigned int>::size_type i = 0; i < org_vars.size(); ++i) {
      auto option = options.option(org_vars[i]);
      auto org_var = org_vars[i];
      auto new_var = new_vars[i];
      change_var(row_index, option, org_var, new_var);
    }
  }

  // Request: org_vars and new_vars are sorted
  // bool valida_multivar_change(size_t row_index,
  //                             std::vector<unsigned int> org_vars,
  //                             std::vector<unsigned int> new_vars) {
  //
  //   const Options &options = specificationFile.getOptions();
  //   std::vector<unsigned int>::size_type i = 0;
  //   for (; i < org_vars.size(); ++i) {
  //     auto option = options.option(org_vars[i]);
  //     auto org_var = org_vars[i];
  //     auto new_var = new_vars[i];
  //     if (valida_change(row_index, option, org_var, new_var)) {
  //       change_var(row_index, option, org_var, new_var);
  //     }
  //     // undo it
  //     else {
  //       while (i > 0) {
  //         --i;
  //         change_var(row_index, options.option(org_vars[i]), new_vars[i],
  //                    org_vars[i]);
  //       }
  //       return false;
  //     }
  //   }
  //   while (i > 0) {
  //     --i;
  //     change_var(row_index, options.option(org_vars[i]), new_vars[i],
  //                org_vars[i]);
  //   }
  //   return true;
  // }

  void pop_back_row() { rows_sat_count.pop_back(); }
  void
  exchange_row(std::vector<std::vector<unsigned int>>::size_type row_index1,
               std::vector<std::vector<unsigned int>>::size_type row_index2) {
    rows_sat_count[row_index1].swap(rows_sat_count[row_index2]);
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
}
#endif /* end of include guard: VALID_CHECK_H_ONAP2ASG */
