#include "wifiap.h"
#include "esp_wifi.h"
#include "string.h"
#include "lcd.h"
#include "esp_mac.h"

void wifista_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        printf("station "MACSTR" join, AID=%d\n", MAC2STR(event->mac), event->aid);
        lcd_show_string(0, 0, "Station connected", WHITE, BLACK);
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        printf("station "MACSTR" leave, AID=%d\n", MAC2STR(event->mac), event->aid);
        lcd_show_string(0, 0, "Station disconnected", WHITE, BLACK);
    }
}

void wifi_init_ap(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifista_event_handler, NULL);
   
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_AP); 

    wifi_config_t wifiap_config = {
        .ap = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD,
            .ssid_len = strlen(DEFAULT_SSID),
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = 2,
        },
    };
    esp_wifi_set_config(ESP_IF_WIFI_AP,&wifiap_config);

    esp_wifi_start();

    
}