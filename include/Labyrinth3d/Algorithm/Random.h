#ifndef LABYRINTH3D_ALGORITHM_RANDOM_H
#define LABYRINTH3D_ALGORITHM_RANDOM_H

/*!
 *  \file Random.h
 *  \brief Switch randomly between two algorithms
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <thread>
#include <chrono>

#include "../Grid.h"

namespace Labyrinth3d
{
    namespace Algorithm
    {
        /*!
         *  \brief Switch randomly between two algorithms
         *
         *  Each cell is marked with a different number.
         *  A random cell with a random direction is chosen: if neighbour number cell
         *  is different, cells are merged (one of both numbers is kept) and the wall
         *  between them is opened.
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
                 *  \param operationsCycle: number of operations in each cycle
                 *  \param cyclePause: pause time between each cycle
                 *  \param timeout: time before to abort generation
                 */
                template <class URNG>
                void operator()(URNG& g, Grid::SubGrid const& subGrid, size_t operationsCycle = 0,
                                std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                Algorithm1& algorithm1_;
                Algorithm2& algorithm2_;
        };
    }
}

template <class Algorithm1, class Algorithm2>
Labyrinth3d::Algorithm::Random<Algorithm1, Algorithm2>::Random(Algorithm1& algorithm1,
                                                               Algorithm2& algorithm2) : algorithm1_(algorithm1),
                                                                                         algorithm2_(algorithm2)
{
}

template <class Algorithm1, class Algorithm2>
template <class URNG>
void Labyrinth3d::Algorithm::Random<Algorithm1, Algorithm2>::operator()(URNG& g, Grid::SubGrid const& subGrid,
                                                                        size_t operationsCycle,
                                                                        std::chrono::milliseconds const& cyclePause,
                                                                        std::chrono::milliseconds const* timeout)
{
    if (g() % 2)
        algorithm1_(g, subGrid, operationsCycle, cyclePause, timeout);
    else
        algorithm2_(g, subGrid, operationsCycle, cyclePause, timeout);
}

#endif // LABYRINTH3D_ALGORITHM_RANDOM_H
