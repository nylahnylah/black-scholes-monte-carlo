#include "Option.h"    // Option class
#include <iostream>    // std::cout
#include <fstream>     // std::ofstream
#include <chrono>      // timing
#include <vector>      // std::vector
#include <cmath>       // std::exp (for parity check)
#include <string>      // std::stod (parse text -> double)

// argc = argument count (includes program name).
// argv = array of argument text strings; argv[0] is the program name.
int main(int argc, char* argv[])
{
    // --- Option parameters ---
    // Defaults: stock 100, strike 105, 5% rate, 20% vol, 1 year.
    double S     = 100.0;
    double K     = 105.0;
    double r     = 0.05;
    double sigma = 0.20;
    double T     = 1.0;

    // If the user supplied exactly 5 numbers, use them; otherwise default.
    // argc == 6 means program name + 5 arguments.
    if (argc == 6) {
        S     = std::stod(argv[1]);
        K     = std::stod(argv[2]);
        r     = std::stod(argv[3]);
        sigma = std::stod(argv[4]);
        T     = std::stod(argv[5]);
    } else if (argc != 1) {
        // Wrong number of arguments — show usage and exit with an error code.
        std::cout << "Usage: " << argv[0] << " S K r sigma T\n";
        std::cout << "Example: " << argv[0] << " 100 105 0.05 0.2 1.0\n";
        return 1;
    } else {
        std::cout << "(No arguments given -- using default parameters. "
                  << "Run with: pricer S K r sigma T)\n\n";
    }

    // --- Input validation ---
    // S, K, sigma, T must be strictly positive or the formulas break
    // (e.g. log(S/K), division by sigma*sqrt(T)). r may be negative.
    if (S <= 0.0 || K <= 0.0 || sigma <= 0.0 || T <= 0.0) {
        std::cout << "ERROR: S, K, sigma and T must all be positive.\n";
        return 1;
    }

    std::cout << "Pricing call/put with: "
              << "S=" << S << " K=" << K << " r=" << r
              << " sigma=" << sigma << " T=" << T << "\n\n";

    Option opt(S, K, r, sigma, T);

    // --- Black-Scholes (exact, closed-form) ---
    // std::chrono::high_resolution_clock::now() records the current time.
    // We record before and after, then take the difference.
    auto bsStart = std::chrono::high_resolution_clock::now();
    double bsPrice = opt.blackScholesPrice();
    auto bsEnd = std::chrono::high_resolution_clock::now();

    // Duration in microseconds as a double
    double bsTime = std::chrono::duration<double, std::micro>(bsEnd - bsStart).count();

    std::cout << "Black-Scholes call:  " << bsPrice << "\n";
    std::cout << "Black-Scholes time:  " << bsTime << " microseconds\n\n";

    // --- Put price and put-call parity check ---
    // Put-call parity is a model-free no-arbitrage identity:
    //   C - P = S - K*exp(-r*T)
    // If our call and put prices are correct, both sides must match.
    double bsPut = opt.blackScholesPut();
    double lhs = bsPrice - bsPut;            // C - P
    double rhs = S - K * std::exp(-r * T);   // S - K*exp(-r*T)

    std::cout << "Black-Scholes put:   " << bsPut << "\n";
    std::cout << "Put-call parity check:\n";
    std::cout << "  C - P            = " << lhs << "\n";
    std::cout << "  S - K*exp(-rT)   = " << rhs << "\n";
    std::cout << "  difference       = " << (lhs - rhs) << "  (should be ~0)\n\n";

    // --- The Greeks (call) ---
    Greeks g = opt.greeks();
    std::cout << "Greeks (call):\n";
    std::cout << "  Delta = " << g.delta << "\n";
    std::cout << "  Gamma = " << g.gamma << "\n";
    std::cout << "  Vega  = " << g.vega  << "\n";
    std::cout << "  Theta = " << g.theta << "\n";
    std::cout << "  Rho   = " << g.rho   << "\n\n";

    // --- Monte Carlo (simulation, approximate) ---
    // Run with increasing number of simulations to show convergence.
    // std::vector is a resizable array — like a Python list but typed.
    std::vector<int> simCounts = {100, 1000, 10000, 100000, 1000000};

    // Open the CSV file for writing.
    // std::ofstream opens a file — if it doesn't exist, it creates it.
    std::ofstream csv("output/results.csv");

    if (!csv.is_open()) {
        std::cout << "ERROR: could not open output/results.csv\n";
        return 1;
    }

    // Write the header row. We now record both the standard MC error and
    // the antithetic MC error so we can compare variance reduction.
    csv << "numSims,mcPrice,mcError,antiPrice,antiError,bsPrice,timeMs\n";

    std::cout << "Monte Carlo convergence (standard vs antithetic):\n";
    std::cout << "numSims    | mcError      | antiError    | time (ms)\n";
    std::cout << "-------------------------------------------------------\n";

    for (int n : simCounts) {
        // 'for (int n : simCounts)' is a range-based for loop —
        // it iterates over every element in simCounts, calling each one n.
        // Equivalent to Python's 'for n in sim_counts'.

        auto mcStart = std::chrono::high_resolution_clock::now();
        double mcPrice = opt.monteCarloPrice(n);
        auto mcEnd = std::chrono::high_resolution_clock::now();
        double mcTime = std::chrono::duration<double, std::milli>(mcEnd - mcStart).count();

        // Antithetic estimate for the same number of payoffs
        double antiPrice = opt.monteCarloPriceAntithetic(n);

        double mcError   = mcPrice - bsPrice;
        double antiError = antiPrice - bsPrice;

        // Print both errors so the variance reduction is visible
        std::cout << n << "\t   | " << mcError << "\t | "
                  << antiError << "\t | " << mcTime << "\n";

        // Write to CSV
        csv << n << "," << mcPrice << "," << mcError << ","
            << antiPrice << "," << antiError << ","
            << bsPrice << "," << mcTime << "\n";
    }

    // ofstream closes automatically when it goes out of scope,
    // but being explicit is good practice.
    csv.close();

    std::cout << "\nResults written to output/results.csv\n";

    return 0;  // 0 means success
}
