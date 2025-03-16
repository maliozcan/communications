#include <complex.h>

#if !defined(FFTW_NO_Complex) && defined(_Complex_I) && defined(complex) && defined(I)
#error ""
#endif

/*
    Q: C++ is complex template compatible with c99 complex?
    A: Yes. https://en.cppreference.com/w/cpp/numeric/complex
            https://stackoverflow.com/questions/44180119/c-c-interop-between-complex-and-stdcomplex

    If we include <complex.h> in C++, this does not make FFTW provide the C99 compatible fftw_complex.

*/



#include <algorithm>
#include <random>
#include <fftw3.h>
#include "gplot.h"
#include "utilities.hpp"


int main () {
    fftw_plan plan{};
    int N = 4096;

    auto* in = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * N));
        
    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> distribution(0, 1);
    for (int i = 0; i < N; ++i) {
        in[i][0] = std::sin(i);
        in[i][1] = std::cos(i);
    }

    auto* out = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * N));
    plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    fftw_execute(plan); /* repeat as needed */
    
    std::vector<double> out_d(N);
    for (int i = 0; i < N; ++i) {
        out_d[i] = out[i][1];
    }
    std::copy(std::cbegin(out_d), std::cend(out_d), std::ostream_iterator<double>(std::cout, "\n"));

    std::vector<double> index(N);
    std::iota(std::begin(index), std::end(index), 0);
    const auto pair = comm::concatenate(std::begin(index), std::end(index), std::cbegin(out_d));

    fftw_destroy_plan(plan);
    fftw_free(in); fftw_free(out);

    gplot gp;
    gp.add_2D_data("FFT", pair);
    gp.plot();

    return 0;
}
