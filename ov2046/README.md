# ESP32-S3 CAM + OV2640 WiFi 图传

在 **ESP32-S3 CAM** 开发板上实现 **OV2640 摄像头 WiFi 图传**，浏览器实时查看画面。

## 📋 硬件

| 组件 | 型号 |
|------|------|
| 主控 | ESP32-S3（需 PSRAM） |
| 摄像头 | OV2640（支持 UXGA 1600×1200） |

> ⚠️ 如果使用不同引脚映射的板型，请修改 `main/main.c` 中的 `camera_config_t`。

## 🚀 快速开始

### 1. 配置 WiFi

打开 `main/main.c`，修改顶部：

```c
#define WIFI_SSID       "你的WiFi名称"     // ← 改成你的 WiFi
#define WIFI_PASSWORD   "你的WiFi密码"     // ← 改成你的密码
```

### 2. 编译烧录

```bash
idf.py build
idf.py -p COM端口 flash monitor
```

### 3. 浏览器打开

烧录完成后，在串口输出中会看到 ESP32 的 IP 地址，例如：

```
I (1234) OV2640_CAM: =================================
I (1235) OV2640_CAM: 请在浏览器中打开:
I (1236) OV2640_CAM:   http://192.168.1.100
I (1237) OV2640_CAM: =================================
```

在电脑或手机的浏览器中输入此 IP 即可看到摄像头画面。

## 🌐 HTTP 接口

| 路径 | 说明 |
|------|------|
| `/` | 视频流网页（含控制按钮） |
| `/stream` | MJPEG 视频流 |
| `/capture` | 捕获单张 JPEG 照片 |
| `/status` | 状态信息 JSON |
| `/res?w=640&h=480` | 切换分辨率 |

### 分辨率选项

| 参数 | 分辨率 |
|------|--------|
| `w=160&h=120` | QQVGA |
| `w=320&h=240` | QVGA |
| `w=640&h=480` | **VGA（默认）** |
| `w=800&h=600` | SVGA |
| `w=1024&h=768` | XGA |
| `w=1600&h=1200` | UXGA |

## 📁 项目结构

```
ov2046/
├── CMakeLists.txt
├── sdkconfig.defaults
├── .gitignore
├── README.md
├── components/
│   ├── .gitkeep
│   └── esp_camera/          # esp32-camera 组件
└── main/
    ├── CMakeLists.txt
    └── main.c               # WiFi 图传主程序
```

## 🔧 常见问题

### 画面卡顿 / 帧率低
- 流模式默认是 VGA (640×480)，可通过网页切换更低分辨率
- 检查 WiFi 信号强度
- 默认 XCLK 20MHz，可尝试降到 10MHz

### 摄像头初始化失败
- 检查引脚映射是否正确
- 确认 PSRAM 已启用（烧录时已配置 `sdkconfig.defaults`）

### 连不上 WiFi
- 检查 SSID 和密码是否正确
- 确保 WiFi 是 2.4GHz（ESP32-S3 不支持 5GHz）
