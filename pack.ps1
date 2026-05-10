# pack.ps1 — 编译、整理、打包 DirectX Tank Game
# 用法: .\pack.ps1 [-Config Release] [-Platform x64] [-SkipBuild]

param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release",

    [ValidateSet("Win32", "x64")]
    [string]$Platform = "x64",

    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$ProjectRoot = $PSScriptRoot
$PackDir = Join-Path $ProjectRoot "pack_output"
$ZipName = "TankGame_${Config}_${Platform}.zip"
$ZipPath = Join-Path $ProjectRoot $ZipName

# ---------- 1. 编译 ----------
if (-not $SkipBuild) {
    Write-Host "===== 编译 ($Config|$Platform) =====" -ForegroundColor Cyan

    # 查找 MSBuild
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswhere) {
        $msb = & $vswhere -latest -requires Microsoft.Component.MSBuild `
            -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
    }
    if (-not $msb -or -not (Test-Path $msb)) {
        # 回退：尝试 PATH 中的 msbuild
        $msb = (Get-Command msbuild -ErrorAction SilentlyContinue).Source
    }
    if (-not $msb) {
        Write-Error "找不到 MSBuild，请在 Developer PowerShell 中运行，或安装 Visual Studio Build Tools。"
        exit 1
    }
    Write-Host "使用 MSBuild: $msb"

    $sln = Join-Path $ProjectRoot "TankGame.sln"
    & $msb $sln /p:Configuration=$Config /p:Platform=$Platform /p:PlatformToolset=v143 /m /verbosity:minimal
    if ($LASTEXITCODE -ne 0) {
        Write-Error "编译失败 (exit code $LASTEXITCODE)"
        exit 1
    }
    Write-Host "编译成功！" -ForegroundColor Green
}

# ---------- 2. 定位编译产物 ----------
if ($Platform -eq "x64") {
    $ExeSearchPaths = @(
        (Join-Path $ProjectRoot "x64\$Config\Tank.exe"),
        (Join-Path $ProjectRoot "TankGame\x64\$Config\Tank.exe")
    )
} else {
    $ExeSearchPaths = @(
        (Join-Path $ProjectRoot "$Config\Tank.exe"),
        (Join-Path $ProjectRoot "TankGame\$Config\Tank.exe"),
        (Join-Path $ProjectRoot "TankGame\Tank\$Config\Tank.exe")
    )
}

$ExePath = $ExeSearchPaths | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $ExePath) {
    Write-Error "找不到 Tank.exe，搜索路径:`n$($ExeSearchPaths -join "`n")"
    exit 1
}
Write-Host "找到可执行文件: $ExePath" -ForegroundColor Green

# ---------- 3. 整理输出目录 ----------
Write-Host "===== 整理文件 =====" -ForegroundColor Cyan

if (Test-Path $PackDir) {
    Remove-Item $PackDir -Recurse -Force
}
New-Item $PackDir -ItemType Directory | Out-Null

# 复制 exe
Copy-Item $ExePath $PackDir

# 复制资源目录
$TankGameDir = Join-Path $ProjectRoot "TankGame"
$resourceDirs = @("Resources")
foreach ($dir in $resourceDirs) {
    $src = Join-Path $TankGameDir $dir
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $PackDir $dir) -Recurse
        Write-Host "  复制 $dir"
    }
}

# 复制地图目录
$mapSrc = Join-Path $TankGameDir "Map"
if (Test-Path $mapSrc) {
    $mapDest = Join-Path $PackDir "Map"
    New-Item $mapDest -ItemType Directory | Out-Null
    # 只复制 .map 文件
    Get-ChildItem $mapSrc -Filter "*.map" | ForEach-Object {
        Copy-Item $_.FullName $mapDest
    }
    Write-Host "  复制 Map ($(( Get-ChildItem $mapDest | Measure-Object).Count) 个关卡)"
}

# 复制设置文件（如存在）
$gameSet = Join-Path $TankGameDir "GameSet.set"
if (Test-Path $gameSet) {
    Copy-Item $gameSet $PackDir
    Write-Host "  复制 GameSet.set"
}

# ---------- 4. 打包 ZIP ----------
Write-Host "===== 打包 =====" -ForegroundColor Cyan

if (Test-Path $ZipPath) {
    Remove-Item $ZipPath -Force
}

Compress-Archive -Path "$PackDir\*" -DestinationPath $ZipPath -CompressionLevel Optimal
Write-Host "打包完成: $ZipPath" -ForegroundColor Green

# 输出文件大小
$zipSize = (Get-Item $ZipPath).Length
if ($zipSize -gt 1MB) {
    Write-Host ("文件大小: {0:N1} MB" -f ($zipSize / 1MB))
} else {
    Write-Host ("文件大小: {0:N0} KB" -f ($zipSize / 1KB))
}

# ---------- 5. 清理临时目录 ----------
Remove-Item $PackDir -Recurse -Force
Write-Host "清理临时目录完成。" -ForegroundColor DarkGray

Write-Host "`n===== 全部完成 =====" -ForegroundColor Green
