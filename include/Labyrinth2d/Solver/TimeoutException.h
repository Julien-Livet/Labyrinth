#ifndef LABYRINTH2D_SOLVER_TIMEOUTEXCEPTION
#define LABYRINTH2D_SOLVER_TIMEOUTEXCEPTION

#include <stdexcept>

namespace Labyrinth2d
{
    namespace Solver
    {
        class TimeoutException : std::runtime_error
        {
            public:
                TimeoutException(std::string const& what_arg = std::string("Labyrinth2d::Solver::TimeoutException"));
        };
    }
}

#endif // LABYRINTH2D_SOLVER_TIMEOUTEXCEPTION
