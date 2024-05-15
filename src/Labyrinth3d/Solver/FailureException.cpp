#include "Labyrinth3d/Solver/FailureException.h"

Labyrinth3d::Solver::FailureException::FailureException(std::string const& what_arg) : std::runtime_error(what_arg)
{
}
