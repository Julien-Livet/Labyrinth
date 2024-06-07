#ifndef LABYRINTH3D_ALGORITHM_SHUFFLESLICER_H
#define LABYRINTH3D_ALGORITHM_SHUFFLESLICER_H

/*!
 *  \file ShuffleSlicer.h
 *  \brief Generate a perfect labyrinth with shuffle slicer of 2D algorithms
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 07/06/2024
 */

#include "../Grid.h"
#include "../Labyrinth.h"
#include "Labyrinth2d/Labyrinth.h"

namespace Labyrinth3d
{
    namespace Algorithm
    {
        template<class URNG, std::size_t I = 0, typename... Tp>
        inline typename std::enable_if<I == sizeof...(Tp), void>::type
            applyAlgorithm(std::tuple<Tp...>& t, size_t n, Labyrinth2d::Labyrinth& l, URNG& g,
                           std::function<void(std::chrono::milliseconds)> const& sleep,
                           size_t cycleOperations, std::chrono::milliseconds const& cyclePause)
        {
        }

        template<class URNG, std::size_t I = 0, typename... Tp>
        inline typename std::enable_if<I < sizeof...(Tp), void>::type
            applyAlgorithm(std::tuple<Tp...>& t, size_t n, Labyrinth2d::Labyrinth& l, URNG& g,
                           std::function<void(std::chrono::milliseconds)> const& sleep,
                           size_t cycleOperations, std::chrono::milliseconds const& cyclePause)
        {
            if (I == n)
                l.generate(g, std::get<I>(t), sleep, cycleOperations, cyclePause);
            else
                applyAlgorithm<URNG, I + 1, Tp...>(t, n, l, g, sleep, cycleOperations, cyclePause);
        }

        /*!
         *  \brief Generate a perfect labyrinth by randomly generating 2D labyrinth at each floor
         */
        template<typename... Algorithms>
        struct ShuffleSlicer
        {
            /*!
             *  \brief Constructor
             */
            ShuffleSlicer(Algorithms const& ...a) : algorithms{std::forward_as_tuple(a...)}
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
                size_t const height(subGrid.height());
                size_t const width(subGrid.width());
                size_t const depth(subGrid.depth());
                size_t const rows(subGrid.rows());
                size_t const columns(subGrid.columns());
                size_t const floors(subGrid.floors());

                for (size_t k(2); k < depth - 1; k += 2)
                {
                    for (size_t i(1); i < height; i += 2)
                    {
                        for (size_t j(1); j < width; j += 2)
                            subGrid.set(i, j, k);
                    }

                    for (size_t i(2); i < height - 1; i += 2)
                    {
                        for (size_t j(1); j < width; j += 2)
                            subGrid.set(i, j, k);
                    }

                    for (size_t j(2); j < width - 1; j += 2)
                    {
                        for (size_t i(1); i < height; i += 2)
                            subGrid.set(i, j, k);
                    }
                }

                Labyrinth2d::Labyrinth l(rows, columns);

                for (size_t k{0}; k < floors; ++k)
                {
                    applyAlgorithm(algorithms, g() % sizeof...(Algorithms), l, g, sleep, cycleOperations, cyclePause);

                    for (size_t i{1}; i < height - 1; ++i)
                    {
                        for (size_t j{1}; j < width - 1; ++j)
                        {
                            if (i % 2 + j % 2 < 2)
                                subGrid.change(i, j, 2 * k + 1, l.grid()(i, j));
                        }
                    }

                    if (k != floors - 1)
                        subGrid.reset((g() % (rows - 1)) * 2 + 1, (g() % (columns - 1)) * 2 + 1, 2 * (k + 1));
                }
            }
        };
    }
}

#endif // LABYRINTH3D_ALGORITHM_SHUFFLESLICER_H
