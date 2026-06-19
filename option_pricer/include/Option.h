#ifndef OPTION_H
#define OPTION_H

// Option represents a European call option and its parameters.
// It knows how to price itself using two methods:
//   - Black-Scholes (closed-form, exact)
//   - Monte Carlo  (simulation-based, approximate)
class Option {
private:
    // The five inputs that fully define a European call option.
    // Private means only methods inside this class can read/write them.
    double S;      // current stock price
    double K;      // strike price
    double r;      // risk-free interest rate (annualised)
    double sigma;  // volatility (annualised)
    double T;      // time to expiry in years

public:
    // Constructor — called when you write: Option opt(S, K, r, sigma, T);
    // It receives the five parameters and stores them in the member variables.
    Option(double S, double K, double r, double sigma, double T);

    // Returns the exact Black-Scholes call price.
    // 'const' means this method promises not to modify any member variables.
    double blackScholesPrice() const;

    // Returns a Monte Carlo estimate of the call price.
    // numSims: how many simulated stock paths to use.
    // More sims = more accurate but slower.
    double monteCarloPrice(int numSims) const;
};

#endif // OPTION_H
