#ifndef LABYRINTH2D_ALGORITHM_CELLFUSION_H
#define LABYRINTH2D_ALGORITHM_CELLFUSION_H

/*!
 *  \file CellFusion.h
 *  \brief Generate a perfect labyrinth with cell fusion
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 14/05/2024
 */

#include <chrono>
#include <functional>
#include <map>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth2d
{
    namespace Algorithm
    {
        /*!
         *  \brief Generate a perfect labyrinth by randomly searching a way
         *
         *  Each cell is marked with a different number.
         *  A random cell with a random direction is chosen: if neighbour number cell
         *  is different, cells are merged (one of both numbers is kept) and the wall
         *  between them is opened.
         */
        struct CellFusion
        {
            /*!
             *  \brief Constructor
             */
            CellFusion();

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
        };
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::CellFusion::operator()(URNG& g, SubGrid const& subGrid,
                                                    std::function<void(std::chrono::milliseconds)> const& sleep,
                                                    size_t cycleOperations, std::chrono::milliseconds const& cyclePause,
                                                    std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const rows(subGrid.rows());
    size_t const columns(subGrid.columns());

    for (size_t i(1); i < height - 3; i += 2)
    {
        for (size_t j(2); j < width - 1; j += 2)
        {
            subGrid.set(i, j);
            subGrid.set(i + 1, j - 1);
        }
    }

    for (size_t i(2); i < height - 1; i += 2)
        subGrid.set(i, width - 2);

    for (size_t j(2); j < width - 1; j += 2)
        subGrid.set(height - 2, j);

    std::unordered_map<size_t, std::vector<std::pair<size_t, size_t> > > cells;
    std::vector<size_t> indexCells;
    indexCells.resize(rows * columns);

    for (size_t i{0}; i < rows; ++i)
    {
        for (size_t j{0}; j < columns; ++j)
        {
            cells[i * columns + j] = std::vector<std::pair<size_t, size_t> >{std::make_pair(i, j)};
            indexCells[i * columns + j] = i * columns + j;
        }
    }

    size_t openedDoorNumber = 0;

    while (cells.size() != 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (cycleOperations && cyclePause.count() && !(openedDoorNumber % cycleOperations))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        auto const v1{g() % cells.size()};

        auto it{cells.begin()};
        std::advance(it, v1);
        std::vector<std::pair<size_t, size_t> > c{it->second};

        while (!c.empty())
        {
            auto const v2{g() % c.size()};

            auto const i{c[v2].first};
            auto const j{c[v2].second};

            std::vector<std::pair<int, int> > directions{std::make_pair<int, int>(1, 0),
                                                         std::make_pair<int, int>(-1, 0),
                                                         std::make_pair<int, int>(0, 1),
                                                         std::make_pair<int, int>(0, -1)};

            while (!directions.empty())
            {
                auto const v3{g() % directions.size()};
                auto const di{directions[v3].first};
                auto const dj{directions[v3].second};

                if (i + di >= 0 && i + di < rows
                    && j + dj >= 0 && j + dj < columns
                    && it->first != indexCells[(i + di) * columns + j + dj])
                {
                    auto const v4{indexCells[(i + di) * columns + j + dj]};

                    subGrid.reset(2 * i + 1 + di, 2 * j + 1 + dj);
                    cells[it->first].insert(cells[it->first].end(), cells[v4].begin(), cells[v4].end());

                    for (auto const& cell : cells[v4])
                        indexCells[cell.first * columns + cell.second] = it->first;

                    cells.erase(cells.find(v4));

                    ++openedDoorNumber;

                    break;
                }

                directions[v3] = directions.back();
                directions.pop_back();
            }

            if (!directions.empty())
                break;

            c[v2] = c.back();
            c.pop_back();
        }
    }
}

#endif // LABYRINTH2D_ALGORITHM_CELLFUSION_H
