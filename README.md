# 灰港余烬：类 DND 单机文字 MUD 最小 Demo

这是分析文档中 M1“纵向切片”的可运行版本。它只实现一条完整主链路：创建角色、探索三个房间、与一名敌人进行 D20 战斗、使用药水、保存并恢复游戏。

## 构建与测试

```powershell
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

运行：

```powershell
.\build\dndmud_demo.exe
```

## 命令

| 命令 | 作用 |
| --- | --- |
| `help` | 查看帮助 |
| `look` | 查看当前位置 |
| `north` / `east` / `south` / `west` | 移动 |
| `go <方向>` | 移动 |
| `status` | 查看角色状态 |
| `inventory` | 查看背包 |
| `attack` | 攻击当前房间的敌人 |
| `use potion` | 使用治疗药水 |
| `save` / `load` | 保存或读取 `savegame.txt` |
| `quit` | 返回主菜单 |

## 当前边界

- 世界固定为三个房间，只有一名敌人和一种消耗品。
- 战斗使用 D20 命中检定，天然 20 为暴击，天然 1 必定失手。
- 存档是带版本头的文本快照，读取时会校验字段和值域。
- 任务、对话、商店、装备和数据文件加载留到后续里程碑。
