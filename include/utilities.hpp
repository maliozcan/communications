#ifndef INCLUDE_UTILITIES_HPP
#define INCLUDE_UTILITIES_HPP

#include <cstdint>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <iterator>

#include "definitions.h"

namespace comm {
namespace detail {
    inline
    auto& get_generator() {
        static std::random_device rd;
        static std::mt19937 generator(rd());
        return generator;
    }

    inline
    auto uniform_distribution_genarator() {
        static std::uniform_int_distribution<uint8_t> distribution(0, 1);
        return distribution(get_generator());
    }

    template<int32_t Mean>
    auto gaussian_distribution_genarator() {
        static std::normal_distribution<double> distribution(Mean, 1);
        return distribution(get_generator());
    }

    template<typename T, typename U>
    std::ostream& print(std::ostream& os, const std::pair<T, U>& p) {
        std::array<char, 64> buf{};
        (void) std::snprintf(buf.data(), buf.size(), "[%f, %f]", p.first, p.second);
        os << buf.data();
        return os;
    }

    template<>
    inline std::ostream& print(std::ostream& os, const std::pair<double, double>& p) {
        std::array<char, 64> buf{};
        (void) std::snprintf(buf.data(), buf.size(), "[%2.2f, %2.4e]", p.first, p.second);
        os << buf.data();
        return os;
    }
}

template<typename InputIterator>
void generate_uniformly_distributed_bits(InputIterator begin, InputIterator end) {
    std::generate(begin, end, []() {
        return detail::uniform_distribution_genarator();
    });
}

inline bit_seq_t generate_uniformly_distributed_bits(const std::size_t num_of_bits) {
    bit_seq_t bits(num_of_bits);
    generate_uniformly_distributed_bits(std::begin(bits), std::end(bits));
    return bits;
}

template<typename T>
double convert_eb_no_to_es_no(T ebno_db, const uint8_t modulation_order) {
    const double snr = std::pow(10, ebno_db/10.0) * modulation_order;
    return 10.0 * std::log10(snr);
}

// simülasyonu x = T * symbol, noise = birim güç şeklinde de yapabilirsin.
template<typename T>
std::vector<double> convert_eb_no_to_es_no(const std::vector<T> eb_no, const uint8_t modulation_order) {
    std::vector<double> symbol_snr(eb_no.size());
    std::transform(eb_no.cbegin(), eb_no.cend(), symbol_snr.begin(), [modulation_order](const T& ebno_db) {
        return convert_eb_no_to_es_no(ebno_db, modulation_order);
    });
    return symbol_snr;
}

template<typename InputIterator, typename U>
void generate_awgn_noise(InputIterator begin, InputIterator end, const U& snr_db) {
    using input_value_type = typename InputIterator::value_type;
    if constexpr(std::is_same_v<input_value_type, complex_signal_t>) {
        const auto snr = std::pow(10, -snr_db/20.0);
        const auto multipler = snr / std::sqrt(2);
        std::generate(begin, end, [multipler]() {
            return multipler * complex_signal_t(detail::gaussian_distribution_genarator<0>(), detail::gaussian_distribution_genarator<0>());
        });
    } else {
        // multiplier?
        std::generate(begin, end, []() {
            return detail::gaussian_distribution_genarator<0>();
        });
    }
}

template<typename U>
complex_signal_seq_t generate_awgn_noise(const std::size_t num_of_samples, const U& snr_db) {
    complex_signal_seq_t noise(num_of_samples);
    generate_awgn_noise(std::begin(noise), std::end(noise), snr_db);
    return noise;
}

template<typename InputIterator, typename OutputIterator>
void add(InputIterator first_begin, InputIterator first_end,
         InputIterator second_begin, OutputIterator result_begin) {
    for (; first_begin != first_end; ++first_begin, ++second_begin, ++result_begin) {
        *result_begin = *first_begin + *second_begin;
    }
}

template<typename T>
T add(const T& first, const T& second) {
    assert(first.size() == second.size());
    T result(first.size());
    add(std::cbegin(first), std::cend(first), std::cbegin(second), std::begin(result));
    return result;
}

template<typename InputIterator, typename OutputIterator>
void add_in_place(InputIterator first_begin, InputIterator first_end, OutputIterator result_begin) {
    for (; first_begin != first_end; ++first_begin, ++result_begin) {
        *result_begin += *first_begin;
    }
}

template<typename Iterator>
std::size_t count_error(Iterator first_begin, Iterator first_end, Iterator second_begin) {
    std::size_t error_num{0};
    for (; first_begin != first_end; ++first_begin, ++second_begin) {
        error_num += static_cast<std::size_t>((*first_begin != *second_begin));
    }
    return error_num;
}

template<typename T>
std::size_t count_error(const T& seq1, const T& seq2) {
    return count_error(std::cbegin(seq1), std::cend(seq1), std::cbegin(seq2));
}

template<typename InputIterator>
void print_container(InputIterator begin, InputIterator end) {
    using value_type = typename InputIterator::value_type;
    std::copy(begin, end, std::ostream_iterator<value_type>(std::cout, "\n"));
}

template<>
inline void print_container(std::vector<std::pair<double, double>>::const_iterator begin,
                     std::vector<std::pair<double, double>>::const_iterator end) {
    for (; begin != end; ++begin) {
        detail::print(std::cout, *begin);
        std::cout << '\n';
    }
}

template<typename InputIterator, typename Delimiter = const char*>
void print_container(InputIterator begin, const std::size_t n, const Delimiter& delim = "\n") {
    using value_type = typename InputIterator::value_type;
    std::copy_n(begin, n, std::ostream_iterator<value_type>(std::cout, delim));
}

template<typename InputIterator1, typename InputIterator2>
auto concatenate(InputIterator1 first_begin, InputIterator1 first_end, InputIterator2 second_begin) {
    using value_type1 = typename InputIterator1::value_type;
    using value_type2 = typename InputIterator2::value_type;
    std::vector<std::pair<value_type1, value_type2>> list(std::distance(first_begin, first_end));
    std::transform(first_begin, first_end, second_begin, std::begin(list), [](const value_type1& val1, const value_type2& val2) {
        return std::make_pair(val1, val2);
    });
    return list;
}

}

#endif // INCLUDE_UTILITIES_HPP
