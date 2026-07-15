#include <fstream>
#include <iostream>
#include <string>

#include <Windows.h>

#include "DataLoader.h"
#include "Monitor.h"

namespace {

// VS에서 실행하면 작업 디렉터리가 프로젝트 폴더, exe를 직접 실행하면
// 출력 폴더가 되므로 두 경우 모두에서 data 폴더를 찾아본다.
std::string ResolveDefaultPath(const std::string& fileName) {
    const char* candidates[] = {
        "data/",
        "../data/",
        "../../data/",
        "../../../data/",
    };
    for (const char* candidate : candidates) {
        std::string path = candidate + fileName;
        std::ifstream file(path);
        if (file.good()) {
            return path;
        }
    }
    return std::string("data/") + fileName;
}

}  // namespace

int main(int argc, char** argv) {
    // 소스/데이터 파일이 모두 UTF-8이므로 콘솔 입출력 코드페이지도 UTF-8로 맞춘다.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::string samplePath = (argc > 1) ? argv[1] : ResolveDefaultPath("sample.json");
    std::string orderPath = (argc > 2) ? argv[2] : ResolveDefaultPath("order.json");

    try {
        std::vector<Sample> samples = LoadSamples(samplePath);
        std::vector<Order> orders = LoadOrders(orderPath);

        std::vector<SampleStatus> statuses = ComputeSampleStatuses(samples, orders);

        PrintOrderMonitor(orders);
        PrintStockMonitor(statuses);
        PrintProductionQueue(orders, statuses);
    } catch (const std::exception& e) {
        std::cerr << "오류: " << e.what() << "\n";
        std::cerr << "사용법: DummyDataTool.exe [sample.json 경로] [order.json 경로]\n";
        return 1;
    }

    return 0;
}
