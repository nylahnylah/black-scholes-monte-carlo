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
