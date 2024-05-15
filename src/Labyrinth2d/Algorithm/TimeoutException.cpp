#include "Labyrinth2d/Algorithm/TimeoutException.h"

Labyrinth2d::Algorithm::TimeoutException::TimeoutException(std::string const& what_arg) : std::runtime_error(what_arg)
{
}
