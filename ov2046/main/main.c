/**
 * @file main.c
 * @brief ESP32-S3 CAM + OV2640 WiFi 图传
 *
 * 功能：
 *  - 初始化 OV2640 摄像头
 *  - 连接 WiFi
 *  - 启动 HTTP 服务器，在浏览器中查看摄像头画面
 *
 * 访问:
 *  http://esp-cam-ip/         — 视频流页面
 *  http://esp-cam-ip/stream   — MJPEG 视频流
 *  http://esp-cam-ip/capture  — 单张 JPEG 照片
 *  http://esp-cam-ip/status   — 状态信息 (JSON)
 *  http://esp-cam-ip/res?w=320&h=240  — 切换分辨率
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "sensor.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_heap_caps.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

/* ======================== WiFi 配置 ========================
 * 模式选择:
 *   0 = STA 模式（连路由器/手机热点）
 *   1 = AP 模式（ESP32 自建热点，电脑/手机连它）
 * ========================================================== */
#define WIFI_MODE_AP    0   // ← 改成 0 就是 STA 模式

/* ---- STA 模式参数（WIFI_MODE_AP=0 时有效） ---- */
#define WIFI_SSID       "HONOR 500 Pro"
#define WIFI_PASSWORD   "cyb123456.789"
#define WIFI_MAX_RETRY  5

/* ---- AP 模式参数（WIFI_MODE_AP=1 时有效） ---- */
#define AP_SSID         "ESP32-CAM"
#define AP_PASSWORD     "12345678"
#define AP_MAX_CONNECT  4

/* ======================== HTTP 服务器 ======================== */
#define STREAM_ALLOWED_CONNECTIONS 4
#define STREAM_PART_BOUNDARY  "123456789000000000000987654321"
#define STREAM_CONTENT_TYPE  "multipart/x-mixed-replace;boundary=" STREAM_PART_BOUNDARY

static const char *TAG = "OV2640_CAM";

/* ======================== ESP32-S3 CAM (N16R8) 引脚定义 ========================
 * 适用于 GOOUUU / 通用 ESP32-S3 CAM 开发板（带 N16R8 模组）
 * 参考: https://github.com/profharris/GOOUUU_ESP32-S3-CAM
 * ===================================================================== */
static const camera_config_t camera_config = {
    .pin_pwdn     = -1,
    .pin_reset    = -1,
    .pin_xclk     = 15,
    .pin_sscb_sda = 4,
    .pin_sscb_scl = 5,

    .pin_d7       = 16,   // Y9
    .pin_d6       = 17,   // Y8
    .pin_d5       = 18,   // Y7
    .pin_d4       = 12,   // Y6  ← 更正
    .pin_d3       = 10,   // Y5  ← 更正
    .pin_d2       = 8,    // Y4  ← 更正
    .pin_d1       = 9,    // Y3  ← 更正
    .pin_d0       = 11,   // Y2  ← 更正
    .pin_vsync    = 6,
    .pin_href     = 7,
    .pin_pclk     = 13,   // ← 更正

    .xclk_freq_hz = 20000000,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size   = FRAMESIZE_VGA,      // 640×480
    .jpeg_quality = 10,                 // 画质降低一档换速度
    .fb_count     = 3,                  // 3 个缓冲区，流水线更流畅
    .fb_location  = CAMERA_FB_IN_PSRAM,
    .grab_mode    = CAMERA_GRAB_WHEN_EMPTY,
};

/* ======================== WiFi 初始化（AP / STA 双模式） ======================== */
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (WIFI_MODE_AP) {
        /* AP 模式：无需处理连接事件 */
        return;
    }

    /* STA 模式事件处理 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "重连 WiFi... (%d/%d)", s_retry_num, WIFI_MAX_RETRY);
        } else {
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            ESP_LOGE(TAG, "WiFi 连接失败");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "WiFi 已连接, IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if (WIFI_MODE_AP) {
        /* ==================== AP 模式 ==================== */
        esp_netif_t *netif = esp_netif_create_default_wifi_ap();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                            ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

        wifi_config_t wifi_config = {
            .ap = {
                .ssid = AP_SSID,
                .ssid_len = strlen(AP_SSID),
                .password = AP_PASSWORD,
                .max_connection = AP_MAX_CONNECT,
                .authmode = WIFI_AUTH_WPA2_PSK,
            },
        };
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        ESP_LOGI(TAG, "AP 模式已启动, WiFi: %s  密码: %s", AP_SSID, AP_PASSWORD);
        ESP_LOGI(TAG, "请用电脑/手机连接此 WiFi，然后打开浏览器访问 http://192.168.4.1");
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

    } else {
        /* ==================== STA 模式 ==================== */
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                            ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                            IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

        wifi_config_t wifi_config = {
            .sta = {
                .ssid = WIFI_SSID,
                .password = WIFI_PASSWORD,
                .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            },
        };
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        ESP_LOGI(TAG, "正在连接 WiFi: %s ...", WIFI_SSID);
    }
}

/* ======================== 摄像头初始化和传感器信息 ======================== */
static esp_err_t init_camera(void)
{
    ESP_LOGI(TAG, "正在初始化摄像头 ...");

    /* 检测 PSRAM 是否可用 */
    size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    bool psram_ok = (psram_size > 0);
    ESP_LOGI(TAG, "PSRAM 大小: %u bytes %s", psram_size, psram_ok ? "✅" : "❌");

    camera_config_t cfg = camera_config;
    if (!psram_ok) {
        ESP_LOGW(TAG, "PSRAM 不可用，使用 DRAM 模式（QQVGA）");
        cfg.fb_location = CAMERA_FB_IN_DRAM;
        cfg.frame_size = FRAMESIZE_QQVGA;  // 160×120（大幅降低分辨率）
        cfg.jpeg_quality = 8;              // 较低画质进一步减少数据量
        cfg.fb_count = 1;
        cfg.xclk_freq_hz = 20000000;       // 保持 20MHz
        cfg.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    }

    esp_err_t ret = esp_camera_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "摄像头初始化失败: 0x%x", ret);
        return ret;
    }

    /* 配置传感器参数（优化帧率） */
    sensor_t *sensor = esp_camera_sensor_get();
    if (sensor) {
        sensor->set_quality(sensor, 10);         // JPEG 质量（越小越快）
        sensor->set_brightness(sensor, 0);       // 默认亮度
        sensor->set_saturation(sensor, 0);       // 默认饱和度
        sensor->set_ae_level(sensor, 0);         // 自动曝光
        sensor->set_agc_gain(sensor, 0);         // 自动增益
        sensor->set_vflip(sensor, 0);            // 不翻转
        sensor->set_hmirror(sensor, 0);          // 不镜像
    }

    ESP_LOGI(TAG, "摄像头初始化成功");
    return ESP_OK;
}

static void print_sensor_info(void)
{
    sensor_t *sensor = esp_camera_sensor_get();
    if (sensor == NULL) return;

    camera_sensor_info_t *info = esp_camera_sensor_get_info(&sensor->id);
    framesize_t fs = sensor->status.framesize;

    ESP_LOGI(TAG, "========== 传感器信息 ==========");
    ESP_LOGI(TAG, "型号       : %s", info ? info->name : "未知");
    ESP_LOGI(TAG, "PID        : 0x%x", sensor->id.PID);
    ESP_LOGI(TAG, "版本       : %d", sensor->id.VER);
    ESP_LOGI(TAG, "MID        : 0x%02x 0x%02x", sensor->id.MIDH, sensor->id.MIDL);
    ESP_LOGI(TAG, "当前分辨率 : %dx%d", resolution[fs].width, resolution[fs].height);
    ESP_LOGI(TAG, "当前格式   : %s", sensor->pixformat == PIXFORMAT_JPEG ? "JPEG" : "其他");
    ESP_LOGI(TAG, "=================================");
}

/* ======================== HTTP 请求处理 ======================== */

/** 替换 HTML 中的占位符 */
static esp_err_t html_response(httpd_req_t *req, const char *html)
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_sendstr(req, html);
    return ESP_OK;
}

/**
 * GET /
 * 返回视频流网页
 */
static const char web_page[] =
    "<!DOCTYPE html><html><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP32-S3 CAM</title>"
    "<style>"
    "*{margin:0;padding:0;box-sizing:border-box;font-family:system-ui,sans-serif}"
    "body{background:#111;color:#fff;min-height:100vh;display:flex;flex-direction:column;align-items:center}"
    "h1{margin:16px 0 8px;font-size:1.2rem;color:#0af}"
    "#stream{width:100%%;max-width:800px;border:2px solid #333;border-radius:8px;background:#000}"
    "canvas{width:100%%;max-width:800px;display:none}"
    ".bar{display:flex;flex-wrap:wrap;gap:8px;padding:12px;justify-content:center}"
    "button{background:#333;color:#fff;border:1px solid #555;padding:8px 16px;border-radius:6px;cursor:pointer;font-size:14px}"
    "button:hover{background:#555}"
    "button.active{background:#0af;color:#000;border-color:#0af}"
    "#info{color:#888;font-size:13px;padding:8px}"
    ".res-grid{display:flex;flex-wrap:wrap;gap:4px;justify-content:center;padding:4px 12px 12px}"
    ".res-grid button{font-size:12px;padding:4px 10px}"
    "</style></head><body>"
    "<h1>📷 ESP32-S3 CAM + OV2640</h1>"
    "<img id='stream' src='/stream'>"
    "<canvas id='canvas'></canvas>"
    "<div class='bar'>"
    "<button onclick='toggleStream()'>⏸ 暂停</button>"
    "<button onclick='capture()'>📸 VGA</button>"
    "<button onclick='capture(\"uxga\")'>📸 UXGA</button>"
    "<button onclick='location.reload()'>🔄 刷新</button>"
    "</div>"
    "<div style='color:#888;font-size:12px;padding:0 12px'>流分辨率（拍照不受影响）</div>"
    "<div class='res-grid'>"
    "<button onclick='setRes(160,120)'>160×120</button>"
    "<button onclick='setRes(320,240)'>320×240</button>"
    "<button onclick='setRes(640,480)'>640×480</button>"
    "<button onclick='setRes(800,600)'>800×600</button>"
    "<button onclick='setRes(1024,768)'>1024×768</button>"
    "<button onclick='setRes(1600,1200)'>1600×1200</button>"
    "</div>"
    "<div id='info'>加载中...</div>"
    "<script>"
    "let paused=false;const img=document.getElementById('stream');"
    "function toggleStream(){paused=!paused;"
    "if(paused){img.src='';this.textContent='▶ 继续'}"
    "else{img.src='/stream';this.textContent='⏸ 暂停'}}"
    "function capture(res){"
    "const url=res?'/capture?r='+res:'/capture';"
    "const a=document.createElement('a');a.href=url;a.download='';"
    "document.body.appendChild(a);a.click();document.body.removeChild(a)}"
    "function setRes(w,h){"
    "fetch('/res?w='+w+'&h='+h).then(r=>r.json()).then(d=>{"
    "document.getElementById('info').textContent=d.msg;"
    "setTimeout(()=>{if(!paused)img.src='/stream'},500)})}"
    "img.onload=function(){fetch('/status').then(r=>r.json()).then(d=>{"
    "document.getElementById('info').textContent=d.ip+' | '+d.resolution})}"
    "</script></body></html>";

static esp_err_t index_handler(httpd_req_t *req)
{
    return html_response(req, web_page);
}

/**
 * GET /status
 * 返回 JSON 状态
 */
static esp_err_t status_handler(httpd_req_t *req)
{
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t fs = sensor ? sensor->status.framesize : FRAMESIZE_INVALID;

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"ip\":\"%s\",\"resolution\":\"%dx%d\",\"fps\":\"--\",\"framesize\":%d}",
        "",  // IP 可以在 html 里不显示
        (fs < FRAMESIZE_INVALID) ? resolution[fs].width : 0,
        (fs < FRAMESIZE_INVALID) ? resolution[fs].height : 0,
        fs);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, buf);
    return ESP_OK;
}

/**
 * GET /capture
 * GET /capture?r=uxga   — 指定分辨率（qqvga/qvga/vga/svga/xga/uxga）
 * 返回单张 JPEG 照片（自动下载，带时间戳文件名）
 */
static esp_err_t capture_handler(httpd_req_t *req)
{
    sensor_t *sensor = esp_camera_sensor_get();

    /* 解析分辨率参数 */
    char buf[16] = {0};
    framesize_t target_fs = FRAMESIZE_INVALID;
    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
        char r[8] = {0};
        httpd_query_key_value(buf, "r", r, sizeof(r));
        if      (strcasecmp(r, "qqvga") == 0) target_fs = FRAMESIZE_QQVGA;
        else if (strcasecmp(r, "qvga")  == 0) target_fs = FRAMESIZE_QVGA;
        else if (strcasecmp(r, "vga")   == 0) target_fs = FRAMESIZE_VGA;
        else if (strcasecmp(r, "svga")  == 0) target_fs = FRAMESIZE_SVGA;
        else if (strcasecmp(r, "xga")   == 0) target_fs = FRAMESIZE_XGA;
        else if (strcasecmp(r, "uxga")  == 0) target_fs = FRAMESIZE_UXGA;
    }

    /* 切换到目标分辨率（默认 UXGA 高质量拍照） */
    framesize_t old_fs = FRAMESIZE_INVALID;
    if (sensor) {
        old_fs = sensor->status.framesize;
        framesize_t cap_fs = (target_fs != FRAMESIZE_INVALID) ? target_fs : FRAMESIZE_UXGA;
        if (old_fs != cap_fs) {
            sensor->set_framesize(sensor, cap_fs);
            sensor->set_quality(sensor, 10);
            vTaskDelay(pdMS_TO_TICKS(200));  // 等待传感器稳定
        }
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    /* 文件名带时间戳，方便数据集整理 */
    int64_t now_us = esp_timer_get_time();
    unsigned long secs = (unsigned long)(now_us / 1000000ULL);
    int h = (secs / 3600) % 24;
    int m = (secs / 60) % 60;
    int s = (int)(secs % 60);

    char filename[64];
    snprintf(filename, sizeof(filename),
        "attachment; filename=ESP32CAM_%02d%02d%02d_%dx%d.jpg",
        h, m, s, fb->width, fb->height);

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", filename);
    esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);

    /* 恢复流分辨率 */
    if (sensor && old_fs != FRAMESIZE_INVALID && old_fs != target_fs) {
        sensor->set_framesize(sensor, old_fs);
    }

    return res;
}

/**
 * GET /stream
 * MJPEG 视频流
 */
static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    char part_buf[128];

    httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store, must-revalidate");
    httpd_resp_set_hdr(req, "Pragma", "no-cache");
    httpd_resp_set_hdr(req, "Expires", "0");

    /* 分辨率已在 init 时设置好，流中不再重复设置 */
    while (1) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "摄像头捕获失败");
            continue;
        }

        /* 写入 MJPEG 帧边界 */
        size_t hlen = snprintf(part_buf, sizeof(part_buf),
            "--" STREAM_PART_BOUNDARY "\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length: %lu\r\n\r\n", (unsigned long)fb->len);

        res = httpd_resp_send_chunk(req, part_buf, hlen);
        if (res != ESP_OK) break;

        res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        if (res != ESP_OK) break;

        /* 写入帧尾 */
        res = httpd_resp_send_chunk(req, "\r\n", 2);
        if (res != ESP_OK) break;

        esp_camera_fb_return(fb);
        fb = NULL;
    }

    if (fb) esp_camera_fb_return(fb);
    return res;
}

/**
 * GET /res?w=640&h=480
 * 切换分辨率
 */
static esp_err_t resolution_handler(httpd_req_t *req)
{
    char buf[64], w_str[8], h_str[8];
    int w = 640, h = 480;

    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
        httpd_query_key_value(buf, "w", w_str, sizeof(w_str));
        httpd_query_key_value(buf, "h", h_str, sizeof(h_str));
        w = atoi(w_str);
        h = atoi(h_str);
    }

    /* 基于宽高查找最接近的 framesize */
    framesize_t fs = FRAMESIZE_VGA;
    for (int i = 0; i < FRAMESIZE_INVALID; i++) {
        if (resolution[i].width == w && resolution[i].height == h) {
            fs = (framesize_t)i;
            break;
        }
    }

    sensor_t *sensor = esp_camera_sensor_get();
    if (sensor) sensor->set_framesize(sensor, fs);

    char json[96];
    snprintf(json, sizeof(json),
        "{\"status\":\"ok\",\"resolution\":\"%dx%d\",\"msg\":\"切换到 %dx%d\"}",
        resolution[fs].width, resolution[fs].height,
        resolution[fs].width, resolution[fs].height);

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_sendstr(req, json);
    return ESP_OK;
}

/**
 * 启动 HTTP 服务器
 */
static void start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;
    config.lru_purge_enable = true;
    config.stack_size = 8192;

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "HTTP 服务器启动失败");
        return;
    }

    httpd_uri_t uri[] = {
        { .uri = "/",          .method = HTTP_GET, .handler = index_handler },
        { .uri = "/stream",    .method = HTTP_GET, .handler = stream_handler },
        { .uri = "/capture",   .method = HTTP_GET, .handler = capture_handler },
        { .uri = "/status",    .method = HTTP_GET, .handler = status_handler },
        { .uri = "/res",       .method = HTTP_GET, .handler = resolution_handler },
    };

    for (int i = 0; i < sizeof(uri) / sizeof(uri[0]); i++) {
        httpd_register_uri_handler(server, &uri[i]);
    }

    /* 注册 404 处理器 */
    httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, NULL);

    ESP_LOGI(TAG, "HTTP 服务器已启动");
}

/* ======================== 主入口 ======================== */
void app_main(void)
{
    ESP_LOGI(TAG, "=================================");
    ESP_LOGI(TAG, "ESP32-S3 CAM + OV2640 WiFi 图传");
    ESP_LOGI(TAG, "=================================");

    /* 初始化 NVS（WiFi 需要） */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* 初始化摄像头 */
    if (init_camera() != ESP_OK) {
        ESP_LOGE(TAG, "摄像头初始化失败，系统暂停");
        vTaskDelay(pdMS_TO_TICKS(1000));
        return;
    }
    print_sensor_info();

    /* 连接 WiFi（AP 或 STA 模式） */
    wifi_init();

    /* 等待 WiFi 就绪 */
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        pdFALSE, pdTRUE, portMAX_DELAY);

    /* 启动 Web 服务器 */
    start_webserver();

    /* 打印 IP 地址 */
    if (WIFI_MODE_AP) {
        ESP_LOGI(TAG, "=================================");
        ESP_LOGI(TAG, "  电脑/手机连接 WiFi: %s", AP_SSID);
        ESP_LOGI(TAG, "  浏览器打开: http://192.168.4.1");
        ESP_LOGI(TAG, "=================================");
    } else {
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif) {
            esp_netif_ip_info_t ip_info;
            esp_netif_get_ip_info(netif, &ip_info);
            ESP_LOGI(TAG, "=================================");
            ESP_LOGI(TAG, "请在浏览器中打开:");
            ESP_LOGI(TAG, "  http://" IPSTR, IP2STR(&ip_info.ip));
            ESP_LOGI(TAG, "=================================");
        }
    }

    /* 主循环 */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
