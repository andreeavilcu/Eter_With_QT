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

    std::pair<size_t, size_t> m_hole{ -1, -1 };

public:
    enum class ExplosionEffect : size_t {
        None = 0,
        RemoveCard,
        ReturnCard,
        SinkHole,
    };

    std::pair<size_t, size_t> getHole();
    void setHole(const std::pair<size_t, size_t>& _hole);

    std::vector<std::vector<ExplosionEffect>> generateExplosion(size_t _size);
    bool rotateExplosion(std::vector<std::vector<ExplosionEffect>>& _matrix, bool& _quit);
    void rotateMatrixRight(std::vector<std::vector<ExplosionEffect>>& _matrix);
    void printExplosion(const std::vector<std::vector<ExplosionEffect>>& _matrix) const;
};
