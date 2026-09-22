#include "menu_catalog.h"
#include <stdio.h>
#include <string.h>

#define CATALOG_SIZE 5

static MenuItem g_catalog[CATALOG_SIZE];

void catalog_init(void) {
    g_catalog[0] = (MenuItem){101, "Caramel Macchiato", "Coffee", 4.50};
    g_catalog[1] = (MenuItem){102, "Matcha Latte",      "Tea",    4.25};
    g_catalog[2] = (MenuItem){103, "Cold Brew Special", "Coffee", 3.75};
    g_catalog[3] = (MenuItem){104, "Iced Americano",    "Coffee", 3.25};
    g_catalog[4] = (MenuItem){105, "Butter Croissant",  "Bakery", 2.80};
}

const MenuItem *find_menu_item_by_id(int item_id) {
    for (int i = 0; i < CATALOG_SIZE; i++) {
        if (g_catalog[i].item_id == item_id) {
            return &g_catalog[i];
        }
    }
    return NULL;
}

double calculate_item_price(const OrderItem *order_item) {
    double unit_price = order_item->item.base_price;
    if (order_item->size == SIZE_LARGE) {
        unit_price += 0.80;
    }
    return unit_price * order_item->quantity;
}

void calculate_order_totals(Order *order) {
    double subtotal = 0.0;
    for (int i = 0; i < order->item_count; i++) {
        subtotal += calculate_item_price(&order->items[i]);
    }
    order->subtotal = subtotal;

    double discount = 0.0;
    if (order->customer->is_member && order->customer->loyalty_card) {
        switch (order->customer->loyalty_card->tier) {
            case TIER_GOLD:
                discount = subtotal * 0.15;
                break;
            case TIER_SILVER:
                discount = subtotal * 0.10;
                break;
            case TIER_BRONZE:
                discount = subtotal * 0.05;
                break;
        }
    }
    order->discount_amount = discount;

    double taxable = subtotal - discount;
    order->tax_amount = taxable * 0.08;
    order->final_total = taxable + order->tax_amount;

    order->points_earned = (int)(order->final_total / 2.0);
}

