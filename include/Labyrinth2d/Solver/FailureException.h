#ifndef LABYRINTH2D_SOLVER_FAILUREEXCEPTION
#define LABYRINTH2D_SOLVER_FAILUREEXCEPTION

#include <stdexcept>

namespace Labyrinth2d
{
    namespace Solver
    {
        class FailureException : std::runtime_error
        {
            public:
                FailureException(std::string const& what_arg = std::string("Labyrinth2d::Solver::FailureException"));
        };
    }
}

#endif // LABYRINTH2D_SOLVER_FAILUREEXCEPTION
