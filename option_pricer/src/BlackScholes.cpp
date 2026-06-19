#include "Option.h"  // tells compiler about the Option class
#include <cmath>     // exp(), log(), sqrt(), erfc()

// Helper: standard normal CDF.
// Identity: N(x) = 0.5 * erfc(-x / sqrt(2))
// 'static' keeps this function private to this file.
static double normalCDF(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Constructor definition.
// The colon syntax is an initialiser list — it directly initialises
// each member variable from the corresponding parameter.
// This is the preferred C++ way to set member variables in a constructor.
Option::Option(double S, double K, double r, double sigma, double T)
    : S(S), K(K), r(r), sigma(sigma), T(T)
{
    // Nothing else needed — all members are set by the initialiser list.
}

// Black-Scholes European call price.
// Because this is a member function of Option, it can access
// S, K, r, sigma, T directly — they belong to this object.
//
// Formula: C = S*N(d1) - K*exp(-r*T)*N(d2)
//   d1 = [ log(S/K) + (r + 0.5*sigma^2)*T ] / (sigma*sqrt(T))
//   d2 = d1 - sigma*sqrt(T)
double Option::blackScholesPrice() const
{
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T)
                / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    return S * normalCDF(d1) - K * std::exp(-r * T) * normalCDF(d2);
}
