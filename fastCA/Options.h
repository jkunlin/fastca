#ifndef OPTIONS_H
#define OPTIONS_H

#include <iostream>
#include <vector>
class Options {
public:
  Options() {}
  void initialize(const std::vector<unsigned> &values);
  unsigned size() const { return cumulativeValueCounts.size(); }
  unsigned option(const unsigned symbol) const { return owingOptions[symbol]; };
  unsigned firstSymbol(const unsigned option) const {
    return option ? cumulativeValueCounts[option - 1] : 0;
  }
  unsigned lastSymbol(const unsigned option) const {
    return cumulativeValueCounts[option] - 1;
  }
  unsigned symbolCount(const unsigned option) const {
    return option
               ? cumulativeValueCounts[option] -
                     cumulativeValueCounts[option - 1]
               : cumulativeValueCounts[option];
  }
  unsigned allSymbolCount() const { return *cumulativeValueCounts.rbegin(); }
#ifndef NDEBUG
  void print();
#endif
private:
  std::vector<unsigned> cumulativeValueCounts;
  std::vector<unsigned> owingOptions;
};

#endif /* end of include guard: OPTIONS_H */
