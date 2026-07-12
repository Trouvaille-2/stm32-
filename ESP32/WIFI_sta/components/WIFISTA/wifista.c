#include "wifista.h"
#include "esp_wifi.h"
#include "lcd.h"

void wifista_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        if(event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            lcd_show_string(1,1,"Wi-Fi disconnected",YELLOW,BLACK);
        }
        else if(event_id == WIFI_EVENT_STA_CONNECTED)
        {
            lcd_show_string(1,1,"Wi-Fi connected",GREEN,BLACK);
        }
    }

    if(event_base == IP_EVENT)
    {
        if(event_id == IP_EVENT_STA_GOT_IP)
        {
            esp_netif_ip_info_t *event = (esp_netif_ip_info_t *)event_data;
            lcd_show_num(2,1,esp_ip4_addr1_16(&event->ip),3,GREEN,BLACK);
            lcd_show_string(2,5,".",GREEN,BLACK);
            lcd_show_num(2,6,esp_ip4_addr2_16(&event->ip),3,GREEN,BLACK);
            lcd_show_string(2,10,".",GREEN,BLACK);
            lcd_show_num(2,11,esp_ip4_addr3_16(&event->ip),3,GREEN,BLACK);
            lcd_show_string(2,15,".",GREEN,BLACK);
            lcd_show_num(2,16,esp_ip4_addr4_16(&event->ip),3,GREEN,BLACK);

        }
    }
}

void wifi_init_sta(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifista_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifista_event_handler, NULL);
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_STA); 

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFULT_SSID,
            .password = DEFAULT_PWD,
        },
    };
    esp_wifi_set_config(WIFI_IF_STA,&wifi_config);

    esp_wifi_start();
}