#ifndef LABYRINTH3D_ALGORITHM_TIMEOUTEXCEPTION
#define LABYRINTH3D_ALGORITHM_TIMEOUTEXCEPTION

#include <stdexcept>

namespace Labyrinth3d
{
    namespace Algorithm
    {
        class TimeoutException : std::runtime_error
        {
            public:
                TimeoutException(std::string const& what_arg = std::string("Labyrinth3d::Algorithm::TimeoutException"));
        };
    }
}

#endif // LABYRINTH3D_ALGORITHM_TIMEOUTEXCEPTION
