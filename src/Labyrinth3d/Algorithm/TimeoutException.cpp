#include "Labyrinth3d/Algorithm/TimeoutException.h"

Labyrinth3d::Algorithm::TimeoutException::TimeoutException(std::string const& what_arg) : std::runtime_error(what_arg)
{
}
