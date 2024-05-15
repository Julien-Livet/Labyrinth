#include "Labyrinth2d/Solver/TimeoutException.h"

Labyrinth2d::Solver::TimeoutException::TimeoutException(std::string const& what_arg) : std::runtime_error(what_arg)
{
}
