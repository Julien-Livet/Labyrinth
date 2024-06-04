#ifndef LABYRINTH3D_ALGORITHM_CELLFUSION_H
#define LABYRINTH3D_ALGORITHM_CELLFUSION_H

/*!
 *  \file CellFusion.h
 *  \brief Generate a perfect labyrinth with cell fusion
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 24/05/2024
 */

#include <chrono>
#include <functional>
#include <map>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth3d
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
        };
    }
}

template <class URNG>
void Labyrinth3d::Algorithm::CellFusion::operator()(URNG& g, SubGrid const& subGrid,
                                                    std::function<void(std::chrono::milliseconds)> const& sleep,
                                                    size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                    std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const depth(subGrid.depth());
    size_t const rows(subGrid.rows());
    size_t const columns(subGrid.columns());
    size_t const floors(subGrid.floors());

    for (size_t k(1); k < depth - 1; ++k)
    {
        for (size_t i(1); i < height - 3; i += 2)
        {
            for (size_t j(2); j < width - 1; j += 2)
            {
                subGrid.set(i, j, k);
                subGrid.set(i + 1, j - 1, k);
            }
        }

        for (size_t i(2); i < height - 1; i += 2)
            subGrid.set(i, width - 2, k);

        for (size_t j(2); j < width - 1; j += 2)
            subGrid.set(height - 2, j, k);
    }

    for (size_t k(2); k < depth - 1; k += 2)
    {
        for (size_t i(1); i < height; i += 2)
        {
            for (size_t j(1); j < width; j += 2)
                subGrid.set(i, j, k);
        }
    }

    for (size_t k(1); k < depth - 1; k += 2)
    {
        for (size_t i(2); i < height - 1; i += 2)
        {
            for (size_t j(2); j < width - 1; j += 2)
                subGrid.set(i, j, k);
        }
    }

    std::unordered_map<size_t, std::vector<std::tuple<size_t, size_t, size_t> > > cells;
    std::vector<size_t> indexCells;
    indexCells.resize(rows * columns * floors);

    for (size_t i{0}; i < rows; ++i)
    {
        for (size_t j{0}; j < columns; ++j)
        {
            for (size_t k{0}; k < floors; ++k)
            {
                cells[(k * rows + i) * columns + j] = std::vector<std::tuple<size_t, size_t, size_t> >{std::make_tuple(i, j, k)};
                indexCells[(k * rows + i) * columns + j] = (k * rows + i) * columns + j;
            }
        }
    }

    size_t openedDoorNumber = 0;

    while (cells.size() != 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (operationsCycle && cyclePause.count() && !(openedDoorNumber % operationsCycle))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        auto const v1{g() % cells.size()};

        auto it{cells.begin()};
        std::advance(it, v1);
        std::vector<std::tuple<size_t, size_t, size_t> > c{it->second};

        while (!c.empty())
        {
            auto const v2{g() % c.size()};

            auto const i{std::get<0>(c[v2])};
            auto const j{std::get<1>(c[v2])};
            auto const k{std::get<2>(c[v2])};

            std::vector<std::tuple<int, int, int> > directions{std::make_tuple<int, int, int>(1, 0, 0),
                                                               std::make_tuple<int, int, int>(-1, 0, 0),
                                                               std::make_tuple<int, int, int>(0, 1, 0),
                                                               std::make_tuple<int, int, int>(0, -1, 0),
                                                               std::make_tuple<int, int, int>(0, 0, 1),
                                                               std::make_tuple<int, int, int>(0, 0, -1)};

            while (!directions.empty())
            {
                auto const v3{g() % directions.size()};
                auto const di{std::get<0>(directions[v3])};
                auto const dj{std::get<1>(directions[v3])};
                auto const dk{std::get<2>(directions[v3])};

                if (i + di >= 0 && i + di < rows
                    && j + dj >= 0 && j + dj < columns
                    && k + dk >= 0 && k + dk < floors
                    /*&& subGrid(2 * i + 1 + di, 2 * j + 1 + dj, 2 * k + 1 + dk)*/
                    && it->first != indexCells[((k + dk) * rows + i + di) * columns + j + dj])
                {
                    auto const v4{indexCells[((k + dk) * rows + i + di) * columns + j + dj]};

                    subGrid.reset(2 * i + 1 + di, 2 * j + 1 + dj, 2 * k + 1 + dk);
                    cells[it->first].insert(cells[it->first].end(), cells[v4].begin(), cells[v4].end());

                    for (auto const& cell : cells[v4])
                        indexCells[(std::get<2>(cell) * rows + std::get<0>(cell)) * columns + std::get<1>(cell)] = it->first;

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

#endif // LABYRINTH3D_ALGORITHM_CELLFUSION_H
