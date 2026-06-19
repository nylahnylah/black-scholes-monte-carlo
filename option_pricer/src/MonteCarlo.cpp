#include "Option.h"   // Option class definition
#include <cmath>      // exp(), sqrt()
#include <random>     // mt19937, normal_distribution
#include <algorithm>  // std::max()

// Monte Carlo estimate of the European call price.
// Under the risk-neutral measure Q, the stock price at expiry is:
//   S_T = S * exp((r - 0.5*sigma^2)*T + sigma*sqrt(T)*Z),  Z ~ N(0,1)
// We simulate this numSims times, average the discounted payoffs.
double Option::monteCarloPrice(int numSims) const
{
    // Set up the random number generator.
    // mt19937 is the Mersenne Twister engine — fast, high quality.
    // Seeded with 42 for reproducibility (same seed = same results every run).
    std::mt19937 rng(42);

    // A distribution object that maps random bits to N(0,1) draws.
    std::normal_distribution<double> dist(0.0, 1.0);

    // Precompute terms that don't change across simulations.
    // Doing this outside the loop avoids recomputing them numSims times.
    double drift   = (r - 0.5 * sigma * sigma) * T;
    double diffusion = sigma * std::sqrt(T);
    double discount  = std::exp(-r * T);

    double payoffSum = 0.0;

    for (int i = 0; i < numSims; i++) {
        double Z   = dist(rng);                    // draw Z ~ N(0,1)
        double ST  = S * std::exp(drift + diffusion * Z);  // stock price at expiry
        double payoff = std::max(ST - K, 0.0);     // call payoff: max(S_T - K, 0)
        payoffSum += payoff;
    }

    // Average payoff, discounted back to today
    return discount * (payoffSum / numSims);
}

// Monte Carlo call price with antithetic variates.
// Each iteration draws one Z and forms two mirrored stock prices from
// +Z and -Z. Because their payoffs are negatively correlated, averaging
// them reduces the variance of the estimator at almost no extra cost.
double Option::monteCarloPriceAntithetic(int numSims) const
{
    std::mt19937 rng(42);
    std::normal_distribution<double> dist(0.0, 1.0);

    double drift     = (r - 0.5 * sigma * sigma) * T;
    double diffusion = sigma * std::sqrt(T);
    double discount  = std::exp(-r * T);

    double payoffSum = 0.0;

    // Each iteration produces TWO payoffs (from +Z and -Z), so we only
    // need numSims/2 iterations to get numSims total payoffs.
    int pairs = numSims / 2;

    for (int i = 0; i < pairs; i++) {
        double Z = dist(rng);

        // Mirrored stock prices from +Z and -Z
        double STplus  = S * std::exp(drift + diffusion * Z);
        double STminus = S * std::exp(drift - diffusion * Z);

        double payoffPlus  = std::max(STplus  - K, 0.0);
        double payoffMinus = std::max(STminus - K, 0.0);

        // Add both payoffs; the pair acts as one lower-variance sample.
        payoffSum += payoffPlus + payoffMinus;
    }

    // Divide by total number of payoffs (2 per pair)
    return discount * (payoffSum / (2 * pairs));
}

// Monte Carlo estimate of the European PUT price.
// Identical simulation, but the payoff is max(K - S_T, 0).
double Option::monteCarloPut(int numSims) const
{
    std::mt19937 rng(42);
    std::normal_distribution<double> dist(0.0, 1.0);

    double drift     = (r - 0.5 * sigma * sigma) * T;
    double diffusion = sigma * std::sqrt(T);
    double discount  = std::exp(-r * T);

    double payoffSum = 0.0;

    for (int i = 0; i < numSims; i++) {
        double Z      = dist(rng);
        double ST     = S * std::exp(drift + diffusion * Z);
        double payoff = std::max(K - ST, 0.0);  // put payoff: max(K - S_T, 0)
        payoffSum += payoff;
    }

    return discount * (payoffSum / numSims);
}
