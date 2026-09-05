# Lab 2: Reentrant Function

This lab demonstrates the difference between a **non-reentrant** and a
**reentrant** function when used by multiple threads.

## Overview

* **`non_reentrant.c`**: Uses the shared global variable `TAX_RATE` to
  calculate prices.
* **`reentrant.c`**: Passes `tax_rate` as a function argument and gives each
  thread its own `ProductData`.

The reentrant version does not depend on shared global data, so it is safer
to call from multiple threads.

## Compilation and Execution

### 1. Non-reentrant version

```bash
gcc -o non_reentrant non_reentrant.c
./non_reentrant
```

### 2. Reentrant version

```bash
gcc -o reentrant reentrant.c
./reentrant
```

## Fix

The non-reentrant version is fixed by:

1. Removing the global `TAX_RATE` dependency.
2. Passing `tax_rate` to `calculate_price()`.
3. Storing each thread's price and tax rate in its own `ProductData`.
