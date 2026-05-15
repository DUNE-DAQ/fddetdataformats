#ifndef FDDETDATAFORMATS_UNITTEST_UNITTESTUTILS_HPP_
#define FDDETDATAFORMATS_UNITTEST_UNITTESTUTILS_HPP_

#include <random>
#include <vector>

namespace {

  template <typename T>
  std::vector<std::vector<T>> generate_matrix(const int nrows, const int ncols, const T maxval) {
  
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<T> dist(0, maxval);

    std::vector<std::vector<T>> vals(nrows, std::vector<T>(ncols));

    for (auto i = 0; i < nrows; ++i) {
      for (auto j = 0; j < ncols; ++j) {
	vals[i][j] = dist(rng);
      }
    }

    return vals;
  }

} // namespace ""

#endif // FDDETDATAFORMATS_UNITTEST_UNITTESTUTILS_HPP_
