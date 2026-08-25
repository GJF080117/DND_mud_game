#pragma once

#include <cstdint>
#include <random>

namespace dndmud {

/// 统一随机数的使用方式，测试时可以换成预先指定的数字。
class IRandomSource {
public:
    virtual ~IRandomSource() = default;
    /// 返回闭区间 [minimum, maximum] 内的整数。
    virtual int uniform(int minimum, int maximum) = 0;
};

/// 使用 C++ 标准库生成正式游戏需要的随机数。
class MtRandomSource final : public IRandomSource {
public:
    /// 每次启动时生成一组不同的随机结果。
    MtRandomSource() : engine_(std::random_device{}()) {}
    /// 使用固定起始数字，方便重复得到同一组结果。
    explicit MtRandomSource(std::uint32_t seed) : engine_(seed) {}

    /// 在最小值和最大值之间等概率生成一个整数。
    int uniform(int minimum, int maximum) override {
        return std::uniform_int_distribution<int>(minimum, maximum)(engine_);
    }

private:
    std::mt19937 engine_;
};

} // namespace dndmud
