#ifndef __ADC_H__
#define __ADC_H__

#include "esp_adc/adc_oneshot.h"

extern adc_oneshot_unit_handle_t adc_handle;

void adc_init(void);

#endif