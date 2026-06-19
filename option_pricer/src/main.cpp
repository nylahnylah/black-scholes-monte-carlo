#include "Option.h"    // Option class
#include <iostream>    // std::cout
#include <fstream>     // std::ofstream
#include <chrono>      // timing
#include <vector>      // std::vector

int main()
{
    // --- Option parameters ---
    // A European call option on a stock currently priced at 100,
    // with strike 105, 5% risk-free rate, 20% volatility, 1 year to expiry.
    double S     = 100.0;
    double K     = 105.0;
    double r     = 0.05;
    double sigma = 0.20;
    double T     = 1.0;

    Option opt(S, K, r, sigma, T);

    // --- Black-Scholes (exact, closed-form) ---
    // std::chrono::high_resolution_clock::now() records the current time.
    // We record before and after, then take the difference.
    auto bsStart = std::chrono::high_resolution_clock::now();
    double bsPrice = opt.blackScholesPrice();
    auto bsEnd = std::chrono::high_resolution_clock::now();

    // Duration in microseconds as a double
    double bsTime = std::chrono::duration<double, std::micro>(bsEnd - bsStart).count();

    std::cout << "Black-Scholes price: " << bsPrice << "\n";
    std::cout << "Black-Scholes time:  " << bsTime << " microseconds\n\n";

    // --- Monte Carlo (simulation, approximate) ---
    // Run with increasing number of simulations to show convergence.
    // std::vector is a resizable array — like a Python list but typed.
    std::vector<int> simCounts = {100, 1000, 10000, 100000, 1000000};

    // Open the CSV file for writing.
    // std::ofstream opens a file — if it doesn't exist, it creates it.
    std::ofstream csv("../output/results.csv");

    // Write the header row
    csv << "numSims,mcPrice,bsPrice,error,timeMs\n";

    std::cout << "Monte Carlo convergence:\n";
    std::cout << "numSims    | mcPrice   | error     | time (ms)\n";
    std::cout << "------------------------------------------------\n";

    for (int n : simCounts) {
        // 'for (int n : simCounts)' is a range-based for loop —
        // it iterates over every element in simCounts, calling each one n.
        // Equivalent to Python's 'for n in sim_counts'.

        auto mcStart = std::chrono::high_resolution_clock::now();
        double mcPrice = opt.monteCarloPrice(n);
        auto mcEnd = std::chrono::high_resolution_clock::now();

        double mcTime = std::chrono::duration<double, std::milli>(mcEnd - mcStart).count();
        double error  = mcPrice - bsPrice;

        // Print to terminal
        std::cout << n << "\t   | " << mcPrice << "\t | "
                  << error << "\t | " << mcTime << "\n";

        // Write to CSV
        csv << n << "," << mcPrice << "," << bsPrice << ","
            << error << "," << mcTime << "\n";
    }

    // ofstream closes automatically when it goes out of scope,
    // but being explicit is good practice.
    csv.close();

    std::cout << "\nResults written to output/results.csv\n";

    return 0;  // 0 means success
}
