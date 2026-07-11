#include "spi_sdcard.h"
#include "esp_vfs_fat.h"
//spi操作tf卡

sdmmc_card_t* sdcard;


esp_err_t sd_spi_init(void)
{
    esp_err_t ret;

    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();

     sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.host_id = SPI2_HOST; //使用SPI2
    slot_config.gpio_cs = GPIO_NUM_2; //片选引脚

    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false, 
        .max_files = 5, 
        .allocation_unit_size = 0, 
        .disk_status_check_enable = false, 
    };

    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &sdcard);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI_SDCARD", "Failed to mount SD card");
        return ret;
    }
    return ESP_OK;
}