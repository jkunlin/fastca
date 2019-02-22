#include "PascalTriangle.h"

PascalTriangle::PascalTriangle() {
  table.push_back(std::vector<unsigned>());
  table[0].push_back(1);
}

void PascalTriangle::rowsGrowTo(const unsigned rows) {
  while (table.size() <= rows) {
    table.push_back(std::vector<unsigned>());
    std::vector<unsigned> &line = table[table.size() - 1];
    std::vector<unsigned> &source = table[table.size() - 2];
    unsigned depth = table.size() - 1;
    line.resize(depth + 1);
    line[0] = 1;
    for (unsigned column = 1, trail = source[0]; column < depth; ++column) {
      line[column] = trail;
      line[column] += trail = source[column];
    }
    line[depth] = 1;
  }
}

// unsigned PascalTriangle::nCr(const unsigned n, const unsigned r) {
//	if (n >= table.size()) {
//		rowsGrowTo(n);
//	}
//	if (r > n) {
//		return 0;
//	}
//	return table[n][r];
//}

// unsigned PascalTriangle::nCr(const unsigned n, const unsigned r) {
//	unsigned res = 1;
//	for (unsigned i = 0; i < r; ++i) {
//		res *= n - i;
//	}
//	for (unsigned i = 2; i <= r; ++i) {
//		res /= i;
//	}
//	return res;
//}

PascalTriangle pascalTriangle;
