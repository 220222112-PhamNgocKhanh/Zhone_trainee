#include "cafe_types.h"
#include "menu_catalog.h"
#include "order_manager.h"
#include <stdio.h>

static void simulate_morning_rush(void) {
    printf("====================================================\n");
    printf("     DAILY BREW CAFE - POINT OF SALE (POS) SYSTEM\n");
    printf("====================================================\n\n");

    /* 1. Register members and guest customers */
    Customer *alice   = create_member_customer(1, "Alice Jenkins", "555-0191", "GC-8801", TIER_GOLD, 120);
    Customer *bob     = create_member_customer(2, "Bob Martin",    "555-0244", "SC-4420", TIER_SILVER, 45);
    Customer *charlie = create_member_customer(3, "Charlie Davis", "555-0318", "BC-1105", TIER_BRONZE, 15);
    Customer *david   = create_guest_customer(4, "David (Walk-in Guest)");

    /* 2. Order #101: Alice (VIP Gold) */
    Order *ord1 = create_order(101, alice);
    add_drink_to_order(ord1, 101, SIZE_LARGE, 1, 50, 100);   /* Caramel Macchiato Large */
    add_drink_to_order(ord1, 105, SIZE_REGULAR, 1, 0, 0);    /* Butter Croissant */

    /* 3. Order #102: Bob (Silver Member) */
    Order *ord2 = create_order(102, bob);
    add_drink_to_order(ord2, 102, SIZE_REGULAR, 2, 100, 50); /* 2x Matcha Latte */

    /* 4. Order #103: Charlie (Bronze Member) */
    Order *ord3 = create_order(103, charlie);
    add_drink_to_order(ord3, 103, SIZE_LARGE, 1, 0, 100);    /* Cold Brew Special Large */

    /* 5. Order #104: David (Walk-in Guest - To Go) */
    Order *ord4 = create_order(104, david);
    add_drink_to_order(ord4, 104, SIZE_LARGE, 1, 50, 100);   /* Iced Americano Large */

    /* 6. Process and print receipts */
    printf("[POS] Processing Order Queue...\n\n");

    process_and_print_receipt(ord1);
    process_and_print_receipt(ord2);
    process_and_print_receipt(ord3);
    process_and_print_receipt(ord4);

    /* 7. Clean up */
    destroy_order(ord1);
    destroy_order(ord2);
    destroy_order(ord3);
    destroy_order(ord4);

    destroy_customer(alice);
    destroy_customer(bob);
    destroy_customer(charlie);
    destroy_customer(david);
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    catalog_init();
    simulate_morning_rush();

    return 0;
}

