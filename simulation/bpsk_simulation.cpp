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

        constexpr double pi = 3.14159265359;
        // BPSK modulation
        comm::complex_signal_seq_t symbols = comm::bpsk_modulation(bits, pi);
        assert(symbols.size() == num_of_bits); // 1 BPSK symbol carries 1 bit

        // Add AWGN noise
        const auto noise = comm::generate_awgn_noise(symbols.size(), snr);

        // Receive noisy symbols
        const auto received = comm::add(symbols, noise);
        #if 0 // Using add_in_place does not matter for an optimized program.
        comm::add_in_place(std::cbegin(noise), std::cend(noise), std::begin(symbols));
        const auto& received = symbols;
        #endif

        // Demodulate symbols
        comm::bit_seq_t demodulated_bits = comm::bpsk_demodulation(received, pi);
        assert(demodulated_bits.size() == num_of_bits);

        const auto error_num = comm::count_error(bits, demodulated_bits);
        ber.push_back(static_cast<double>(error_num) / static_cast<double>(num_of_bits));
    }
    return ber;
}

std::vector<double> bpsk_theory(const std::vector<double>& snr_list) {
    std::vector<double> theory{};
    std::transform(std::cbegin(snr_list), std::cend(snr_list), std::back_inserter(theory), [](const double snr_db) {
        return 0.5 * std::erfc(std::sqrt(std::pow(10, snr_db / 10)));
    });
    return theory;
}

int main() {
    constexpr std::size_t num_of_bits = 1'000'000;
    std::vector<double> snr{};
    snr.resize(11);
    std::iota(std::begin(snr), std::end(snr), 0);
    snr.push_back(10.6);
    const auto ber = simulate(snr, num_of_bits);
    std::cout << "BER result\n";
    comm::print_container(std::cbegin(ber), std::cend(ber));

    const auto sim = comm::concatenate(std::cbegin(snr), std::cend(snr), std::cbegin(ber));
    std::cout << "Simulation result\n";
    comm::print_container(std::cbegin(sim), std::cend(sim));

    const auto theory_of_bpsk = bpsk_theory(snr);
    const auto theory = comm::concatenate(std::cbegin(snr), std::cend(snr), std::cbegin(theory_of_bpsk));
    std::cout << "BPSK Theory\n";
    comm::print_container(std::cbegin(theory), std::cend(theory));

    gplot gp{gplot::type::semilogy};
    gp.add_2D_data("BPSK sim. with AWGN Channel", sim);
    gp.add_2D_data("BPSK theory. with AWGN Channel", theory);
    gp.plot();
}
