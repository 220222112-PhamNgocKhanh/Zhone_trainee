# Cafe POS & Loyalty System 

A C-based Point of Sale (POS) and Loyalty Card system simulation for **"Daily Brew Cafe"**.

---

##  Overview

This project simulates a real-world cafe counter ordering and billing system, handling customized drink orders, tier-based member discounts, loyalty point accruals, and receipt printing.

### Key Features
* **Menu & Customization:** Drink ordering with custom Size (Regular/Large), Sugar levels (0%, 50%, 100%), and Ice levels (0%, 50%, 100%).
* **Loyalty Tiers:**
  * **Gold:** 15% discount
  * **Silver:** 10% discount
  * **Bronze:** 5% discount
  * **Points:** Earns 1 point for every $2 spent.
* **Walk-in Guests:** Standard billing without discounts or loyalty points (`loyalty_card = NULL`).
* **Receipt Generation:** Calculates total items, member discounts, and 8% VAT, followed by an updated loyalty points summary in the footer.

---

##  Project Structure

```text
├── cafe_types.h       # Struct definitions (Customer, LoyaltyCard, MenuItem, OrderItem, Order)
├── menu_catalog.h     # Menu catalog headers
├── menu_catalog.c     # Drink menu data, unit prices, and tier discount calculations
├── order_manager.h    # Order management headers
├── order_manager.c    # Customer creation, order processing, and receipt formatting
├── main.c             # Morning rush simulation serving 4 consecutive customers
└── Makefile           # Build system configured with -g -O0 for debugging