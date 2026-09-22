#include "order_manager.h"
#include "menu_catalog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Customer *create_member_customer(int id, const char *name, const char *phone,
                                 const char *card_no, MembershipTier tier, int initial_points) {
    Customer *cust = (Customer *)malloc(sizeof(Customer));
    if (!cust) {
        perror("Failed to allocate member customer");
        exit(EXIT_FAILURE);
    }
    cust->customer_id = id;
    strncpy(cust->name, name, sizeof(cust->name) - 1);
    cust->name[sizeof(cust->name) - 1] = '\0';
    strncpy(cust->phone, phone, sizeof(cust->phone) - 1);
    cust->phone[sizeof(cust->phone) - 1] = '\0';
    cust->is_member = true;

    cust->loyalty_card = (LoyaltyCard *)malloc(sizeof(LoyaltyCard));
    if (!cust->loyalty_card) {
        perror("Failed to allocate loyalty card");
        free(cust);
        exit(EXIT_FAILURE);
    }
    strncpy(cust->loyalty_card->card_number, card_no, sizeof(cust->loyalty_card->card_number) - 1);
    cust->loyalty_card->card_number[sizeof(cust->loyalty_card->card_number) - 1] = '\0';
    cust->loyalty_card->tier = tier;
    cust->loyalty_card->accumulated_points = initial_points;

    return cust;
}

Customer *create_guest_customer(int id, const char *name) {
    Customer *cust = (Customer *)malloc(sizeof(Customer));
    if (!cust) {
        perror("Failed to allocate guest customer");
        exit(EXIT_FAILURE);
    }
    cust->customer_id = id;
    strncpy(cust->name, name, sizeof(cust->name) - 1);
    cust->name[sizeof(cust->name) - 1] = '\0';
    strcpy(cust->phone, "N/A");
    cust->is_member = false;
    cust->loyalty_card = NULL;

    return cust;
}

Order *create_order(int order_id, Customer *customer) {
    Order *order = (Order *)malloc(sizeof(Order));
    if (!order) {
        perror("Failed to allocate order");
        exit(EXIT_FAILURE);
    }
    memset(order, 0, sizeof(Order));
    order->order_id = order_id;
    order->customer = customer;
    return order;
}

int add_drink_to_order(Order *order, int item_id, DrinkSize size, int qty, int sugar, int ice) {
    if (order->item_count >= MAX_ITEMS_PER_ORDER) {
        fprintf(stderr, "[ORDER] Capacity limit reached on Order #%d\n", order->order_id);
        return -1;
    }

    const MenuItem *menu_item = find_menu_item_by_id(item_id);
    if (!menu_item) {
        fprintf(stderr, "[ORDER] Item ID %d not found in menu catalog\n", item_id);
        return -1;
    }

    OrderItem *item = &order->items[order->item_count++];
    item->item = *menu_item;
    item->size = size;
    item->quantity = qty;
    item->sugar_percent = sugar;
    item->ice_percent = ice;

    return 0;
}

static void format_receipt_body(const Order *order, char *buffer, size_t max_len) {
    char lines[768];
    lines[0] = '\0';

    for (int i = 0; i < order->item_count; i++) {
        const OrderItem *it = &order->items[i];
        double line_cost = calculate_item_price(it);
        char line_item[128];
        snprintf(line_item, sizeof(line_item),
                 " %-20s (%s) x%d  $%6.2f\n"
                 "   [Sugar: %d%% | Ice: %d%%]\n",
                 it->item.name,
                 (it->size == SIZE_LARGE) ? "L" : "R",
                 it->quantity,
                 line_cost,
                 it->sugar_percent,
                 it->ice_percent);
        strncat(lines, line_item, sizeof(lines) - strlen(lines) - 1);
    }

    snprintf(buffer, max_len,
             "================ DAILY BREW CAFE ================\n"
             "Receipt #: ORD-%04d\n"
             "Customer : %s\n"
             "----------------------------------------------------\n"
             "%s"
             "----------------------------------------------------\n"
             "Subtotal       : $%6.2f\n"
             "Member Discount: -$%5.2f\n"
             "Tax (8%%)       : $%6.2f\n"
             "FINAL TOTAL    : $%6.2f\n",
             order->order_id,
             order->customer->name,
             lines,
             order->subtotal,
             order->discount_amount,
             order->tax_amount,
             order->final_total);
}

static void format_loyalty_footer(const Order *order, char *buffer, size_t max_len) {
    const Customer *cust = order->customer;
    const LoyaltyCard *card = cust->loyalty_card;

    /* Reward points update calculation */
    int current_points = card->accumulated_points;
    int updated_balance = current_points + order->points_earned;

    const char *tier_str = "BRONZE";
    if (card->tier == TIER_GOLD) {
        tier_str = "GOLD (15% OFF)";
    } else if (card->tier == TIER_SILVER) {
        tier_str = "SILVER (10% OFF)";
    }

    snprintf(buffer, max_len,
             "----------------------------------------------------\n"
             "Loyalty Card   : %s\n"
             "Membership Tier: %s\n"
             "Points Earned  : +%-3d | New Balance: %d pts\n"
             "----------------------------------------------------\n"
             "Thank you for enjoying your coffee with us!\n"
             "====================================================",
             card->card_number, tier_str,
             order->points_earned, updated_balance);
}

void process_and_print_receipt(Order *order) {
    calculate_order_totals(order);

    char receipt_body[1024];
    char loyalty_footer[512];

    format_receipt_body(order, receipt_body, sizeof(receipt_body));
    printf("%s", receipt_body);

    format_loyalty_footer(order, loyalty_footer, sizeof(loyalty_footer));
    printf("%s\n\n", loyalty_footer);
}

void destroy_customer(Customer *cust) {
    if (!cust) return;
    if (cust->loyalty_card) {
        free(cust->loyalty_card);
    }
    free(cust);
}

void destroy_order(Order *order) {
    if (!order) return;
    free(order);
}

