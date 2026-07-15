#pragma once

#include <string>

// 주문 상태: RESERVED -> CONFIRMED -> PRODUCING -> RELEASE
// REJECTED는 무효 주문이라 모니터링에서 제외한다.
enum class OrderStatus { RESERVED, CONFIRMED, PRODUCING, RELEASE, REJECTED, UNKNOWN };

OrderStatus ParseOrderStatus(const std::string& text);
std::string ToString(OrderStatus status);

// data/order.json 의 주문 한 건
struct Order {
    std::string orderId;
    std::string sampleName;
    int quantity = 0;
    OrderStatus status = OrderStatus::UNKNOWN;
};
