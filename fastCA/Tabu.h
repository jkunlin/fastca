#include <cassert>
#include <vector>

// need to be initialized
template <typename T> class Tabu {
public:
  Tabu(unsigned size) : tabuList(size), oldestIndex(0) {}
  void initialize(const T &element) {
    for (auto &ele : tabuList) {
      ele = element;
    }
  }
  bool isTabu(const T &element) {
    for (auto ele : tabuList) {
      if (ele == element) {
        return true;
      }
    }
    return false;
  }
  typename std::vector<T>::iterator begin() { return tabuList.begin(); }
  typename std::vector<T>::iterator const end() { return tabuList.end(); }
  void insert(const T &element) {
    tabuList[oldestIndex] = element;
    oldestIndex = (oldestIndex + 1) % tabuList.size();
  }

private:
  std::vector<T> tabuList;
  unsigned oldestIndex;
};

class TupleTabu {
public:
  TupleTabu(unsigned long long t) : tenure(t) {}
  void initialize(std::vector<unsigned long long>::size_type size) {
    std::vector<unsigned long long>(size, 0).swap(elementLastChange);
  }
  bool isTabu(std::vector<unsigned long long>::size_type index,
              unsigned long long step) {
    assert(step >= elementLastChange[index]);
    if (elementLastChange[index] == 0) {
      return false;
    }
    return step - elementLastChange[index] <= tenure;
  }
  void setLastChange(std::vector<unsigned long long>::size_type index,
                     unsigned long long step) {
    elementLastChange[index] = step;
  }
  std::vector<unsigned long long> &getElementLastChange() {
    return elementLastChange;
  }

private:
  std::vector<unsigned long long> elementLastChange;
  const unsigned long long tenure;
};

// not need to initialize
class TupleTabu2 {
public:
  TupleTabu2(unsigned tenure) : tupleBucket(tenure), oldestIndex(0) {}
  void initialize() {
    unsigned tenure = tupleBucket.size();
    tupleBucket.clear();
    tupleBucket.resize(tenure);
    oldestIndex = 0;
  }
  void pushingTuples() {
    oldestIndex = (oldestIndex + 1) % tupleBucket.size();
    tupleBucket[oldestIndex].clear();
  }
  void insert(unsigned encode) { tupleBucket[oldestIndex].push_back(encode); }
  void getAllTabuTuples(std::vector<unsigned> &tuples) {
    tuples.clear();
    for (auto &bucket : tupleBucket) {
      for (auto encode : bucket) {
        tuples.push_back(encode);
      }
    }
  }

private:
  std::vector<std::vector<unsigned>> tupleBucket;
  unsigned oldestIndex;
};

class CellTabu {
public:
  CellTabu(unsigned long long t) : tenure(t) {}
  void initialize(unsigned r_size, unsigned c_size) {
    std::vector<std::vector<unsigned long long>>(r_size).swap(
        elementLastChange);
    for (unsigned i = 0; i < r_size; ++i) {
      std::vector<unsigned long long>(c_size, 0).swap(elementLastChange[i]);
    }
  }
  std::vector<std::vector<unsigned long long>> &getElementLastChange() {
    return elementLastChange;
  }
  bool isTabu(unsigned r, unsigned c, unsigned long long step) {
    assert(step >= elementLastChange[r][c]);
    if (elementLastChange[r][c] == 0) {
      return false;
    }
    return step - elementLastChange[r][c] <= tenure;
  }
  void setLastChange(unsigned r, unsigned c, unsigned long long step) {
    elementLastChange[r][c] = step;
  }

private:
  std::vector<std::vector<unsigned long long>> elementLastChange;
  const unsigned long long tenure;
};

class Entry {
public:
  Entry() {}
  Entry(const unsigned r, const unsigned c) {
    row = r;
    col = c;
  }
  Entry(const Entry &copy) {
    row = copy.row;
    col = copy.col;
  }
  bool operator==(const Entry &e) { return (row == e.row) && (col == e.col); }
  void operator=(const Entry &copy) {
    row = copy.row;
    col = copy.col;
  }
  unsigned getRow() const { return row; }
  void setRow(unsigned r) { row = r; }

private:
  unsigned row, col;
};
