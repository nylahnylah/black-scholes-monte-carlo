# Option Pricer

A C++ pricer for European options that values a call and a put two independent ways: the **Black-Scholes** closed-form formula and **Monte Carlo** simulation, and verifies them against each other. Results are exported to CSV and visualised in a Python notebook.

The project demonstrates that Monte Carlo converges to the analytical Black-Scholes price at the theoretical rate of `1/√N`, and that **antithetic variates** reduce the Monte Carlo error by roughly an order of magnitude at the same computational cost.

---

## Features

- **Black-Scholes** closed-form pricing for calls and puts
- **Monte Carlo** pricing under the risk-neutral measure, for calls and puts
- **Antithetic variates** variance reduction for Monte Carlo
- **The Greeks** — delta, gamma, vega, theta, rho (closed-form)
- **Put-call parity** check as a built-in correctness test
- **Command-line input** with validation
- **Python notebook** plotting convergence and variance reduction

---

## Project structure

```
option_pricer/
├── include/
│   └── Option.h          # Option class + Greeks struct
├── src/
│   ├── BlackScholes.cpp  # constructor, d1/d2, call, put, Greeks
│   ├── MonteCarlo.cpp    # MC call, MC put, antithetic MC
│   └── main.cpp          # CLI, runs everything, writes CSV
├── output/               # generated results.csv and convergence.png
├── python/
│   └── analysis.ipynb    # reads CSV, plots convergence
├── Makefile
└── README.md
```

---

## Build & run

Requires a C++17 compiler (e.g. `g++`) and `make`.

```bash
make                 # compile -> pricer.exe
./pricer.exe         # run with default parameters
```

Pass your own parameters as `S K r sigma T`:

```bash
./pricer.exe 100 105 0.05 0.2 1.0
```

| Argument | Meaning                              | Example |
|----------|--------------------------------------|---------|
| `S`      | current stock price                  | 100     |
| `K`      | strike price                         | 105     |
| `r`      | risk-free interest rate (annualised) | 0.05    |
| `sigma`  | volatility (annualised)              | 0.20    |
| `T`      | time to expiry in years              | 1.0     |

Results are written to `output/results.csv`.

---

## Analysis

Open `python/analysis.ipynb` (requires `pandas`, `matplotlib`, `numpy`) and run all cells. It produces three plots:

1. **Convergence** - standard and antithetic Monte Carlo prices approaching the Black-Scholes price
2. **Error (log-log)** - both methods against the theoretical `1/√N` reference; antithetic sits below standard, confirming variance reduction
3. **Timing** - computation time scaling linearly with the number of simulations

---

## The theory in brief

A European **call** gives the right to buy an asset at strike `K` at expiry `T`; its payoff is `max(S_T − K, 0)`. Under the **risk-neutral measure**, the fair price of any derivative is the discounted expected payoff:

```
price = e^(−rT) · E[ payoff(S_T) ]
```

with the stock evolving as geometric Brownian motion:

```
S_T = S · exp( (r − ½σ²)T + σ√T · Z ),   Z ~ N(0,1)
```

**Black-Scholes** evaluates this expectation analytically:

```
Call:  C = S·N(d1) − K·e^(−rT)·N(d2)
Put:   P = K·e^(−rT)·N(−d2) − S·N(−d1)

d1 = [ln(S/K) + (r + ½σ²)T] / (σ√T)
d2 = d1 − σ√T
```

**Monte Carlo** evaluates the same expectation numerically by simulating many `S_T` and averaging the discounted payoffs. It is approximate (error `~1/√N`) but works for any payoff, including those with no closed form.

**Put-call parity** (`C − P = S − K·e^(−rT)`) is a model-free no-arbitrage identity used here to cross-check the call and put prices.

---

## Possible extensions

- American options (binomial tree / least-squares Monte Carlo)
- Path-dependent payoffs (Asian, barrier options)
- Multithreaded Monte Carlo
- Implied volatility solver (invert Black-Scholes via Newton-Raphson)
