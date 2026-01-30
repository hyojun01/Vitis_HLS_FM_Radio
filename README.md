# Vitis HLS FM Radio

FM Radio DSP Hardware Acceleration using Vitis HLS.

## 📖 Overview

이 프로젝트는 FM 라디오 수신기의 디지털 신호 처리(DSP) 파이프라인을 Xilinx Vitis HLS를 사용하여 하드웨어 가속화한 구현입니다. 모든 블록은 AXI Stream 인터페이스를 사용하여 데이터를 스트리밍 방식으로 처리하며, `II=1` (Initiation Interval = 1)로 최적화되어 매 클럭 사이클마다 새로운 데이터를 처리할 수 있습니다.

## 🏗️ Architecture

```
┌─────────────────────┐    ┌─────────────────────┐    ┌─────────────────────┐    ┌─────────────────────┐
│   Low Pass Filter   │───▶│     Quadrature      │───▶│   FIR Decimation    │───▶│   Low Pass Filter   │
│       (First)       │    │    Demodulator      │    │       Filter        │    │      (Second)       │
│   Decimation: 4x    │    │    FM Demodulation  │    │   Decimation: 5x    │    │   Decimation: 2x    │
└─────────────────────┘    └─────────────────────┘    └─────────────────────┘    └─────────────────────┘
```

**Total Decimation Factor: 4 × 5 × 2 = 40x**

## 📁 Project Structure

```
Vitis_HLS_FM_Radio/
├── Makefile                    # Top-level Makefile
├── README.md
├── low_pass_filter_first/      # 1st Stage Low Pass Filter (4x Decimation)
│   ├── low_pass_filter_first.cpp
│   ├── low_pass_filter_first.tcl
│   └── Makefile
├── quadrature_demodulator/     # FM Demodulator
│   ├── quadrature_demodulator.cpp
│   ├── quadrature_demodulator.tcl
│   └── Makefile
├── fir_decimation_filter/      # FIR Decimation Filter (5x Decimation)
│   ├── fir_decimation_filter.cpp
│   ├── fir_decimation_filter.tcl
│   └── Makefile
└── low_pass_filter_second/     # 2nd Stage Low Pass Filter (2x Decimation)
    ├── low_pass_filter_second.cpp
    ├── low_pass_filter_second.tcl
    └── Makefile
```

## 🔧 Module Descriptions

### 1. Low Pass Filter First (`low_pass_filter_first`)
- **기능**: 입력 RF 신호의 대역 제한 및 다운샘플링
- **Decimation Factor**: 4x
- **Filter Taps**: 41 taps (Polyphase: 4 phases × 11 taps)
- **Data Type**: `ap_fixed<32, 16>`
- **최적화**: Polyphase Filter 기법 적용

### 2. Quadrature Demodulator (`quadrature_demodulator`)
- **기능**: FM 복조 (위상 변화를 통한 주파수 복원)
- **입력**: Real (I) 및 Imaginary (Q) 복소수 성분
- **출력**: 복조된 오디오 신호
- **Data Type**: 입력 `ap_fixed<16, 8>`, 출력 `ap_fixed<32, 16>`
- **원리**: 연속 샘플 간의 위상 차이를 계산하여 순시 주파수를 추출
  - `result = I[n] × (Q[n] - Q[n-2]) - Q[n] × (I[n] - I[n-2])`

### 3. FIR Decimation Filter (`fir_decimation_filter`)
- **기능**: 다운샘플링 및 안티앨리어싱 필터링
- **Decimation Factor**: 5x
- **Filter Taps**: 25 taps (Polyphase: 5 phases × 5 taps)
- **Data Type**: `ap_fixed<16, 8>`
- **최적화**: Polyphase Filter 기법 적용

### 4. Low Pass Filter Second (`low_pass_filter_second`)
- **기능**: 최종 오디오 대역 필터링 및 다운샘플링
- **Decimation Factor**: 2x
- **Filter Taps**: 41 taps (Polyphase: 2 phases × 21 taps)
- **Data Type**: `ap_fixed<32, 16>`
- **최적화**: Polyphase Filter 기법 적용

## ⚡ Optimization Techniques

### Polyphase Filter Decomposition
모든 Decimation Filter는 **Polyphase Filter** 기법을 사용하여 최적화되어 있습니다.

**기존 방식 vs Polyphase 방식:**
| 구분 | 기존 FIR + Decimation | Polyphase Filter |
|------|----------------------|------------------|
| 연산량 | N taps × 매 입력 샘플 | N/M taps × 매 입력 샘플 |
| 효율성 | 버려지는 연산 존재 | 모든 연산이 유효 |

### HLS Optimization Directives
- `#pragma HLS PIPELINE II=1`: 매 클럭마다 새로운 입력 처리
- `#pragma HLS ARRAY_PARTITION complete`: 계수 및 레지스터 완전 분할로 병렬 접근
- `#pragma HLS UNROLL`: 루프 완전 전개로 병렬 처리
- `#pragma HLS INTERFACE axis`: AXI Stream 인터페이스로 스트리밍 처리

## 🛠️ Build Instructions

### Prerequisites
- Xilinx Vitis HLS 2022.1 이상
- Make

### Build All Modules
```bash
make all
```

### Build Individual Module
```bash
cd <module_directory>
make
```

### Clean Build
```bash
make clean
```

## 📊 Interface Specifications

모든 모듈은 다음 인터페이스를 사용합니다:

| Port | Interface | Description |
|------|-----------|-------------|
| input / real / imag | AXI Stream | 입력 데이터 스트림 |
| output | AXI Stream | 출력 데이터 스트림 |
| return | ap_ctrl_none | Free-running 모드 (제어 신호 없음) |

## 📝 License

This project is open source and available under the MIT License.
