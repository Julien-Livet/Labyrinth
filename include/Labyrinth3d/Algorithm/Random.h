#ifndef LABYRINTH3D_ALGORITHM_RANDOM_H
#define LABYRINTH3D_ALGORITHM_RANDOM_H

/*!
 *  \file Random.h
 *  \brief Switch randomly between several algorithms
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <tuple>

#include "../Grid.h"

namespace Labyrinth3d
{
    namespace Algorithm
    {
        template<class URNG, std::size_t I = 0, typename... Tp>
        inline typename std::enable_if<I == sizeof...(Tp), void>::type
            applyAlgorithm(std::tuple<Tp...>& /*t*/, size_t /*n*/, URNG& /*g*/,
                           std::function<void(std::chrono::milliseconds)> const& /*sleep*/,
                           size_t /*cycleOperations*/, std::chrono::milliseconds const& /*cyclePause*/,
                           std::chrono::milliseconds const* /*timeout*/)
        {
        }

        template<class URNG, std::size_t I = 0, typename... Tp>
        inline typename std::enable_if<I < sizeof...(Tp), void>::type
            applyAlgorithm(std::tuple<Tp...>& t, size_t n, URNG& g,
                           std::function<void(std::chrono::milliseconds)> const& sleep,
                           size_t cycleOperations, std::chrono::milliseconds const& cyclePause,
                           std::chrono::milliseconds const* timeout)
        {
            if (I == n)
                std::get<I>(t)(g, sleep, cycleOperations, timeout);
            else
                applyAlgorithm<URNG, I + 1, Tp...>(t, n, g, sleep, cycleOperations, cyclePause, timeout);
        }

        /*!
         *  \brief Switch randomly between several algorithms
         */
        template<typename... Algorithms>
        class Random
        {
            public:
                /*!
                 *  \brief Constructor
                 *
                 *  \param a: algorithms used to generate the labyrinth
                 */
                Random(Algorithms const& ...a) : algorithms{std::forward_as_tuple(a...)}
                {
                }

                std::tuple<Algorithms ...> algorithms;

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
                                std::chrono::milliseconds const* timeout = nullptr)
                {
                    applyAlgorithm(algorithms, g() % sizeof...(Algorithms), g, sleep, cycleOperations, cyclePause, timeout);
                }
        };
    }
}

#endif // LABYRINTH3D_ALGORITHM_RANDOM_H
