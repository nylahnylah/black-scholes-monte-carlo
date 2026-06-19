#include "Option.h"  // tells compiler about the Option class
#include <cmath>     // exp(), log(), sqrt(), erfc()

// Helper: standard normal CDF.
// Identity: N(x) = 0.5 * erfc(-x / sqrt(2))
// 'static' keeps this function private to this file.
static double normalCDF(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Helper: standard normal PDF (the bell curve density), N'(x).
//   N'(x) = exp(-x^2 / 2) / sqrt(2*pi)
// Needed for gamma, vega and theta.
static double normalPDF(double x)
{
    static const double inv_sqrt_2pi = 0.3989422804014327;  // 1 / sqrt(2*pi)
    return inv_sqrt_2pi * std::exp(-0.5 * x * x);
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

// d1 and d2 helpers. Computed once here and reused by the call price,
// put price, and (later) the Greeks. These are member functions, so they
// read S, K, r, sigma, T directly off the object.
//   d1 = [ log(S/K) + (r + 0.5*sigma^2)*T ] / (sigma*sqrt(T))
//   d2 = d1 - sigma*sqrt(T)
double Option::d1() const
{
    return (std::log(S / K) + (r + 0.5 * sigma * sigma) * T)
           / (sigma * std::sqrt(T));
}

double Option::d2() const
{
    return d1() - sigma * std::sqrt(T);
}

// Black-Scholes European CALL price.
// Formula: C = S*N(d1) - K*exp(-r*T)*N(d2)
double Option::blackScholesPrice() const
{
    return S * normalCDF(d1()) - K * std::exp(-r * T) * normalCDF(d2());
}

// Black-Scholes European PUT price.
// Formula: P = K*exp(-r*T)*N(-d2) - S*N(-d1)
// The signs on d1, d2 flip because a put pays off when the stock
// finishes BELOW the strike, so we want the lower-tail probabilities.
double Option::blackScholesPut() const
{
    return K * std::exp(-r * T) * normalCDF(-d2()) - S * normalCDF(-d1());
}

// The five Greeks for the call, all closed-form derivatives of the BS price.
Greeks Option::greeks() const
{
    double D1 = d1();
    double D2 = d2();
    double sqrtT = std::sqrt(T);
    double discK = K * std::exp(-r * T);  // discounted strike, reused below

    Greeks g;
    // Delta = N(d1): change in call price per unit change in S.
    g.delta = normalCDF(D1);
    // Gamma = N'(d1) / (S*sigma*sqrt(T)): curvature, same for call and put.
    g.gamma = normalPDF(D1) / (S * sigma * sqrtT);
    // Vega = S*N'(d1)*sqrt(T): sensitivity to volatility.
    g.vega  = S * normalPDF(D1) * sqrtT;
    // Theta = -S*N'(d1)*sigma/(2*sqrt(T)) - r*K*exp(-rT)*N(d2): time decay.
    g.theta = -(S * normalPDF(D1) * sigma) / (2.0 * sqrtT)
              - r * discK * normalCDF(D2);
    // Rho = K*T*exp(-rT)*N(d2): sensitivity to interest rate.
    g.rho   = T * discK * normalCDF(D2);

    return g;
}
