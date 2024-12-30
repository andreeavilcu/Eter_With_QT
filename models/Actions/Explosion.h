#pragma once

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

class Explosion {
public:
    Explosion(const Explosion&) = delete;
    Explosion& operator=(const Explosion&) = delete;

    static Explosion& getInstance() {
        static Explosion instance;
        return instance;
    }

    enum class ExplosionEffect : size_t {
        None = 0,
        RemoveCard,
        ReturnCard,
        SinkHole,
    };

private:
    Explosion() = default;
    ~Explosion() = default;

    std::pair<size_t, size_t> m_hole{ -1, -1 };
    std::vector<std::vector<ExplosionEffect>> m_explosionEffects{};

public:

    std::pair<size_t, size_t> getHole();
    void setHole(const std::pair<size_t, size_t>& _hole);

    std::vector<std::vector<ExplosionEffect>> getExplosionEffect() {
        return m_explosionEffects;
    }

    std::vector<std::vector<ExplosionEffect>> generateExplosion(size_t _size);
    bool rotateExplosion(bool& _quit);
    void rotateMatrixRight();
    void printExplosion() const;
};
