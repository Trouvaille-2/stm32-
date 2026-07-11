@echo off
chcp 65001 >nul
echo ============================================
echo  ESP32-S3 CAM + OV2640 工程设置脚本
echo ============================================
echo.

:: 检查 git
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未找到 git，请先安装 Git
    pause
    exit /b 1
)

:: 添加 esp32-camera 子模块
if not exist "components\esp32-camera" (
    echo [1/2] 添加 esp32-camera 组件 (git submodule)...
    git submodule add https://github.com/espressif/esp32-camera.git components/esp32-camera
    if %errorlevel% neq 0 (
        echo [错误] submodule 添加失败
        echo 请手动执行: git clone https://github.com/espressif/esp32-camera.git components/esp32-camera
    )
) else (
    echo [1/2] esp32-camera 组件已存在，跳过
)

echo [2/2] 设置完成！
echo.
echo 下一步操作：
echo   1. 打开 ESP-IDF 命令行
echo   2. cd /d %CD%
echo   3. idf.py set-target esp32s3
echo   4. idf.py build
echo   5. idf.py -p COM端口 flash monitor
echo.
pause
