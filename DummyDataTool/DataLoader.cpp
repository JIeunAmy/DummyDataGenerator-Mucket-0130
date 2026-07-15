#include "DataLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "Json.h"

namespace {

std::string ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("파일을 열 수 없습니다: " + path);
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

}  // namespace

std::vector<Sample> LoadSamples(const std::string& path) {
    JsonValue root = JsonParser::Parse(ReadFile(path));
    std::vector<Sample> samples;
    for (const JsonValue& item : root.Get("samples").AsArray()) {
        Sample sample;
        sample.id = item.Get("id").AsString();
        sample.name = item.Get("name").AsString();
        sample.avgProductionTimeMin = item.Get("avgProductionTimeMin").AsDouble();
        sample.yieldRate = item.Get("yieldRate").AsDouble();
        sample.currentStock = item.Get("currentStock").AsInt();
        samples.push_back(sample);
    }
    return samples;
}

std::vector<Order> LoadOrders(const std::string& path) {
    JsonValue root = JsonParser::Parse(ReadFile(path));
    std::vector<Order> orders;
    for (const JsonValue& item : root.Get("orders").AsArray()) {
        Order order;
        order.orderId = item.Get("orderId").AsString();
        order.sampleName = item.Get("sampleName").AsString();
        order.quantity = item.Get("quantity").AsInt();
        order.status = ParseOrderStatus(item.Get("status").AsString());
        orders.push_back(order);
    }
    return orders;
}
