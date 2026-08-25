#include "dndmud/Game.h"
#include "dndmud/Random.h"

#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

// Windows 控制台默认代码页可能不是 UTF-8，需要显式设置中文输入输出编码。
void configureConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

// 主菜单属于程序外壳，不进入 Game 的游戏内命令系统。
void showTitle() {
    std::cout << "\n================================\n"
        << "  灰港余烬 - 类 DND 文字 MUD Demo\n"
        << "================================\n"
        << "1. 新游戏\n"
        << "2. 读取存档\n"
        << "3. 退出\n"
        << "选择：";
}

} // namespace

int main() {
    configureConsole();

    // 随机数工具在程序结束前一直存在，所有游戏都可以安全使用它。
    dndmud::MtRandomSource randomSource;
    const std::filesystem::path savePath = "savegame.txt";

    std::string choice;
    // 每次返回主菜单都创建新的 Game；读取存档会随后恢复其状态。
    while (true) {
        showTitle();
        if (!std::getline(std::cin, choice)) {
            break;
        }

        dndmud::Game game(randomSource);
        if (choice == "1") {
            std::cout << "角色名：";
            std::string name;
            if (!std::getline(std::cin, name)) {
                break;
            }
            game.newGame(name);
            game.run(std::cin, std::cout, savePath);
        } else if (choice == "2") {
            if (game.load(savePath, std::cout)) {
                game.run(std::cin, std::cout, savePath);
            }
        } else if (choice == "3") {
            std::cout << "再会。\n";
            break;
        } else {
            std::cout << "请输入 1、2 或 3。\n";
        }
    }
    return 0;
}
