#include <immintrin.h>
#include <iostream>

using namespace std;

inline double hsum_double_avx(__m256d v) {
  __m128d vlow = _mm256_castpd256_pd128(v);
  __m128d vhigh = _mm256_extractf128_pd(v, 1); // high 128
  vlow = _mm_add_pd(vlow, vhigh);              // reduce down to 128

  __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
  return _mm_cvtsd_f64(_mm_add_sd(vlow, high64)); // reduce to scalar
}

//                   %rsi           %rdi
double sumSq(const double *x, uint64_t len) {
  __m256d mean = _mm256_setzero_pd();
  __m256d sum = _mm256_setzero_pd();
  for (; len > 0; len -= 4, x += 4) {
    __m256d x = _mm256_load_pd(x);        // load 4 values from memory
    __m256d del = _mm256_sub_pd(x, mean); // compute x[i] - mean 4times
    //__m256d delsq = _mm256_mul_pd(del, del); // square the results
    //    sum = _mm256_add_pd(sum, delsq);

    sum = _mm256_fmadd_pd(del, del, sum);
  }
  return hsum_double_avx(sum);
}

int main() {
  double x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11.5, 12};
  cout << sumSq(x, sizeof(x) / sizeof(double)) << '\n';
}
