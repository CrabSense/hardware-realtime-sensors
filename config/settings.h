#pragma once

#define FIRMWARE_VERSION "1.0.2"
#define CONTROLLER_TYPE "realtime_sensor"

#define WIFI_CONNECT_TIMEOUT_MS 25000
#define SENSOR_PUBLISH_INTERVAL_MS 15000

#define PROVISION_HTTP_PORT 80
#define DEFAULT_BACKEND_URL "http://10.0.0.1:5080"

#define SENSOR_CODE_TEMP_SUFFIX "-temp"
#define SENSOR_CODE_PH_SUFFIX "-ph"
#define SENSOR_CODE_TDS_SUFFIX "-tds"

// Analog pH 5V, dốc 180 mV/pH. One-point: Serial ổn định ~2514 mV khi máy đo tay 7.45.
#define PH_CAL_MV     2514.0f
#define PH_CAL_PH     7.45f
#define PH_MV_PER_PH  180.0f

// Analog EC (DFRobot DFR0300 / DFR0300-H) → độ mặn ppt.
// K=1 (DFR0300): 1–20 mS/cm. K=10 (DFR0300-H): 10–100 mS/cm — đúng nước mặn ~17 ppt.
#define EC_KVALUE     1.0f
#define EC_RES2       820.0f
#define EC_REF        200.0f
#define EC_TO_PPT     0.64f
#define EC_SAT_MV     3000.0f
