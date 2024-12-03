#include "Explosion.h"

std::pair<size_t, size_t> Explosion::getHole() {
    return this->m_hole;
}

void Explosion::setHole(const std::pair<size_t, size_t> &_hole) {
    this->m_hole = _hole;
}

std::vector<std::vector<Explosion::ExplosionEffect>> Explosion::generateExplosion(const size_t _size) {
    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> indexDistribution{ 0, _size - 1};

    std::uniform_int_distribution<size_t> effectDistribution{ 0, 10 };
    std::bernoulli_distribution removeOrReplaceDistribution{ 0.5 };

    std::vector<std::vector<ExplosionEffect>> explosionEffects;

    explosionEffects.resize(_size);
    for (auto& row : explosionEffects)
        row.resize(_size);

    const size_t effectCount = indexDistribution(gen) + (_size - 1);

    bool sinkHoleGenerated = false;

    for (int _ = 0; _ < effectCount; _++) {
        size_t x, y;

        do {
            x = indexDistribution(gen);
            y = indexDistribution(gen);
        } while (explosionEffects[x][y] != ExplosionEffect::None);

        if (const size_t effect = effectDistribution(gen); effect && !sinkHoleGenerated) {
            explosionEffects[x][y] = ExplosionEffect::SinkHole;
            sinkHoleGenerated = true;
        }
            

        else
            explosionEffects[x][y] = removeOrReplaceDistribution(gen)
                ? ExplosionEffect::ReturnCard
                : ExplosionEffect::RemoveCard;
    }

    return explosionEffects;
}

bool Explosion::rotateExplosion(std::vector<std::vector<ExplosionEffect>> &_matrix, bool &_quit) {
    char choice;
    std::cin >> choice;

    if (tolower(choice) == 'c')
        return false;

    if (tolower(choice) == 'x') {
        _quit = true;
        return false;
    }

    if (tolower(choice) == 'r')
        rotateMatrixRight(_matrix);

    return true;
}

void Explosion::rotateMatrixRight(std::vector<std::vector<ExplosionEffect>> &_matrix) {
    const std::vector<std::vector<ExplosionEffect>> temp = _matrix;

    for (int i = 0; i < _matrix.size(); ++i) {
        for (int j = 0; j < _matrix.size(); ++j) {
            _matrix[j][_matrix.size() - 1 - i] = temp[i][j];
        }
    }
}

void Explosion::printExplosion(const std::vector<std::vector<ExplosionEffect>>& _matrix) const {
    for (size_t i = 0; i < _matrix.size(); ++i) {
        for (size_t j = 0; j < _matrix.size(); ++j) {
            switch (_matrix[i][j]) {
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
}