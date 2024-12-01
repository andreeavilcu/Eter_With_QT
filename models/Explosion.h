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

private:
    Explosion() = default;
    ~Explosion() = default;

public:
    enum class ExplosionEffect : size_t {
        None = 0,
        RemoveCard,
        ReturnCard,
        SinkHole,
    };

    std::vector<std::vector<ExplosionEffect>> generateExplosion(size_t _size);
    bool rotateExplosion(std::vector<std::vector<ExplosionEffect>>& _matrix, bool& _quit);
    void rotateMatrixRight(std::vector<std::vector<ExplosionEffect>>& _matrix);
    void printExplosion(const std::vector<std::vector<ExplosionEffect>>& _matrix) const;
};
