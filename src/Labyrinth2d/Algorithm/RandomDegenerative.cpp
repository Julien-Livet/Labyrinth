#include <vector>
#include <limits>
#include <cassert>
#include <stdexcept>

#include "Labyrinth2d/Algorithm/RandomDegenerative.h"

Labyrinth2d::Algorithm::RandomDegenerative::RandomDegenerative(double wallDestructionPercentage) : wallDestructionPercentage_(wallDestructionPercentage)
{
    if (!(wallDestructionPercentage_ >= -std::numeric_limits<double>::epsilon() && wallDestructionPercentage_ <= 100.0 + std::numeric_limits<double>::epsilon()))
        throw std::invalid_argument("Invalid percentage");
}

double Labyrinth2d::Algorithm::RandomDegenerative::wallDestructionPercentage() const
{
    return wallDestructionPercentage_;
}
