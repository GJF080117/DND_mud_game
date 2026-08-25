#include "dndmud/Game.h"

#include <algorithm>
#include <iostream>
#include <utility>

namespace dndmud {
namespace {

std::string displayName(Direction direction) {
    switch (direction) {
    case Direction::North: return "north（北）";
    case Direction::East: return "east（东）";
    case Direction::South: return "south（南）";
    case Direction::West: return "west（西）";
    }
    return {};
}

void printAttack(std::ostream& output, const Character& attacker, const AttackResult& result) {
    output << attacker.name() << "掷出 d20=" << result.d20Roll;
    if (!result.hit) {
        output << "，攻击未命中。\n";
        return;
    }
    output << "，命中并造成 " << result.damage << " 点伤害";
    if (result.critical) {
        output << "（暴击）";
    }
    output << "。\n";
}

} // namespace

Game::Game(IRandomSource& randomSource)
    : world_(World::createDemo()),
      player_("冒险者"),
      combatSystem_(randomSource) {}

void Game::newGame(std::string playerName) {
    if (playerName.empty()) {
        playerName = "冒险者";
    }
    if (playerName.size() > 40) {
        playerName.resize(40);
    }
    world_ = World::createDemo();
    player_ = Player(std::move(playerName));
}

bool Game::load(const std::filesystem::path& savePath, std::ostream& output) {
    std::string error;
    const auto loaded = SaveService::load(savePath, error);
    if (!loaded) {
        output << "读取失败：" << error << '\n';
        return false;
    }
    restore(*loaded);
    output << "已读取存档。\n";
    return true;
}

bool Game::save(const std::filesystem::path& savePath, std::ostream& output) const {
    std::string error;
    if (!SaveService::save(snapshot(), savePath, error)) {
        output << "保存失败：" << error << '\n';
        return false;
    }
    output << "游戏已保存到 " << savePath.string() << "。\n";
    return true;
}

void Game::run(
    std::istream& input,
    std::ostream& output,
    const std::filesystem::path& savePath) {
    output << "\n输入 help 查看命令。\n";
    showRoom(output);

    std::string line;
    bool running = true;
    while (running && player_.isAlive()) {
        output << "\n> ";
        if (!std::getline(input, line)) {
            output << "\n输入结束，返回主菜单。\n";
            return;
        }
        running = executeLine(line, output, savePath);
    }
}

bool Game::executeLine(
    const std::string& line,
    std::ostream& output,
    const std::filesystem::path& savePath) {
    const ParsedCommand command = commandParser_.parse(line);
    switch (command.type) {
    case CommandType::Help: showHelp(output); break;
    case CommandType::Look: showRoom(output); break;
    case CommandType::Move: move(command.argument, output); break;
    case CommandType::Status: showStatus(output); break;
    case CommandType::Inventory: showInventory(output); break;
    case CommandType::Attack: fight(output); break;
    case CommandType::Use: useItem(command.argument, output); break;
    case CommandType::Save: save(savePath, output); break;
    case CommandType::Load: load(savePath, output); break;
    case CommandType::Quit:
        output << "返回主菜单。\n";
        return false;
    case CommandType::Unknown:
        output << "无法识别该命令。输入 help 查看可用命令。\n";
        break;
    }
    return player_.isAlive();
}

const Player& Game::player() const noexcept { return player_; }
const World& Game::world() const noexcept { return world_; }

void Game::showHelp(std::ostream& output) const {
    output << "可用命令：\n"
           << "  look                 查看房间\n"
           << "  north/east/south/west 或 go <方向>\n"
           << "  status               查看状态\n"
           << "  inventory            查看背包\n"
           << "  attack               攻击当前敌人\n"
           << "  use potion           使用治疗药水\n"
           << "  save / load           保存 / 读取\n"
           << "  quit                  返回主菜单\n";
}

void Game::showRoom(std::ostream& output) const {
    const Room& current = world_.room(player_.roomId());
    output << "\n== " << current.title << " ==\n"
           << current.description << '\n';

    if (const Enemy* enemy = world_.enemyInRoom(player_.roomId())) {
        output << "敌人：" << enemy->name() << "（HP " << enemy->health()
               << '/' << enemy->maxHealth() << "）\n";
    } else if (current.enemyId) {
        output << "灰牙狼已经倒下，货箱上的余烬徽记重新亮起。\n";
    }

    output << "出口：";
    bool first = true;
    for (const auto& [direction, destination] : current.exits) {
        static_cast<void>(destination);
        output << (first ? " " : "，") << displayName(direction);
        first = false;
    }
    output << '\n';
}

void Game::showStatus(std::ostream& output) const {
    output << player_.name() << " | HP " << player_.health() << '/'
           << player_.maxHealth() << " | AC " << player_.armorClass()
           << " | 攻击加值 +" << player_.attackBonus() << '\n';
}

void Game::showInventory(std::ostream& output) const {
    output << "背包：治疗药水 x" << player_.potionCount() << '\n';
}

void Game::move(const std::string& argument, std::ostream& output) {
    const auto direction = parseDirection(argument);
    if (!direction) {
        output << "请指定 north、east、south 或 west。\n";
        return;
    }

    const auto destination = world_.destination(player_.roomId(), *direction);
    if (!destination) {
        output << "这个方向没有出口。\n";
        return;
    }

    player_.setRoomId(*destination);
    showRoom(output);
}

void Game::fight(std::ostream& output) {
    Enemy* enemy = world_.enemyInRoom(player_.roomId());
    if (!enemy) {
        output << "这里没有可以攻击的敌人。\n";
        return;
    }

    const AttackResult playerAttack = combatSystem_.attack(player_, *enemy);
    printAttack(output, player_, playerAttack);
    if (playerAttack.targetDefeated) {
        player_.setWon(true);
        output << enemy->name() << "倒下了。你取回余烬徽记，Demo 主线完成。\n";
        return;
    }

    const AttackResult enemyAttack = combatSystem_.attack(*enemy, player_);
    printAttack(output, *enemy, enemyAttack);
    if (enemyAttack.targetDefeated) {
        output << "你失去了意识。本次冒险结束，可从主菜单读取旧存档。\n";
    } else {
        output << "你的 HP：" << player_.health() << '/' << player_.maxHealth()
               << "；敌人 HP：" << enemy->health() << '/' << enemy->maxHealth() << '\n';
    }
}

void Game::useItem(const std::string& argument, std::ostream& output) {
    if (argument != "potion") {
        output << "当前只能使用 potion。\n";
        return;
    }
    if (player_.health() == player_.maxHealth()) {
        output << "生命值已满，无需使用药水。\n";
        return;
    }
    if (!player_.consumePotion()) {
        output << "背包中没有治疗药水。\n";
        return;
    }

    const int healed = player_.heal(8);
    output << "你恢复了 " << healed << " 点生命，当前 HP "
           << player_.health() << '/' << player_.maxHealth() << "。\n";
}

GameSnapshot Game::snapshot() const {
    return GameSnapshot{
        player_.name(),
        player_.health(),
        player_.roomId(),
        player_.potionCount(),
        world_.enemy().health(),
        player_.hasWon()};
}

void Game::restore(const GameSnapshot& snapshot) {
    world_ = World::createDemo();
    player_ = Player(snapshot.playerName);
    player_.setHealth(snapshot.playerHealth);
    player_.setRoomId(snapshot.roomId);
    player_.setPotionCount(snapshot.potionCount);
    player_.setWon(snapshot.won);
    world_.enemy().setHealth(snapshot.enemyHealth);
}

} // namespace dndmud
