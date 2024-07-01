#ifndef LABYRINTH3D_ALGORITHM_KRUSKAL_H
#define LABYRINTH3D_ALGORITHM_KRUSKAL_H

/*!
 *  \file Kruskal.h
 *  \brief Generate a perfect labyrinth with Kruskal algorithm
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <functional>
#include <vector>

#include <boost/pending/disjoint_sets.hpp>

#include "../Grid.h"
#include "../Labyrinth.h"
#include "../utility.h"

namespace Labyrinth3d
{
    namespace Algorithm
    {
        /*!
         *  \brief Generate a perfect labyrinth with with Kruskal algorithm
         *
         *  Each cell is marked with a different number.
         *  A random cell with a random direction is chosen: if neighbour number cell
         *  is different, cells are merged (one of both numbers is kept) and the wall
         *  between them is opened.
         */
        class Kruskal
        {
            public:
                /*!
                 *  \brief Recursive indicates the use of recursive function calls
                 *         and Iterative the use of iterative loops
                 */
                enum Type
                {
                    Recursive,
                    Iterative
                };

                Type type;

                /*!
                 *  \brief Constructor
                 *
                 *  \param type_: indicate how the algorithm will work (memory grows quickly with recursion unlike iterative loops)
                 */
                Kruskal(Type type_ = Recursive);

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
                template <class URNG>
                void recursiveGeneration_(URNG& g, SubGrid const& subGrid,
                                          std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                                          size_t cycleOperations = 0,
                                          std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                          std::chrono::milliseconds const* timeout = nullptr);

                template <class URNG>
                void loopGeneration_(URNG& g, SubGrid const& subGrid,
                                     std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                                     size_t cycleOperations = 0,
                                     std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                     std::chrono::milliseconds const* timeout = nullptr);

                inline void fill_(size_t value, std::vector<size_t>& grid, size_t height, size_t width,size_t depth,
                                  std::tuple<size_t, size_t, size_t> const& position, std::tuple<int, int, int> const& direction) const;
        };
    }
}

template <class URNG>
void Labyrinth3d::Algorithm::Kruskal::operator()(URNG& g, SubGrid const& subGrid,
                                                 std::function<void(std::chrono::milliseconds)> const& sleep,
                                                 size_t cycleOperations, std::chrono::milliseconds const& cyclePause,
                                                 std::chrono::milliseconds const* timeout)
{
    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const depth(subGrid.depth());

    for (size_t k{1}; k < depth - 1; ++k)
    {
        for (size_t i{1}; i < height - 3; i += 2)
        {
            for (size_t j{2}; j < width - 1; j += 2)
            {
                subGrid.set(i, j, k);
                subGrid.set(i + 1, j - 1, k);
            }
        }

        for (size_t i{2}; i < height - 1; i += 2)
            subGrid.set(i, width - 2, k);

        for (size_t j{2}; j < width - 1; j += 2)
            subGrid.set(height - 2, j, k);
    }

    for (size_t k{2}; k < depth - 1; k += 2)
    {
        for (size_t i{1}; i < height; i += 2)
        {
            for (size_t j{1}; j < width; j += 2)
                subGrid.set(i, j, k);
        }
    }

    for (size_t k{1}; k < depth - 1; k += 2)
    {
        for (size_t i{2}; i < height - 1; i += 2)
        {
            for (size_t j{2}; j < width - 1; j += 2)
                subGrid.set(i, j, k);
        }
    }

    if (type == Recursive)
        recursiveGeneration_(g, subGrid, sleep, cycleOperations, cyclePause, timeout);
    else// if (type == Iterative)
        loopGeneration_(g, subGrid, sleep, cycleOperations, cyclePause, timeout);
}

template <class URNG>
void Labyrinth3d::Algorithm::Kruskal::recursiveGeneration_(URNG &g, SubGrid const& subGrid,
                                                           std::function<void(std::chrono::milliseconds)> const& sleep,
                                                           size_t cycleOperations,
                                                           std::chrono::milliseconds const& cyclePause,
                                                           std::chrono::milliseconds const* timeout)
{
    auto const t{std::chrono::steady_clock::now()};

    auto const height{subGrid.height()};
    auto const width{subGrid.width()};
    auto const depth{subGrid.depth()};
    auto const rows{subGrid.rows()};
    auto const columns{subGrid.columns()};
    auto const floors{subGrid.floors()};

    std::vector<size_t> grid;
    std::vector<std::tuple<size_t, size_t, size_t> > possiblePositions;

    grid.reserve(rows * columns * floors);
    possiblePositions.reserve(rows * columns * floors);

    for (size_t i{0}; i < rows * columns * floors; ++i)
    {
        grid.push_back(i);
        possiblePositions.emplace_back(std::make_tuple((i / columns * 2 + 1) % rows,
                                                       (i % columns) * 2 + 1,
                                                       (i / (columns * rows)) * 2 + 1));
    }

    size_t openedWalls{0};

    while (openedWalls < (height - 1) * (width - 1)  * (depth - 1) / 8 - 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (cycleOperations && cyclePause.count() && !(openedWalls % cycleOperations))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        std::vector<std::tuple<int, int, int> > possibleDirections{std::make_tuple(2, 0, 0), std::make_tuple(-2, 0, 0),
                                                                   std::make_tuple(0, 2, 0), std::make_tuple(0, -2, 0),
                                                                   std::make_tuple(0, 0, 2), std::make_tuple(0, 0, -2)};

        size_t const i{g() % possiblePositions.size()};

        size_t j{g() % possibleDirections.size()};

        bool b{std::get<0>(possiblePositions[i]) + std::get<0>(possibleDirections[j]) < height
               && std::get<1>(possiblePositions[i]) + std::get<1>(possibleDirections[j]) < width
               && std::get<2>(possiblePositions[i]) + std::get<2>(possibleDirections[j]) < depth
               && grid[((std::get<2>(possiblePositions[i]) - 1) * rows
                        + std::get<0>(possiblePositions[i]) - 1) / 2 * columns
                       + (std::get<1>(possiblePositions[i]) - 1) / 2]
                  != grid[((std::get<2>(possiblePositions[i]) - 1 + std::get<2>(possibleDirections[j])) * rows
                           + std::get<0>(possiblePositions[i]) - 1 + std::get<0>(possibleDirections[j])) / 2 * columns
                          + (std::get<1>(possiblePositions[i]) - 1 + std::get<1>(possibleDirections[j])) / 2]};

        while (!b)
        {
            possibleDirections[j] = possibleDirections.back();

            possibleDirections.pop_back();

            if (possibleDirections.empty())
                break;

            j = g() % possibleDirections.size();

            b = std::get<0>(possiblePositions[i]) + std::get<0>(possibleDirections[j]) < height
                && std::get<1>(possiblePositions[i]) + std::get<1>(possibleDirections[j]) < width
                && std::get<2>(possiblePositions[i]) + std::get<2>(possibleDirections[j]) < depth
                && grid[((std::get<2>(possiblePositions[i]) - 1) * rows
                         + std::get<0>(possiblePositions[i]) - 1) / 2 * columns
                        + (std::get<1>(possiblePositions[i]) - 1) / 2]
                   != grid[((std::get<2>(possiblePositions[i]) - 1 + std::get<2>(possibleDirections[j])) * rows
                            + std::get<0>(possiblePositions[i]) - 1 + std::get<0>(possibleDirections[j])) / 2 * columns
                           + (std::get<1>(possiblePositions[i]) - 1 + std::get<1>(possibleDirections[j])) / 2];
        }

        if (!b)
        {
            possiblePositions[i] = possiblePositions.back();

            possiblePositions.pop_back();
        }
        else
        {
            ++openedWalls;

            subGrid.toggle(std::get<0>(possiblePositions[i]) + std::get<0>(possibleDirections[j]) / 2,
                           std::get<1>(possiblePositions[i]) + std::get<1>(possibleDirections[j]) / 2,
                           std::get<2>(possiblePositions[i]) + std::get<2>(possibleDirections[j]) / 2);

            fill_(grid[((std::get<2>(possiblePositions[i]) - 1 + std::get<2>(possibleDirections[j])) * rows
                        + std::get<0>(possiblePositions[i]) - 1 + std::get<0>(possibleDirections[j])) / 2 * columns
                  + (std::get<1>(possiblePositions[i]) - 1 + std::get<1>(possibleDirections[j])) / 2],
                  grid, height, width, depth, possiblePositions[i], possibleDirections[j]);
        }
    }
}

template <class URNG>
void Labyrinth3d::Algorithm::Kruskal::loopGeneration_(URNG &g, SubGrid const& subGrid,
                                                      std::function<void(std::chrono::milliseconds)> const& sleep,
                                                      size_t cycleOperations, std::chrono::milliseconds const& cyclePause,
                                                      std::chrono::milliseconds const* timeout)
{
    auto const t{std::chrono::steady_clock::now()};

    auto const height{subGrid.height()};
    auto const width{subGrid.width()};
    auto const depth{subGrid.depth()};
    auto const rows{subGrid.rows()};
    auto const columns{subGrid.columns()};
    auto const floors{subGrid.floors()};

    std::vector<size_t> grid;
    std::vector<std::tuple<size_t, size_t, size_t> > possiblePositions;

    grid.reserve(rows * columns * floors);
    possiblePositions.reserve(rows * columns * floors);

    for (size_t i{0}; i < rows * columns * floors; ++i)
    {
        grid.emplace_back(i);
        possiblePositions.emplace_back(std::make_tuple((i / columns * 2 + 1) % rows,
                                                       (i % columns) * 2 + 1,
                                                       (i / (columns * rows)) * 2 + 1));
    }

    boost::disjoint_sets_with_storage<> disjointsSets{rows * columns * floors};

    size_t openedWalls{0};

    while (openedWalls < (height - 1) * (width - 1) * (depth - 1) / 8 - 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (cycleOperations && cyclePause.count() && !(openedWalls % cycleOperations))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        std::vector<std::tuple<int, int, int> > possibleDirections{std::make_tuple(2, 0, 0), std::make_tuple(-2, 0, 0),
                                                                   std::make_tuple(0, 2, 0), std::make_tuple(0, -2, 0),
                                                                   std::make_tuple(0, 0, 2), std::make_tuple(0, 0, -2)};

        size_t const i{g() % possiblePositions.size()};

        size_t j{g() % possibleDirections.size()};

        auto const element{disjointsSets.find_set(((std::get<2>(possiblePositions[i]) - 1)) * rows
                                                   + std::get<0>(possiblePositions[i]) - 1) / 2 * columns
                                                  + (std::get<1>(possiblePositions[i]) - 1) / 2};

        bool b{std::get<0>(possiblePositions[i]) + std::get<0>(possibleDirections[j]) < height
               && std::get<1>(possiblePositions[i]) + std::get<1>(possibleDirections[j]) < width
               && std::get<2>(possiblePositions[i]) + std::get<2>(possibleDirections[j]) < depth
               && element != disjointsSets.find_set(((std::get<2>(possiblePositions[i]) - 1 + std::get<2>(possibleDirections[j])) * rows
                                                     + std::get<0>(possiblePositions[i]) - 1 + std::get<0>(possibleDirections[j])) / 2 * columns
                                                    + (std::get<1>(possiblePositions[i]) - 1 + std::get<1>(possibleDirections[j])) / 2)};

        while (!b)
        {
            possibleDirections[j] = possibleDirections.back();

            possibleDirections.pop_back();

            if (possibleDirections.empty())
                break;

            j = g() % possibleDirections.size();

            b = std::get<0>(possiblePositions[i]) + std::get<0>(possibleDirections[j]) < height
                && std::get<1>(possiblePositions[i]) + std::get<1>(possibleDirections[j]) < width
                && std::get<2>(possiblePositions[i]) + std::get<2>(possibleDirections[j]) < depth
                && element != disjointsSets.find_set(((std::get<2>(possiblePositions[i]) - 1 + std::get<2>(possibleDirections[j])) * rows
                                                      + std::get<0>(possiblePositions[i]) - 1 + std::get<0>(possibleDirections[j])) / 2 * columns
                                                     + (std::get<1>(possiblePositions[i]) - 1 + std::get<1>(possibleDirections[j])) / 2);
        }

        if (!b)
        {
            possiblePositions[i] = possiblePositions.back();

            possiblePositions.pop_back();
        }
        else
        {
            ++openedWalls;

            subGrid.toggle(std::get<0>(possiblePositions[i]) + std::get<0>(possibleDirections[j]) / 2,
                           std::get<1>(possiblePositions[i]) + std::get<1>(possibleDirections[j]) / 2,
                           std::get<2>(possiblePositions[i]) + std::get<2>(possibleDirections[j]) / 2);

            disjointsSets.union_set(((std::get<2>(possiblePositions[i]) - 1) * rows
                                     + std::get<0>(possiblePositions[i]) - 1) / 2 * columns
                                    + (std::get<1>(possiblePositions[i]) - 1) / 2,
                                    ((std::get<2>(possiblePositions[i]) - 1 + std::get<2>(possibleDirections[j])) * rows
                                     + std::get<0>(possiblePositions[i]) - 1 + std::get<0>(possibleDirections[j])) / 2 * columns
                                    + (std::get<1>(possiblePositions[i]) - 1 + std::get<1>(possibleDirections[j])) / 2);
        }
    }
}

inline void Labyrinth3d::Algorithm::Kruskal::fill_(size_t value, std::vector<size_t>& grid,
                                                   size_t height, size_t width, size_t depth,
                                                   std::tuple<size_t, size_t, size_t> const& position,
                                                   std::tuple<int, int, int> const& direction) const
{
    size_t const columns{(width - 1) / 2};
    size_t const rows{(height - 1) / 2};

    grid[((std::get<2>(position) - 1 + std::get<2>(direction)) * rows
          + std::get<0>(position) - 1 + std::get<0>(direction)) / 2 * columns
         + (std::get<1>(position) - 1 + std::get<1>(direction)) / 2] = grid[((std::get<2>(position) - 1) * rows
                                                                             + std::get<0>(position) - 1) / 2 * columns
                                                                            + (std::get<1>(position) - 1) / 2];

    std::array<std::tuple<int, int, int>, 6> constexpr possibleDirections{std::make_tuple(2, 0, 0), std::make_tuple(-2, 0, 0),
                                                                          std::make_tuple(0, 2, 0), std::make_tuple(0, -2, 0),
                                                                          std::make_tuple(0, 0, 2), std::make_tuple(0, 0, -2)};

    for (auto const& d : possibleDirections)
        if (std::get<0>(position) + std::get<0>(direction) + std::get<0>(d) < height
            && std::get<1>(position) + std::get<1>(direction) + std::get<1>(d) < width
            && std::get<2>(position) + std::get<2>(direction) + std::get<2>(d) < depth
            && grid[((std::get<2>(position) - 1 + std::get<2>(direction) + std::get<2>(d)) * rows
                     + std::get<0>(position) - 1 + std::get<0>(direction) + std::get<0>(d)) / 2 * columns
                    + (std::get<1>(position) - 1 + std::get<1>(direction) + std::get<1>(d)) / 2] == value)
            fill_(value, grid, height, width, depth,
                  std::make_tuple(std::get<0>(position) + std::get<0>(direction),
                                  std::get<1>(position) + std::get<1>(direction),
                                  std::get<2>(position) + std::get<2>(direction)), d);
}

#endif // LABYRINTH3D_ALGORITHM_KRUSKAL_H
