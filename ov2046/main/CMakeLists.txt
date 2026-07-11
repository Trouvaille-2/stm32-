idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_camera esp_timer nvs_flash esp_wifi esp_event esp_netif esp_http_server lwip)
