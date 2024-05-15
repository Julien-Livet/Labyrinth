#ifndef LABYRINTH3D_SOLVER_FAILUREEXCEPTION
#define LABYRINTH3D_SOLVER_FAILUREEXCEPTION

#include <stdexcept>

namespace Labyrinth3d
{
    namespace Solver
    {
        class FailureException : std::runtime_error
        {
            public:
                FailureException(std::string const& what_arg = std::string("Labyrinth3d::Solver::FailureException"));
        };
    }
}

#endif // LABYRINTH3D_SOLVER_FAILUREEXCEPTION
