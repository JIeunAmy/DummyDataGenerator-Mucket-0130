# CLAUDE.md

이 파일은 Claude Code가 이 저장소에서 작업할 때 참고하는 컨텍스트입니다.

## 프로젝트 개요

**Sample(시료) Dummy Data Tool** — 반도체 시료(Sample) 재고 및 주문 흐름을 시뮬레이션하기 위한
더미 데이터 생성/관리 툴입니다.

- 과제 원본 명세: `docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf`
  - 13쪽: 시료 관리 화면 예시 (시료 등록 / 시료 목록 / 시료 검색)
  - 18~19쪽: 모니터링 화면 예시 (주문량 확인 / 재고량 확인)
  - 20쪽: 생산라인 화면 예시 (생산 현황, 대기 주문 큐, FIFO 스케줄링)
  - README.md의 예시 화면은 이 PDF의 화면을 텍스트로 재현한 것이며, 실제 표기 형식은 자유롭게 변경 가능

## 기술 스택

- Visual Studio C++ 프로젝트 (`DummyDataTool.slnx` → `DummyDataTool/DummyDataTool.vcxproj`)
- 구성: Debug/Release × Win32/x64, Console Application, C++20 (`stdcpp20`)
- 아직 실제 소스 코드(`.cpp`/`.h`)는 작성되지 않은 초기 스캐폴드 상태

## 디렉터리 구조

- `DummyDataTool/` — VC++ 프로젝트 파일 위치 (소스 추가 예정)
- `docs/` — 과제 명세 PDF
- `data/sample.json` — 시료 목록 더미 데이터 (13쪽 예시 기반: ID / 시료명 / 평균 생산시간 / 수율 / 현재 재고)
- `data/order.json` — 주문 더미 데이터 (orderId / sampleName / quantity / status)
- `README.md` — 프로젝트 소개 및 화면 예시

## 도메인 개념

- **시료(Sample)**: ID, 이름, 평균 생산시간(min/ea), 수율, 현재 재고 수량을 가짐
- **주문 상태**: RESERVED → CONFIRMED → PRODUCING → RELEASE (REJECTED는 무효 주문이라 모니터링에서 제외)
- **재고 상태 판정**: 주문 대비 재고 수량 기준으로 여유 / 부족(부족분 존재) / 고갈(0개) 세 단계로 표기
- **생산 로직**: 실 생산량 = ceil(부족분 / 수율), 총 생산 시간 = 평균 생산시간 × 실 생산량, 생산 큐는 FIFO

## 저장소

- 원격: `origin` → https://github.com/JIeunAmy/DummyDataGenerator-Mucket-0130 (기본 브랜치 `main`)

## 작업 시 유의사항

- 화면/데이터 예시를 바꿀 때는 PDF의 해당 쪽 번호를 이미지로 렌더링해 확인 후 반영할 것
  (텍스트 추출만으로는 표/이미지 형태 내용이 깨지므로, PyMuPDF(`fitz`)로 페이지를 PNG로 렌더링해서 확인)
