#ifndef MENU_CATALOG_H
#define MENU_CATALOG_H

#include "cafe_types.h"

void catalog_init(void);
const MenuItem *find_menu_item_by_id(int item_id);
double calculate_item_price(const OrderItem *order_item);
void calculate_order_totals(Order *order);

#endif /* MENU_CATALOG_H */

