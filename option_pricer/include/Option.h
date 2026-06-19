#ifndef OPTION_H
#define OPTION_H

// A plain bundle of the five option Greeks (sensitivities of the price).
// A struct is like a class but its members are public by default — used
// here just to group related data so we can return all five at once.
struct Greeks {
    double delta;  // dC/dS  — sensitivity to stock price
    double gamma;  // d2C/dS2 — curvature / rate of change of delta
    double vega;   // dC/dsigma — sensitivity to volatility
    double theta;  // dC/dT  — time decay
    double rho;    // dC/dr  — sensitivity to interest rate
};

// Option represents a European option (call or put) and its parameters.
// It knows how to price itself two ways:
//   - Black-Scholes (closed-form, exact)
//   - Monte Carlo  (simulation-based, approximate)
class Option {
private:
    // The five inputs that fully define a European option.
    // Private means only methods inside this class can read/write them.
    double S;      // current stock price
    double K;      // strike price
    double r;      // risk-free interest rate (annualised)
    double sigma;  // volatility (annualised)
    double T;      // time to expiry in years

    // Private helper: computes d1 from the Black-Scholes formula.
    // Shared by call price, put price, and the Greeks so we don't
    // duplicate the formula. Only callable from inside this class.
    double d1() const;
    double d2() const;

public:
    // Constructor — called when you write: Option opt(S, K, r, sigma, T);
    // It receives the five parameters and stores them in the member variables.
    Option(double S, double K, double r, double sigma, double T);

    // --- Black-Scholes closed-form prices ---
    double blackScholesPrice() const;  // call
    double blackScholesPut() const;    // put

    // --- Monte Carlo estimates ---
    // numSims: how many simulated stock paths to use (more = accurate but slow).
    double monteCarloPrice(int numSims) const;  // call
    double monteCarloPut(int numSims) const;     // put

    // Monte Carlo call price using antithetic variates (variance reduction).
    // For each draw Z it also uses -Z, averaging the two payoffs. Same number
    // of payoffs as monteCarloPrice(numSims) but with lower variance.
    double monteCarloPriceAntithetic(int numSims) const;

    // --- The Greeks (for the call) ---
    // Returns all five sensitivities bundled in a Greeks struct.
    Greeks greeks() const;
};

#endif // OPTION_H
