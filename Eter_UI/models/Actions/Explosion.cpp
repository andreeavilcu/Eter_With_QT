#include "Explosion.h"

std::pair<size_t, size_t> Explosion::getHole() {
    return this->m_hole;
}

void Explosion::setHole(const std::pair<size_t, size_t> &_hole) {
    this->m_hole = _hole;
}

std::vector<std::vector<Explosion::ExplosionEffect>> Explosion::generateExplosion(const size_t _size) {
    m_explosionEffects = std::vector(_size, std::vector<ExplosionEffect>(_size));

    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> indexDistribution{ 0, _size - 1};

    std::uniform_int_distribution<size_t> effectDistribution{ 0, 10 };
    std::bernoulli_distribution removeOrReplaceDistribution{ 0.5 };

    const size_t effectCount = indexDistribution(gen) + (_size - 1);

    bool sinkHoleGenerated = false;

    for (int _ = 0; _ < effectCount; _++) {
        size_t x, y;

        do {
            x = indexDistribution(gen);
            y = indexDistribution(gen);
        } while (m_explosionEffects[x][y] != ExplosionEffect::None);

        if (const size_t effect = effectDistribution(gen); effect && !sinkHoleGenerated) {
            m_explosionEffects[x][y] = ExplosionEffect::SinkHole;
            sinkHoleGenerated = true;
        }
            

        else
            m_explosionEffects[x][y] = removeOrReplaceDistribution(gen)
                ? ExplosionEffect::ReturnCard
                : ExplosionEffect::RemoveCard;
    }

    return m_explosionEffects;
}

bool Explosion::rotateExplosion(bool &_quit) {
    char choice;
    std::cin >> choice;

    if (tolower(choice) == 'c')
        return false;

    if (tolower(choice) == 'x') {
        _quit = true;
        return false;
    }

    if (tolower(choice) == 'r')
        rotateMatrixRight();

    return true;
}

void Explosion::rotateMatrixRight() {
    const std::vector<std::vector<ExplosionEffect>> temp = m_explosionEffects;

    for (int i = 0; i < m_explosionEffects.size(); ++i) {
        for (int j = 0; j < m_explosionEffects.size(); ++j) {
            m_explosionEffects[j][m_explosionEffects.size() - 1 - i] = temp[i][j];
        }
    }
}

void Explosion::printExplosion() const {
    std::cout << "Explosion:\n\n";

    for (size_t i = 0; i < m_explosionEffects.size(); ++i) {
        for (size_t j = 0; j < m_explosionEffects.size(); ++j) {
            switch (m_explosionEffects[i][j]) {
                case ExplosionEffect::None:
                    std::cout << "- ";
                    break;
                case ExplosionEffect::SinkHole:
                    std::cout << "H ";
                    break;
                case ExplosionEffect::RemoveCard:
                    std::cout << "R ";
                    break;
                case ExplosionEffect::ReturnCard:
                    std::cout << "r ";
                    break;
            }
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}