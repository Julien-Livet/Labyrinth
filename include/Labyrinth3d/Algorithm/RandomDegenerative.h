#ifndef LABYRINTH3D_ALGORITHM_RANDOMDEGENERATIVE_H
#define LABYRINTH3D_ALGORITHM_RANDOMDEGENERATIVE_H

/*!
 *  \file RandomDegenerative.h
 *  \brief Degenerate a perfect labyrinth by randomly opening walls
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth3d
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
                 *  \param sleep: sleep function
                 *  \param operationsCycle: number of operations in each cycle
                 *  \param cyclePause: pause time between each cycle
                 *  \param timeout: time before to abort generation
                 */
                template <class URNG>
                void operator()(URNG& g, SubGrid const& subGrid,
                                std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                                size_t operationsCycle = 0,
                                std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                double wallDestructionPercentage_;
        };
    }
}

template <class URNG>
void Labyrinth3d::Algorithm::RandomDegenerative::operator()(URNG& g, SubGrid const& subGrid,
                                                            std::function<void(std::chrono::milliseconds)> const& sleep,
                                                            size_t operationsCycle,
                                                            std::chrono::milliseconds const& cyclePause,
                                                            std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    std::vector<std::tuple<size_t, size_t, size_t> > walls;

    for (size_t k(1); k < subGrid.depth(); k += 2)
        for (size_t i(1); i < subGrid.height() - 1; ++i)
            for (size_t j(0); j < subGrid.columns() - (i % 2 ? 1 : 0); ++j)
                if (subGrid(i, 2 * j + 1 + (i % 2 ? 1 : 0), k))
                    walls.push_back(std::make_tuple(i, 2 * j + 1 + (i % 2 ? 1 : 0), k));

    for (size_t i(1); i < subGrid.height(); i += 2)
        for (size_t k(2); k < subGrid.depth() - 1; k += 2)
            for (size_t j(1); j < subGrid.height() - 1; j += 2)
                if (subGrid(i, j, k))
                    walls.push_back(std::make_tuple(i, j, k));

    size_t const destructedWallTotal(static_cast<size_t>(static_cast<double>(walls.size()) * wallDestructionPercentage_ / 100.0));

    size_t destructedWalls(0);

    while (destructedWalls < destructedWallTotal && !walls.empty())
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (operationsCycle && cyclePause.count() && !(destructedWalls % operationsCycle))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        size_t const k(g() % walls.size());

        subGrid.toggle(std::get<0>(walls[k]), std::get<1>(walls[k]), std::get<2>(walls[k]));

        walls[k] = walls.back();

        walls.pop_back();

        ++destructedWalls;
    }
}

#endif // LABYRINTH3D_ALGORITHM_RANDOMDEGENERATIVE_H
