#ifndef LABYRINTH2D_ALGORITHM_TIMEOUTEXCEPTION
#define LABYRINTH2D_ALGORITHM_TIMEOUTEXCEPTION

#include <stdexcept>

namespace Labyrinth2d
{
    namespace Algorithm
    {
        class TimeoutException : std::runtime_error
        {
            public:
                TimeoutException(std::string const& what_arg = std::string("Labyrinth2d::Algorithm::TimeoutException"));
        };
    }
}

#endif // LABYRINTH2D_ALGORITHM_TIMEOUTEXCEPTION
