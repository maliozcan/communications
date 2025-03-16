#ifndef INCLUDE_PSK_HPP
#define INCLUDE_PSK_HPP

#include <algorithm>
#include <array>
#include <complex>
#include <cassert>
#include <vector>

#include "definitions.h"


namespace comm {

/**
 * @brief Modulate bits in a input container to a output container.
 *
 * Makes 0 to -1, and 1 to 1.
 *
 * @tparam InputIterator
 * @tparam OutputIterator
 * @param input_begin
 * @param input_end
 * @param output_begin
 */

// https://en.cppreference.com/w/cpp/types/void_t -> is_iterable, SFINAE, enable_if
// https://blog.tartanllama.xyz/detection-idiom/
// https://en.cppreference.com/w/cpp/types/enable_if
// https://www.boost.org/doc/libs/1_80_0/libs/type_traits/doc/html/boost_typetraits/reference/detected.html


template<typename InputIterator, typename OutputIterator>
void bpsk_modulation(const InputIterator input_begin, const InputIterator input_end, OutputIterator output_begin, double offset = 0) {
    using input_value_type = typename InputIterator::value_type;
    using output_value_type = typename OutputIterator::value_type;
    const auto c = std::cos(offset);
    const auto s = std::sin(offset);
    std::transform(input_begin, input_end, output_begin, [c, s](const input_value_type bit) {
        // (cos(pi + offset) + i*sin(pi + offset) : (cos(0 + offset) + i*sin(0 + offset)
        return (bit == 0) ? output_value_type(-c, -s) : output_value_type(c, s);
    });
}

inline
complex_signal_seq_t bpsk_modulation(const bit_seq_t& bit_seq, double offset = 0) {
    complex_signal_seq_t symbols(bit_seq.size());
    bpsk_modulation(std::cbegin(bit_seq), std::cend(bit_seq), std::begin(symbols), offset);
    return symbols;
}

/**
 * @brief
 *
 * @tparam InputIterator
 * @tparam OutputIterator
 * @param input_begin the beginning of received symbols
 * @param input_end the end of received symbols
 * @param output_begin bit sequence of demodulated symbols
 */
template<typename InputIterator, typename OutputIterator>
void bpsk_demodulation(const InputIterator input_begin, const InputIterator input_end, OutputIterator output_begin, double offset = 0) {
    using input_value_type = typename InputIterator::value_type;
    using output_value_type = typename OutputIterator::value_type;
    const auto c = std::cos(offset);
    const auto s = std::sin(offset);
    // For zero offset, If the received symbol is above the curve of y=-x (y+x=0), it is demodulated as 0, otherwise 1.
    std::transform(input_begin, input_end, output_begin, [c, s](const input_value_type symbol) {
        return (symbol.imag() * s + symbol.real() * c < 0) ? static_cast<output_value_type>(0) : static_cast<output_value_type>(1);
    });
}

inline
bit_seq_t bpsk_demodulation(const complex_signal_seq_t& symbols, double offset = 0) {
    bit_seq_t bit_seq(symbols.size());
    bpsk_demodulation(std::cbegin(symbols), std::cend(symbols), std::begin(bit_seq), offset);
    return bit_seq;
}

/*
    00 -> ( 1,  1)
    01 -> ( 1, -1)
    10 -> (-1,  1)
    11 -> (-1, -1)
*/
template<typename InputIterator, typename OutputIterator>
void qpsk_modulation(InputIterator input_begin, InputIterator input_end, OutputIterator output_begin) {
    using input_value_type = typename InputIterator::value_type;
    assert(std::distance(input_begin, input_end) % 2 == 0);
    for (; input_begin != input_end; ++input_begin, ++output_begin) {
        const input_value_type bit1 = *input_begin;
        const input_value_type bit2 = *(++input_begin);
        const double val1 = 1 - 2 * bit1;
        const double val2 = 1 - 2 * bit2;
        *output_begin = 1/std::sqrt(2) * std::complex<double>{val1, val2};
        // std::printf("%u%u -> (%2.0f, %2.0f)\n", bit1, bit2, val1, val2);
    }
}

inline
complex_signal_seq_t qpsk_modulation(const bit_seq_t& bit_seq) {
    assert(bit_seq.size() % 2 == 0);
    complex_signal_seq_t symbols(bit_seq.size() / 2);
    qpsk_modulation(std::cbegin(bit_seq), std::cend(bit_seq), std::begin(symbols));
    return symbols;
}


// Return demodulated bit sequence
template<typename InputIterator, typename OutputIterator>
void qpsk_demodulation(InputIterator input_begin, InputIterator input_end, OutputIterator output_begin, OutputIterator output_end) {
    using output_value_type = typename OutputIterator::value_type;
    assert(std::distance(input_begin, input_end) * 2 == std::distance(output_begin, output_end));
    static std::array<std::array<std::pair<output_value_type, output_value_type>, 2>, 2> table = []() {
        std::array<std::array<std::pair<output_value_type, output_value_type>, 2>, 2> t{};
        t[0][0] = {1, 1}; // (-1, -1) -> 11
        t[0][1] = {1, 0}; // (-1,  1) -> 10
        t[1][0] = {0, 1}; // ( 1, -1) -> 01
        t[1][1] = {0, 0}; // ( 1,  1) -> 00
        return t;
    }();
    for (; input_begin != input_end; ++input_begin, ++output_begin) {
        const auto bits = table[static_cast<uint8_t>(input_begin->real() > 0)][static_cast<uint8_t>(input_begin->imag() > 0)];
        *output_begin = bits.first;
        *(++output_begin) = bits.second;
    }
}

inline
bit_seq_t qpsk_demodulation(const complex_signal_seq_t& symbols) {
    bit_seq_t bit_seq(symbols.size() * 2);
    qpsk_demodulation(std::cbegin(symbols), std::cend(symbols), std::begin(bit_seq), std::end(bit_seq));
    return bit_seq;
}

}


#endif // INCLUDE_PSK_HPP