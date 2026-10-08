#include "main.h"

typedef enum
{
    MODE_SENSOR_CALIB,
    MODE_DFU,
    MODE_USB_WEBCAM,
    MODE_TEST_ADC,
    MODE_TEST_MOTOR,
    MODE_LINE_SENSOR,
    MODE_LINE_CAMERA,
    MODE_NORMAL
} mode_t;

typedef struct
{
    void (*init)(void);
    void (*run)(void);
    void (*stop)(void);
} mode_api_t;