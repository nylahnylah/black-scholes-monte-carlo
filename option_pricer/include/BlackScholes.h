// Include guard — prevents this file being processed twice if included from
// multiple places. The name BLACKSCHOLES_H is just a convention (filename
// in caps with underscores).
#ifndef BLACKSCHOLES_H
#define BLACKSCHOLES_H

// Returns the Black-Scholes price of a European call option.
//
// Parameters:
//   S     : current stock price
//   K     : strike price
//   r     : risk-free interest rate (annualised, e.g. 0.05 for 5%)
//   sigma : volatility (annualised, e.g. 0.20 for 20%)
//   T     : time to expiry in years (e.g. 1.0 for one year)
//
// Returns:
//   call option price as a double
double callPrice(double S, double K, double r, double sigma, double T);

#endif // BLACKSCHOLES_H
