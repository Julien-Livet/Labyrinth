#ifndef LABYRINTH2D_ALGORITHM_RANDOMDEGENERATIVE_H
#define LABYRINTH2D_ALGORITHM_RANDOMDEGENERATIVE_H

/*!
 *  \file RandomDegenerative.h
 *  \brief Degenerate a perfect labyrinth by randomly opening walls
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <thread>

#include "../Grid.h"
#include "../Labyrinth.h"

// TODO: Add a parameter to avoid as possible alone walls...

namespace Labyrinth2d
{
    namespace Algorithm
    {
        /*!
         *  \brief Degenerate a perfect labyrinth by randomly opening walls
         *
         *  A row or a column is randomly chosen while size is not reached.
         *  This line is expanded in orthogonal direction with 5 x 5 sized labyrinths.
         */
        class RandomDegenerative
        {
            public:
                /*!
                 *  \brief Constructor
                 *
                 *  \param wallDestructionPercentage: percentage of walls destruction
                 */
                RandomDegenerative(double wallDestructionPercentage);

                /*!
                 *  \return The percentage of wall destruction
                 */
                double wallDestructionPercentage() const;

                /*!
                 *  \brief Degenerate the labyrinth
                 *
                 *  \param g: uniform random number generator
                 *  \param subGrid: sub-grid of labyrinth grid
                 *  \param operationsCycle: number of operations in each cycle
                 *  \param cyclePause: pause time between each cycle
                 *  \param timeout: time before to abort generation
                 */
                template <class URNG>
                void operator()(URNG& g, SubGrid<bool> const& subGrid, size_t operationsCycle = 0,
                                std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                double wallDestructionPercentage_;
        };
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::RandomDegenerative::operator()(URNG& g, SubGrid<bool> const& subGrid,
                                                            size_t operationsCycle,
                                                            std::chrono::milliseconds const& cyclePause,
                                                            std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    std::vector<std::pair<size_t, size_t> > walls;

    for (size_t i(1); i < subGrid.height() - 1; ++i)
        for (size_t j(0); j < subGrid.columns() - (i % 2 ? 1 : 0); ++j)
            if (subGrid(i, 2 * j + 1 + (i % 2 ? 1 : 0)))
                walls.push_back(std::make_pair(i, 2 * j + 1 + (i % 2 ? 1 : 0)));

    size_t const destructedWallTotal(static_cast<size_t>(static_cast<double>(walls.size()) * wallDestructionPercentage_ / 100.0));

    size_t destructedWalls(0);

    while (destructedWalls < destructedWallTotal && !walls.empty())
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
        if (operationsCycle && cyclePause.count() && !(destructedWalls % operationsCycle))
            std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        size_t const k(g() % walls.size());

        subGrid.toggle(walls[k].first, walls[k].second);

        walls[k] = walls.back();

        walls.pop_back();

        ++destructedWalls;
    }
}

#endif // LABYRINTH2D_ALGORITHM_RANDOMDEGENERATIVE_H
