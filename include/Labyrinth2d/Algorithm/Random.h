#ifndef LABYRINTH2D_ALGORITHM_RANDOM_H
#define LABYRINTH2D_ALGORITHM_RANDOM_H

/*!
 *  \file Random.h
 *  \brief Switch randomly between two algorithms
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>

#include "../Grid.h"

namespace Labyrinth2d
{
    namespace Algorithm
    {
        /*!
         *  \brief Switch randomly between two algorithms
         */
        template <class Algorithm1, class Algorithm2>
        class Random
        {
            public:
                /*!
                 *  \brief Constructor
                 *
                 *  \param algorithm1: first algorithm used to generate the labyrinth
                 *  \param algorithm2: second algorithm used to generate the labyrinth
                 */
                Random(Algorithm1& algorithm1, Algorithm2& algorithm2);

                /*!
                 *  \brief Generate the labyrinth
                 *
                 *  \param g: uniform random number generator
                 *  \param subGrid: sub-grid of labyrinth grid
                 *  \param sleep: sleep function
                 *  \param cycleOperations: number of operations in each cycle
                 *  \param cyclePause: pause time between each cycle
                 *  \param timeout: time before to abort generation
                 */
                template <class URNG>
                void operator()(URNG& g, SubGrid const& subGrid,
                                std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                                size_t cycleOperations = 0,
                                std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                Algorithm1& algorithm1_;
                Algorithm2& algorithm2_;
        };
    }
}

template <class Algorithm1, class Algorithm2>
Labyrinth2d::Algorithm::Random<Algorithm1, Algorithm2>::Random(Algorithm1& algorithm1,
                                                               Algorithm2& algorithm2) : algorithm1_(algorithm1),
                                                                                         algorithm2_(algorithm2)
{
}

template <class Algorithm1, class Algorithm2>
template <class URNG>
void Labyrinth2d::Algorithm::Random<Algorithm1, Algorithm2>::operator()(URNG& g, SubGrid const& subGrid,
                                                                        std::function<void(std::chrono::milliseconds)> const& sleep,
                                                                        size_t cycleOperations,
                                                                        std::chrono::milliseconds const& cyclePause,
                                                                        std::chrono::milliseconds const* timeout)
{
    if (g() % 2)
        algorithm1_(g, subGrid, sleep, cycleOperations, cyclePause, timeout);
    else
        algorithm2_(g, subGrid, sleep, cycleOperations, cyclePause, timeout);
}

#endif // LABYRINTH2D_ALGORITHM_RANDOM_H
