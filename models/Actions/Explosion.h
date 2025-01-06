#pragma once

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>
#include <nlohmann/json.hpp>

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

    [[nodiscard]] nlohmann::json serialize() const {
        nlohmann::json json;

        nlohmann::json jsonArray = nlohmann::json::array();

        for (const auto& layer1 : m_explosionEffects) {
            nlohmann::json layer1Array = nlohmann::json::array();

            for (const auto& layer2 : layer1) {
                layer1Array.push_back(layer2);
            }

            jsonArray.push_back(layer1Array);
        }

        json["explosion"] = jsonArray;
        json["hole"] = this->m_hole;

        return json;
    }
};
