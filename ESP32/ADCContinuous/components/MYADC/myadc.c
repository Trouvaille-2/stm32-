#include "myadc.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_continuous.h"

adc_continuous_handle_t adc_handle ;
uint8_t *data_value;
uint16_t adc_value_rl=0;
uint16_t adc_value_rp=0;

bool continuous_callback(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    data_value = edata->conv_frame_buffer;
    if(edata->size == 8)
    {
        adc_value_rl =(data_value[1]& 0x0F)<<8 | data_value[0];
        adc_value_rp =(data_value[5]& 0x0F)<<8 | data_value[4];
        return true;
    }
    return false;
    
}
void adc_init()
{
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,//配置转换结果的最大值
        .conv_frame_size = 8,//配置转换帧的大小
    };
    adc_continuous_new_handle(&adc_config, &adc_handle);

    adc_digi_pattern_config_t adc_digi_arr[] = {
        {
            .atten=ADC_ATTEN_DB_11,
            .bit_width=ADC_BITWIDTH_12,
            .channel=ADC_CHANNEL_3,
            .unit=ADC_UNIT_1,
        },
        {
            .atten=ADC_ATTEN_DB_11,
            .bit_width=ADC_BITWIDTH_12,
            .channel=ADC_CHANNEL_4,
            .unit=ADC_UNIT_1,
        }
    };

    adc_continuous_config_t continuous_config_structure = {
        .adc_pattern =adc_digi_arr,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
        .pattern_num = 2,
        .sample_freq_hz = 20000,
    };
    adc_continuous_config(adc_handle,&continuous_config_structure);

    adc_continuous_evt_cbs_t evt_structure = {
        .on_conv_done = continuous_callback,
    };
    adc_continuous_register_event_callbacks(adc_handle,&evt_structure,NULL);
    adc_continuous_start(adc_handle);
}