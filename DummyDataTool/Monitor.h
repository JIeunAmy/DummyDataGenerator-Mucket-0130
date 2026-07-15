#pragma once

#include <vector>

#include "Order.h"
#include "Sample.h"

// 재고 상태 판정: 여유 / 부족 / 고갈
enum class StockStatus { Sufficient, Shortage, Depleted };

std::string ToKoreanString(StockStatus status);

struct SampleStatus {
    Sample sample;
    int totalOrderedQty = 0;         // REJECTED를 제외한 유효 주문 수량 합계
    StockStatus status = StockStatus::Sufficient;
    int shortfallQty = 0;            // 주문 대비 재고 부족분
    int actualProductionQty = 0;     // ceil(부족분 / 수율)
    double totalProductionTimeMin = 0.0;  // 평균 생산시간 * 실 생산량
};

// 시료별 주문 대비 재고 상태 및 생산 필요량을 계산한다.
std::vector<SampleStatus> ComputeSampleStatuses(const std::vector<Sample>& samples,
                                                 const std::vector<Order>& orders);

// [주문량 확인] 화면: 상태별(RESERVED/CONFIRMED/PRODUCING/RELEASE) 주문 목록 (REJECTED 제외)
void PrintOrderMonitor(const std::vector<Order>& orders);

// [재고량 확인] 화면: 시료별 현재 재고 및 주문 대비 상태
void PrintStockMonitor(const std::vector<SampleStatus>& statuses);

// 생산라인 화면: 재고 부족으로 생산이 필요한 시료를 FIFO(주문 등록 순서)로 나열
void PrintProductionQueue(const std::vector<Order>& orders, const std::vector<SampleStatus>& statuses);
