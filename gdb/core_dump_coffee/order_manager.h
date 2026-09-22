#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "cafe_types.h"

Customer *create_member_customer(int id, const char *name, const char *phone,
                                 const char *card_no, MembershipTier tier, int initial_points);
Customer *create_guest_customer(int id, const char *name);

Order *create_order(int order_id, Customer *customer);
int add_drink_to_order(Order *order, int item_id, DrinkSize size, int qty, int sugar, int ice);

void process_and_print_receipt(Order *order);

void destroy_customer(Customer *cust);
void destroy_order(Order *order);

#endif /* ORDER_MANAGER_H */

