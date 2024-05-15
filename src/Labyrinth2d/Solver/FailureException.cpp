#include "Labyrinth2d/Solver/FailureException.h"

Labyrinth2d::Solver::FailureException::FailureException(std::string const& what_arg) : std::runtime_error(what_arg)
{
}
