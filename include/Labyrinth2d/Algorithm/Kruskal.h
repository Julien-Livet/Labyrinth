#ifndef LABYRINTH2D_ALGORITHM_KRUSKAL_H
#define LABYRINTH2D_ALGORITHM_KRUSKAL_H

/*!
 *  \file Kruskal.h
 *  \brief Generate a perfect labyrinth with Kruskal algorithm
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <thread>
#include <vector>

#include <boost/pending/disjoint_sets.hpp>

#include "../Grid.h"
#include "../Labyrinth.h"
#include "../utility.h"

namespace Labyrinth2d
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
                 *  \param operationsCycle: number of operations in each cycle
                 *  \param cyclePause: pause time between each cycle
                 *  \param timeout: time before to abort generation
                 */
                template <class URNG>
                void operator()(URNG& g, SubGrid<bool> const& subGrid, size_t operationsCycle = 0,
                                std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                template <class URNG>
                void recursiveGeneration_(URNG& g, SubGrid<bool> const& subGrid, size_t operationsCycle = 0,
                                          std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                          std::chrono::milliseconds const* timeout = nullptr);

                template <class URNG>
                void loopGeneration_(URNG& g, SubGrid<bool> const& subGrid, size_t operationsCycle = 0,
                                     std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                     std::chrono::milliseconds const* timeout = nullptr);

                inline void fill_(size_t value, std::vector<size_t>& grid, size_t height, size_t width,
                                  std::pair<size_t, size_t> const& position, std::pair<int, int> const& direction) const;
        };
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::Kruskal::operator()(URNG& g, SubGrid<bool> const& subGrid,
                                                 size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                 std::chrono::milliseconds const* timeout)
{
    size_t const height(subGrid.height());
    size_t const width(subGrid.width());

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

    if (type == Recursive)
        recursiveGeneration_(g, subGrid, operationsCycle, cyclePause, timeout);
    else// if (type == Iterative)
        loopGeneration_(g, subGrid, operationsCycle, cyclePause, timeout);
}

template <class URNG>
void Labyrinth2d::Algorithm::Kruskal::recursiveGeneration_(URNG &g, SubGrid<bool> const& subGrid,
                                                           size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                           std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const rows(subGrid.rows());
    size_t const columns(subGrid.columns());

    std::vector<size_t> grid;
    std::vector<std::pair<size_t, size_t> > possiblePositions;

    grid.reserve(rows * columns);
    possiblePositions.reserve(rows * columns);

    for (size_t i(0); i < rows * columns; ++i)
    {
        grid.push_back(i);
        possiblePositions.push_back(std::make_pair(i / columns * 2 + 1, (i % columns) * 2 + 1));
    }

    size_t openedWalls(0);

    while (openedWalls < (height - 1) * (width - 1) / 4 - 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
        if (operationsCycle && cyclePause.count() && !(openedWalls % operationsCycle))
            std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        std::vector<std::pair<int, int> > possibleDirections{std::make_pair(2, 0), std::make_pair(-2, 0),
                                                             std::make_pair(0, 2), std::make_pair(0, -2)};

        size_t const i(g() % possiblePositions.size());

        size_t j(g() % possibleDirections.size());

        bool b(possiblePositions[i].first + possibleDirections[j].first < height
               && possiblePositions[i].second + possibleDirections[j].second < width
               && grid[(possiblePositions[i].first - 1) / 2 * columns + (possiblePositions[i].second - 1) / 2]
                  != grid[(possiblePositions[i].first - 1 + possibleDirections[j].first) / 2 * columns
                     + (possiblePositions[i].second - 1 + possibleDirections[j].second) / 2]);

        while (!b)
        {
            possibleDirections[j] = possibleDirections.back();

            possibleDirections.pop_back();

            if (possibleDirections.empty())
                break;

            j = g() % possibleDirections.size();

            b = possiblePositions[i].first + possibleDirections[j].first < height
                && possiblePositions[i].second + possibleDirections[j].second < width
                && grid[(possiblePositions[i].first - 1) / 2 * columns + (possiblePositions[i].second - 1) / 2]
                   != grid[(possiblePositions[i].first - 1 + possibleDirections[j].first) / 2 * columns
                      + (possiblePositions[i].second - 1 + possibleDirections[j].second) / 2];
        }

        if (!b)
        {
            possiblePositions[i] = possiblePositions.back();

            possiblePositions.pop_back();
        }
        else
        {
            ++openedWalls;

            subGrid.toggle(possiblePositions[i].first + possibleDirections[j].first / 2,
                           possiblePositions[i].second + possibleDirections[j].second / 2);

            fill_(grid[(possiblePositions[i].first - 1 + possibleDirections[j].first) / 2 * columns
                  + (possiblePositions[i].second - 1 + possibleDirections[j].second) / 2], grid, height, width,
                  possiblePositions[i], possibleDirections[j]);
        }
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::Kruskal::loopGeneration_(URNG &g, SubGrid<bool> const& subGrid,
                                                      size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                      std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const rows(subGrid.rows());
    size_t const columns(subGrid.columns());

    std::vector<std::pair<size_t, size_t> > possiblePositions;

    possiblePositions.reserve(rows * columns);

    for (size_t i(0); i < rows * columns; ++i)
        possiblePositions.push_back(std::make_pair(i / columns * 2 + 1, (i % columns) * 2 + 1));

    boost::disjoint_sets_with_storage<> disjointsSets(rows * columns);

    size_t openedWalls(0);

    while (openedWalls < (height - 1) * (width - 1) / 4 - 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
        if (operationsCycle && cyclePause.count() && !(openedWalls % operationsCycle))
            std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        std::vector<std::pair<int, int> > possibleDirections{std::make_pair(2, 0), std::make_pair(-2, 0),
                                                             std::make_pair(0, 2), std::make_pair(0, -2)};

        size_t const i(g() % possiblePositions.size());

        size_t j(g() % possibleDirections.size());

        auto const element(disjointsSets.find_set((possiblePositions[i].first - 1) / 2 * columns
                                                  + (possiblePositions[i].second - 1) / 2));

        bool b(possiblePositions[i].first + possibleDirections[j].first < height
               && possiblePositions[i].second + possibleDirections[j].second < width
               && element != disjointsSets.find_set((possiblePositions[i].first - 1
                                                     + possibleDirections[j].first) / 2 * columns
                                                    + (possiblePositions[i].second - 1 + possibleDirections[j].second) / 2));

        while (!b)
        {
            possibleDirections[j] = possibleDirections.back();

            possibleDirections.pop_back();

            if (possibleDirections.empty())
                break;

            j = g() % possibleDirections.size();

            b = possiblePositions[i].first + possibleDirections[j].first < height
                && possiblePositions[i].second + possibleDirections[j].second < width
                && element != disjointsSets.find_set((possiblePositions[i].first - 1
                                                      + possibleDirections[j].first) / 2 * columns
                                                     + (possiblePositions[i].second - 1 + possibleDirections[j].second) / 2);
        }

        if (!b)
        {
            possiblePositions[i] = possiblePositions.back();

            possiblePositions.pop_back();
        }
        else
        {
            ++openedWalls;

            subGrid.toggle(possiblePositions[i].first + possibleDirections[j].first / 2,
                           possiblePositions[i].second + possibleDirections[j].second / 2);

            disjointsSets.union_set((possiblePositions[i].first - 1) / 2 * columns + (possiblePositions[i].second - 1) / 2,
                                    (possiblePositions[i].first - 1 + possibleDirections[j].first) / 2 * columns
                                    + (possiblePositions[i].second - 1 + possibleDirections[j].second) / 2);
        }
    }
}

inline void Labyrinth2d::Algorithm::Kruskal::fill_(size_t value, std::vector<size_t>& grid, size_t height, size_t width,
                                                   std::pair<size_t, size_t> const& position,
                                                   std::pair<int, int> const& direction) const
{
    size_t const columns((width - 1) / 2);

    grid[(position.first - 1 + direction.first) / 2 * columns
         + (position.second - 1 + direction.second) / 2] = grid[(position.first - 1) / 2 * columns
                                                                + (position.second - 1) / 2];

    std::array<std::pair<int, int>, 4> constexpr possibleDirections{std::make_pair(2, 0), std::make_pair(-2, 0),
                                                                    std::make_pair(0, 2), std::make_pair(0, -2)};

    for (auto const& d : possibleDirections)
        if (position.first + direction.first + d.first < height
            && position.second + direction.second + d.second < width
            && grid[(position.first - 1 + direction.first + d.first) / 2 * columns
                    + (position.second - 1 + direction.second + d.second) / 2] == value)
            fill_(value, grid, height, width,
                  std::make_pair(position.first + direction.first,
                                 position.second + direction.second), d);
}

#endif // LABYRINTH2D_ALGORITHM_KRUSKAL_H
