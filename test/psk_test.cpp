#include "doctest.h"

#include <vector>
#include <iostream>
#include  <algorithm>

#include "psk.hpp"
#include "utilities.hpp"


TEST_CASE("BPSK") {
    constexpr double pi = 3.14159265359;
    std::vector<double> offset_list{};
    std::generate_n(std::back_inserter(offset_list), 20, []() {
        static int32_t i = -10;
        if (i == 0) {
            ++i;
            return 0.0;
        }
        return pi / i++;
    });

    std::vector<uint8_t> bits{0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1};

    for (const auto offset : offset_list) {
        std::vector<std::complex<double>> symbols(bits.size()); // 1 BPSK symbol carries 1 bit
        comm::bpsk_modulation(std::cbegin(bits), std::cend(bits), std::begin(symbols), offset);

        // Demodulate symbols which had not been affected by noise
        std::vector<uint8_t> demodulated_bits(bits.size());
        comm::bpsk_demodulation(std::cbegin(symbols), std::cend(symbols), std::begin(demodulated_bits), offset);

        CHECK(demodulated_bits == bits);
    }
}

TEST_CASE("QPSK") {
    std::vector<uint8_t> bits{0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1};
    assert(bits.size() % 2 == 0);

    std::vector<std::complex<double>> symbols(bits.size() / 2); // 1 QPSK symbol carries 2 bits
    comm::qpsk_modulation(std::cbegin(bits), std::cend(bits), std::begin(symbols));

    // Demodulate symbols which have no noise
    std::vector<uint8_t> demodulated_bits(bits.size());
    comm::qpsk_demodulation(std::cbegin(symbols), std::cend(symbols), std::begin(demodulated_bits), std::end(demodulated_bits));

    CHECK(demodulated_bits == bits);
}
