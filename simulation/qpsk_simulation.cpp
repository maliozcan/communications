#include <iostream>
#include <random>
#include <vector>
#include <complex>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <fstream>
#include <cmath>

#include "psk.hpp"
#include "utilities.hpp"
#include "gplot.h"

std::vector<double> simulate(const std::vector<double>& snr_list, const std::size_t num_of_bits) {
    std::vector<double> ber{};
    ber.reserve(snr_list.size());
    for (const auto snr : snr_list) {
        // Generate random bits
        comm::bit_seq_t bits = comm::generate_uniformly_distributed_bits(num_of_bits);
        assert(static_cast<std::size_t>(std::count(std::cbegin(bits), std::cend(bits), 0) + std::count(std::cbegin(bits), std::cend(bits), 1)) == num_of_bits);

        // BPSK modulation
        comm::complex_signal_seq_t symbols = comm::qpsk_modulation(bits);
        assert(symbols.size() == num_of_bits / 2); // 1 QPSK symbol carries 2 bits

        // Add AWGN noise
        auto noise = comm::generate_awgn_noise(symbols.size(), snr);

        // Receive noisy symbols
        const auto received = comm::add(symbols, noise);
        #if 0 // Using add_in_place does not matter for an optimized program.
        comm::add_in_place(std::cbegin(noise), std::cend(noise), std::begin(symbols));
        const auto& received = symbols;
        #endif

        // Demodulate symbols
        comm::bit_seq_t demodulated_bits = comm::qpsk_demodulation(received);
        assert(demodulated_bits.size() == num_of_bits);

        const auto error_num = comm::count_error(bits, demodulated_bits);
        ber.push_back(static_cast<double>(error_num) / static_cast<double>(num_of_bits));
    }
    return ber;
}

// This is same for bpsk since each dimension is independent from the
// other and the bits that will be transmitted are uniformly distributed.
// BER^{QPSK} = (1/2) * BER^{BPSK} + (1/2) * BER^{BPSK} = BER^{BPSK}
std::vector<double> qpsk_theory(const std::vector<double>& snr_list) {
    std::vector<double> theory{};
    std::transform(std::cbegin(snr_list), std::cend(snr_list), std::back_inserter(theory), [](const double snr_db) {
        return 0.5 * std::erfc(std::sqrt(std::pow(10, snr_db / 10)));
    });
    return theory;
}

int main() {
    constexpr std::size_t num_of_bits = 1'000'000;
    std::vector<double> eb_no(11); // energy per bit to noise power spectral density ratio
    std::iota(std::begin(eb_no), std::end(eb_no), 0);
    eb_no.push_back(10.6);
    assert(std::is_sorted(std::cbegin(eb_no), std::cend(eb_no)));

    // Convert EbNo to EsNo for symbol. 2 bits form a QPSK symbol.
    // This will be used for AWGN noise generation.
    // EsNo: energy per symbol to noise power spectral density ratio
    // Resource: https://en.wikipedia.org/wiki/Eb/N0
    const auto symbol_snr = comm::convert_eb_no_to_es_no(eb_no, 2);

    const auto ber = simulate(symbol_snr, num_of_bits);
    // comm::print_container(std::cbegin(ber), std::cend(ber));

    const auto sim = comm::concatenate(std::cbegin(eb_no), std::cend(eb_no), std::cbegin(ber));
    // comm::print_container(std::cbegin(sim), std::cend(sim));

    const auto theory_of_qpsk = qpsk_theory(eb_no);
    const auto theory = comm::concatenate(std::cbegin(eb_no), std::cend(eb_no), std::cbegin(theory_of_qpsk));
    // comm::print_container(std::cbegin(theory), std::cend(theory));

    gplot gp{gplot::type::semilogy};
    gp.add_2D_data("QPSK sim. with AWGN Channel", sim);
    gp.add_2D_data("QPSK theory. with AWGN Channel", theory);
    gp.plot();
}

