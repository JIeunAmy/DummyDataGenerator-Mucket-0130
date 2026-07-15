#pragma once

#include <string>

// data/sample.json 의 시료 한 건 (13쪽 시료 목록 화면 예시 기준)
struct Sample {
    std::string id;
    std::string name;
    double avgProductionTimeMin = 0.0;  // 평균 생산시간 (min/ea)
    double yieldRate = 0.0;             // 수율 (0.0 ~ 1.0)
    int currentStock = 0;               // 현재 재고
};
