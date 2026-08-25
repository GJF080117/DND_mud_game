#include "dndmud/CombatSystem.h"
#include "dndmud/CommandParser.h"
#include "dndmud/Game.h"
#include "dndmud/SaveService.h"
#include "dndmud/World.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

class SequenceRandom final : public dndmud::IRandomSource {
public:
    explicit SequenceRandom(std::vector<int> values) : values_(std::move(values)) {}

    int uniform(int minimum, int maximum) override {
        if (index_ >= values_.size()) {
            throw std::runtime_error("random sequence exhausted");
        }
        const int value = values_[index_++];
        if (value < minimum || value > maximum) {
            throw std::runtime_error("random value outside requested range");
        }
        return value;
    }

private:
    std::vector<int> values_;
    std::size_t index_{0};
};

int failures = 0;

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "FAIL line " << line << ": " << expression << '\n';
        ++failures;
    }
}

#define CHECK(expression) check((expression), #expression, __LINE__)

void testCommandParser() {
    dndmud::CommandParser parser;
    const auto move = parser.parse("  GO East  ");
    CHECK(move.type == dndmud::CommandType::Move);
    CHECK(move.argument == "east");
    CHECK(parser.parse("n").type == dndmud::CommandType::Move);
    CHECK(parser.parse("unknown").type == dndmud::CommandType::Unknown);
}

void testWorld() {
    dndmud::World world = dndmud::World::createDemo();
    CHECK(world.destination(0, dndmud::Direction::North) == 1);
    CHECK(!world.destination(0, dndmud::Direction::West));
    CHECK(world.enemyInRoom(1) == nullptr);
    CHECK(world.enemyInRoom(2) != nullptr);
}

void testCriticalHit() {
    SequenceRandom random({20, 6});
    dndmud::CombatSystem combat(random);
    dndmud::Player player("测试者");
    dndmud::Enemy enemy("target", "木桩", dndmud::CombatStats{10, 30, 0, 0, 4});
    const auto result = combat.attack(player, enemy);
    CHECK(result.hit);
    CHECK(result.critical);
    CHECK(result.damage == 16);
    CHECK(result.targetDefeated);
}

void testSaveRoundTrip() {
    const auto path = std::filesystem::temp_directory_path() / "dndmud_demo_test_save.txt";
    std::error_code ignored;
    std::filesystem::remove(path, ignored);

    const dndmud::GameSnapshot expected{"测试角色", 17, 2, 0, 3, false};
    std::string error;
    CHECK(dndmud::SaveService::save(expected, path, error));
    const auto actual = dndmud::SaveService::load(path, error);
    CHECK(actual.has_value());
    if (actual) {
        CHECK(actual->playerName == expected.playerName);
        CHECK(actual->playerHealth == expected.playerHealth);
        CHECK(actual->roomId == expected.roomId);
        CHECK(actual->potionCount == expected.potionCount);
        CHECK(actual->enemyHealth == expected.enemyHealth);
        CHECK(actual->won == expected.won);
    }

    const dndmud::GameSnapshot updated{"测试角色", 9, 1, 1, 0, true};
    CHECK(dndmud::SaveService::save(updated, path, error));
    const auto replaced = dndmud::SaveService::load(path, error);
    CHECK(replaced.has_value());
    if (replaced) {
        CHECK(replaced->playerHealth == updated.playerHealth);
        CHECK(replaced->roomId == updated.roomId);
        CHECK(replaced->won == updated.won);
    }
    CHECK(!std::filesystem::exists(path.string() + ".bak"));

    std::ofstream(path, std::ios::trunc) << "BROKEN_SAVE\n";
    CHECK(!dndmud::SaveService::load(path, error));
    std::filesystem::remove(path, ignored);
}

void testVerticalSlice() {
    SequenceRandom random({20, 6});
    dndmud::Game game(random);
    game.newGame("演示角色");
    std::ostringstream output;
    const auto path = std::filesystem::temp_directory_path() / "dndmud_demo_flow_save.txt";

    CHECK(game.executeLine("north", output, path));
    CHECK(game.player().roomId() == 1);
    CHECK(game.executeLine("east", output, path));
    CHECK(game.player().roomId() == 2);
    CHECK(game.executeLine("attack", output, path));
    CHECK(game.player().hasWon());
    CHECK(game.world().enemyInRoom(2) == nullptr);
    CHECK(output.str().find("Demo 主线完成") != std::string::npos);
}

} // namespace

int main() {
    testCommandParser();
    testWorld();
    testCriticalHit();
    testSaveRoundTrip();
    testVerticalSlice();

    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cerr << failures << " test(s) failed.\n";
    return 1;
}
