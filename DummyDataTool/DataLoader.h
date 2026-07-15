#pragma once

#include <string>
#include <vector>

#include "Order.h"
#include "Sample.h"

// 파일을 열 수 없거나 형식이 예상과 다르면 std::runtime_error를 던진다.
std::vector<Sample> LoadSamples(const std::string& path);
std::vector<Order> LoadOrders(const std::string& path);
