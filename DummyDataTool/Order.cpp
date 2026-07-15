#include "Order.h"

OrderStatus ParseOrderStatus(const std::string& text) {
    if (text == "RESERVED") return OrderStatus::RESERVED;
    if (text == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (text == "PRODUCING") return OrderStatus::PRODUCING;
    if (text == "RELEASE") return OrderStatus::RELEASE;
    if (text == "REJECTED") return OrderStatus::REJECTED;
    return OrderStatus::UNKNOWN;
}

std::string ToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::RESERVED: return "RESERVED";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::PRODUCING: return "PRODUCING";
        case OrderStatus::RELEASE: return "RELEASE";
        case OrderStatus::REJECTED: return "REJECTED";
        default: return "UNKNOWN";
    }
}
