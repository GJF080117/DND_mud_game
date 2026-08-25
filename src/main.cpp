#include "dndmud/Game.h"
#include "dndmud/Random.h"

#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

void configureConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

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
    dndmud::MtRandomSource randomSource;
    const std::filesystem::path savePath = "savegame.txt";

    std::string choice;
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
