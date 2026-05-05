# AGENTS.md — DirectX Tank Game

项目导航枢纽。详细内容请点击链接。

## 快速参考

- **构建**: `msbuild TankGame.sln /p:Configuration=Debug /p:Platform=Win32` → `Tank.exe`
- **VS 2022**: 需加 `/p:PlatformToolset=v143`（vcxproj 中的 v145 不存在）
- **IDE**: Visual Studio 2017+ (v141) 或 2022 (v143, Windows SDK 10.0.15063.0)
- **字符集**: MultiByte（**不要**切换到 Unicode）

## 文档索引

| 文档 | 用途 |
|------|------|
| [.github/copilot-instructions.md](.github/copilot-instructions.md) | 架构、约定、已知陷阱、How-To 模式 |
| [README.md](README.md) | 项目概览、构建步骤、操作按键 |
| [specs/README.md](specs/README.md) | 规范体系与生命周期 |

## 规范目录

| 目录 | 用途 |
|------|------|
| [specs/game-prds/](specs/game-prds/) | 游戏功能 PRD |
| [specs/numerical/](specs/numerical/) | 数值规范与配置表 |
| [specs/product-specs/](specs/product-specs/) | 产品规范 |
| [specs/design-docs/](specs/design-docs/) | 技术设计文档 |
| [specs/exec-plans/](specs/exec-plans/) | 执行计划 |
| [specs/decisions/](specs/decisions/) | 架构决策记录 (ADR) |
| [specs/system/](specs/system/) | 系统级契约 |
| [specs/services/](specs/services/) | 服务级规范 |

## 关键约束

- 已知拼写错误是标识符，**不要修正**：`GameSatting`, `SoundSwicth`, `Backgroud`, `Sound_Rescource`, `Dirction`/`lift`/`below`
- `DirectX.h` ↔ `Global.h` 存在循环引用——不要在它们之间添加类型依赖
- `Now_GAME_STATE`（int 0/1）≠ `GAME_STATE`（场景枚举）
