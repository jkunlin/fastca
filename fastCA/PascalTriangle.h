#ifndef PASCALTRIANGLE_H
#define PASCALTRIANGLE_H

#include <vector>

class PascalTriangle {
public:
  PascalTriangle();
  void rowsGrowTo(const unsigned rows);
  unsigned nCr(const unsigned n, const unsigned r) {
    if (n >= table.size()) {
      rowsGrowTo(n);
    }
    if (r > n) {
      return 0;
    }
    return table[n][r];
  }

private:
  std::vector<std::vector<unsigned>> table;
};

extern PascalTriangle pascalTriangle;
#endif /* end of include guard: PASCALTRIANGLE_H */
