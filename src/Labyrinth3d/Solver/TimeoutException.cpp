#include "Labyrinth3d/Solver/TimeoutException.h"

Labyrinth3d::Solver::TimeoutException::TimeoutException(std::string const& what_arg) : std::runtime_error(what_arg)
{
}
