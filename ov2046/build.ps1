$ErrorActionPreference = "Stop"
$projectDir = "d:\Desktop\code\32\ov2046"

Set-Location $projectDir

# Setup environment
$env:IDF_PATH = "D:\ESP\.espressif\v5.4.4\esp-idf"
$env:IDF_PYTHON_ENV_PATH = "C:\Espressif\tools\python\v5.4.4\venv"
$env:Path = "C:\Espressif\tools\python\v5.4.4\venv\Scripts;$env:Path"

Write-Host "=== 设置目标芯片 esp32s3 ==="
python D:\ESP\.espressif\v5.4.4\esp-idf\tools\idf.py set-target esp32s3
if ($LASTEXITCODE -ne 0) { throw "set-target 失败" }

Write-Host "=== 编译 ==="
python D:\ESP\.espressif\v5.4.4\esp-idf\tools\idf.py build
if ($LASTEXITCODE -ne 0) { throw "build 失败" }

Write-Host "=== 编译成功! ==="
