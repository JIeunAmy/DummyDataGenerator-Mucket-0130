#include "Monitor.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>

namespace {

// 한글 등 동아시아 폭 넓은 문자는 콘솔에서 2칸을 차지하므로,
// std::setw(바이트/코드포인트 수 기준)만으로는 표가 어긋난다.
// UTF-8을 디코딩해 실제 표시 폭을 계산하고 그 폭에 맞춰 오른쪽에 공백을 채운다.
int DisplayWidth(const std::string& utf8) {
    int width = 0;
    size_t i = 0;
    while (i < utf8.size()) {
        unsigned char c = static_cast<unsigned char>(utf8[i]);
        uint32_t codepoint = 0;
        size_t len = 1;
        if ((c & 0x80) == 0) {
            codepoint = c;
            len = 1;
        } else if ((c & 0xE0) == 0xC0) {
            codepoint = c & 0x1F;
            len = 2;
        } else if ((c & 0xF0) == 0xE0) {
            codepoint = c & 0x0F;
            len = 3;
        } else if ((c & 0xF8) == 0xF0) {
            codepoint = c & 0x07;
            len = 4;
        }
        for (size_t j = 1; j < len && i + j < utf8.size(); ++j) {
            codepoint = (codepoint << 6) | (static_cast<unsigned char>(utf8[i + j]) & 0x3F);
        }
        bool isWide = (codepoint >= 0x1100 && codepoint <= 0x115F) ||  // 한글 자모
                      (codepoint >= 0x2E80 && codepoint <= 0xA4CF) ||  // CJK 기호/한자
                      (codepoint >= 0xAC00 && codepoint <= 0xD7A3) ||  // 한글 음절
                      (codepoint >= 0xF900 && codepoint <= 0xFAFF) ||  // CJK 호환 한자
                      (codepoint >= 0xFF00 && codepoint <= 0xFF60);    // 전각 문자
        width += isWide ? 2 : 1;
        i += len;
    }
    return width;
}

std::string PadRight(const std::string& text, int targetWidth) {
    int pad = targetWidth - DisplayWidth(text);
    return text + std::string(pad > 0 ? pad : 0, ' ');
}

}  // namespace

std::string ToKoreanString(StockStatus status) {
    switch (status) {
        case StockStatus::Sufficient: return "여유";
        case StockStatus::Shortage: return "부족";
        case StockStatus::Depleted: return "고갈";
    }
    return "알수없음";
}

std::vector<SampleStatus> ComputeSampleStatuses(const std::vector<Sample>& samples,
                                                 const std::vector<Order>& orders) {
    std::map<std::string, int> orderedQtyBySampleName;
    for (const Order& order : orders) {
        if (order.status == OrderStatus::REJECTED) {
            continue;  // 무효 주문은 재고 판정에서 제외
        }
        orderedQtyBySampleName[order.sampleName] += order.quantity;
    }

    std::vector<SampleStatus> result;
    for (const Sample& sample : samples) {
        SampleStatus status;
        status.sample = sample;

        auto it = orderedQtyBySampleName.find(sample.name);
        status.totalOrderedQty = (it != orderedQtyBySampleName.end()) ? it->second : 0;

        if (sample.currentStock == 0) {
            status.status = StockStatus::Depleted;
        } else if (status.totalOrderedQty > sample.currentStock) {
            status.status = StockStatus::Shortage;
        } else {
            status.status = StockStatus::Sufficient;
        }

        status.shortfallQty = std::max(0, status.totalOrderedQty - sample.currentStock);
        if (status.shortfallQty > 0 && sample.yieldRate > 0.0) {
            status.actualProductionQty = static_cast<int>(std::ceil(status.shortfallQty / sample.yieldRate));
        }
        status.totalProductionTimeMin = status.actualProductionQty * sample.avgProductionTimeMin;

        result.push_back(status);
    }
    return result;
}

void PrintOrderMonitor(const std::vector<Order>& orders) {
    std::cout << "====================================================================\n";
    std::cout << " 주문량 확인 (RESERVED / CONFIRMED / PRODUCING / RELEASE, REJECTED 제외)\n";
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << PadRight("주문ID", 10) << PadRight("시료명", 24) << PadRight("수량", 8) << "상태\n";
    for (const Order& order : orders) {
        if (order.status == OrderStatus::REJECTED || order.status == OrderStatus::UNKNOWN) {
            continue;
        }
        std::cout << PadRight(order.orderId, 10) << PadRight(order.sampleName, 24)
                   << PadRight(std::to_string(order.quantity), 8) << ToString(order.status) << "\n";
    }
    std::cout << "\n";
}

void PrintStockMonitor(const std::vector<SampleStatus>& statuses) {
    std::cout << "====================================================================\n";
    std::cout << " 재고량 확인 (주문 대비 재고 상태: 여유 / 부족 / 고갈)\n";
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << PadRight("ID", 8) << PadRight("시료명", 22) << PadRight("재고", 8) << PadRight("주문량", 8)
              << "상태\n";
    for (const SampleStatus& status : statuses) {
        std::cout << PadRight(status.sample.id, 8) << PadRight(status.sample.name, 22)
                   << PadRight(std::to_string(status.sample.currentStock), 8)
                   << PadRight(std::to_string(status.totalOrderedQty), 8) << ToKoreanString(status.status) << "\n";
    }
    std::cout << "\n";
}

void PrintProductionQueue(const std::vector<Order>& orders, const std::vector<SampleStatus>& statuses) {
    std::map<std::string, const SampleStatus*> statusBySampleName;
    for (const SampleStatus& status : statuses) {
        statusBySampleName[status.sample.name] = &status;
    }

    std::cout << "====================================================================\n";
    std::cout << " 생산 대기 큐 (재고 부족 시료, FIFO)\n";
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << PadRight("주문ID", 10) << PadRight("시료명", 24) << PadRight("부족분", 8) << PadRight("실 생산량", 12)
              << "총 생산시간(min)\n";

    bool any = false;
    for (const Order& order : orders) {
        if (order.status == OrderStatus::REJECTED || order.status == OrderStatus::UNKNOWN) {
            continue;
        }
        auto it = statusBySampleName.find(order.sampleName);
        if (it == statusBySampleName.end() || it->second->shortfallQty == 0) {
            continue;
        }
        const SampleStatus& status = *it->second;
        std::cout << PadRight(order.orderId, 10) << PadRight(order.sampleName, 24)
                   << PadRight(std::to_string(status.shortfallQty), 8)
                   << PadRight(std::to_string(status.actualProductionQty), 12) << status.totalProductionTimeMin
                   << "\n";
        any = true;
    }
    if (!any) {
        std::cout << " (생산이 필요한 주문 없음)\n";
    }
    std::cout << "\n";
}
