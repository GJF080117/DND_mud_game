#pragma once

#include <cstdint>
#include <random>

namespace dndmud {

class IRandomSource {
public:
    virtual ~IRandomSource() = default;
    virtual int uniform(int minimum, int maximum) = 0;
};

class MtRandomSource final : public IRandomSource {
public:
    MtRandomSource() : engine_(std::random_device{}()) {}
    explicit MtRandomSource(std::uint32_t seed) : engine_(seed) {}

    int uniform(int minimum, int maximum) override {
        return std::uniform_int_distribution<int>(minimum, maximum)(engine_);
    }

private:
    std::mt19937 engine_;
};

} // namespace dndmud
