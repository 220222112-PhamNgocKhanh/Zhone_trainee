#ifndef CACHE_TYPES_H
#define CACHE_TYPES_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_ITEMS_PER_ORDER 8
#define MAX_NAME_LEN        32

typedef enum {
    SIZE_REGULAR,
    SIZE_LARGE
} DrinkSize;

typedef enum {
    TIER_BRONZE,
    TIER_SILVER,
    TIER_GOLD
} MembershipTier;

typedef struct {
    char           card_number[16];
    MembershipTier tier;
    int            accumulated_points;
    int            discount_percent;
} LoyaltyCard;

typedef struct {
    int          customer_id;
    char         name[MAX_NAME_LEN];
    char         phone[16];
    bool         is_member;
    LoyaltyCard *loyalty_card;
} Customer;

typedef struct {
    int   item_id;
    char  name[MAX_NAME_LEN];
    char  category[16];
    double base_price;
} MenuItem;

typedef struct {
    MenuItem  item;
    DrinkSize size;
    int       quantity;
    int       sugar_percent;
    int       ice_percent;
} OrderItem;

typedef struct {
    int       order_id;
    Customer *customer;
    OrderItem items[MAX_ITEMS_PER_ORDER];
    int       item_count;
    double    subtotal;
    double    discount_amount;
    double    tax_amount;
    double    final_total;
    int       points_earned;
} Order;

#endif /* CACHE_TYPES_H */

