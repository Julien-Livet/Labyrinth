#ifndef LABYRINTH3D_SOLVER_TIMEOUTEXCEPTION
#define LABYRINTH3D_SOLVER_TIMEOUTEXCEPTION

#include <stdexcept>

namespace Labyrinth3d
{
    namespace Solver
    {
        class TimeoutException : std::runtime_error
        {
            public:
                TimeoutException(std::string const& what_arg = std::string("Labyrinth3d::Solver::TimeoutException"));
        };
    }
}

#endif // LABYRINTH3D_SOLVER_TIMEOUTEXCEPTION
