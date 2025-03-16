#ifndef INCLUDE_DEFINITIONS_H
#define INCLUDE_DEFINITIONS_H

#include <complex>
#include <cstdint>
#include <vector>
#ifdef FFTW_COMPLEX_TYPE
#include <fftw3.h>
#endif

enum class result {
    success,
    failure
};

namespace comm {

    using bit_t = uint8_t;
    using bit_seq_t = std::vector<bit_t>;

#ifndef FFTW_COMPLEX_TYPE
    using complex_signal_t = std::complex<double>;
    using complex_signal_seq_t = std::vector<complex_signal_t>;

#else
    using complex_signal_t = fftw_complex;

    complex_signal_t operator+(const complex_signal_t& val1, const complex_signal_t& val1) {
        return complex_signal_t{val1[0] + val2[0], val1[1] + val2[1]};
    }

#endif


};

#endif // INCLUDE_DEFINITIONS_H
