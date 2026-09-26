#pragma once

// =======================
// MAX31865 - SPI
// =======================
#define MAX31865_CS    10
#define MAX31865_MOSI  11
#define MAX31865_SCK   12
#define MAX31865_MISO  13

// =======================
// Analog: pH + độ mặn (EC)
// GPIO5: DFR0300 / DFR0300-H analog EC — không dùng SEN0244 TDS nước ngọt.
// =======================
#define PH_PIN          4
#define SALINITY_PIN    5
#define TDS_PIN         SALINITY_PIN
