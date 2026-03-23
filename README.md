[English](#tank-battle) | [中文](#坦克大战)

---

# Tank Battle

A classic tank battle game built with DirectX 9 and C++, inspired by the NES Battle City.

## Features

- **Single Player** — Battle through 6 built-in stages against AI-controlled enemies
- **Two Player** — Local co-op on the same keyboard with customizable key bindings
- **Map Editor** — Design and save custom 13×13 tile maps
- **Settings** — Rebind controls for both players, toggle sound on/off

## Screenshots

<!-- Add gameplay screenshots here -->

## Prerequisites

- Windows 10+
- [Visual Studio 2017+](https://visualstudio.microsoft.com/) (Desktop development with C++ workload)
- [DirectX SDK June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## Build

Open `TankGame.sln` in Visual Studio, or from a Developer Command Prompt:

```
msbuild TankGame.sln /p:Configuration=Debug /p:Platform=Win32
```

Output: `Tank.exe`

## Controls

| Action | Player 1 | Player 2 |
|--------|----------|----------|
| Move Up | ↑ | W |
| Move Down | ↓ | S |
| Move Left | ← | A |
| Move Right | → | D |
| Shoot | X | J |

Controls can be remapped in the Settings menu.

## Project Structure

```
TankGame/          — Source code and game resources
├── WinMain.cpp    — Entry point and window setup
├── GameMain.cpp   — Game loop, scene management
├── GamingScene.*  — Core gameplay logic
├── DirectX.*      — DirectX 9 rendering and input wrapper
├── DirectSound.*  — Audio system
├── Resources/     — Textures, sounds, cursors
└── Map/           — Level files (.map, 13×13 binary grids)
```

## License

<!-- Add your license here -->

---

# 坦克大战

使用 DirectX 9 和 C++ 开发的经典坦克大战游戏，灵感来源于 FC 坦克大战。

## 功能特性

- **单人模式** — 通过 6 个内置关卡与 AI 敌人战斗
- **双人模式** — 同一键盘本地合作，支持自定义按键绑定
- **地图编辑器** — 设计并保存自定义 13×13 瓦片地图
- **游戏设定** — 为两位玩家重新绑定操作键，开关音效

## 截图

<!-- 在此添加游戏截图 -->

## 环境要求

- Windows 10+
- [Visual Studio 2017+](https://visualstudio.microsoft.com/)（需安装"使用 C++ 的桌面开发"工作负载）
- [DirectX SDK June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## 构建

在 Visual Studio 中打开 `TankGame.sln`，或在开发者命令提示符中执行：

```
msbuild TankGame.sln /p:Configuration=Debug /p:Platform=Win32
```

输出：`Tank.exe`

## 操作按键

| 操作 | 玩家一 | 玩家二 |
|------|--------|--------|
| 上移 | ↑ | W |
| 下移 | ↓ | S |
| 左移 | ← | A |
| 右移 | → | D |
| 射击 | X | J |

可在"游戏设定"菜单中重新映射按键。

## 项目结构

```
TankGame/          — 源代码与游戏资源
├── WinMain.cpp    — 入口点与窗口设置
├── GameMain.cpp   — 游戏循环、场景管理
├── GamingScene.*  — 核心游戏逻辑
├── DirectX.*      — DirectX 9 渲染与输入封装
├── DirectSound.*  — 音频系统
├── Resources/     — 贴图、音效、光标
└── Map/           — 关卡文件（.map，13×13 二进制网格）
```

## 许可证

<!-- 在此添加许可证 -->
