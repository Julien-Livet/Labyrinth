#ifndef LABYRINTH2D_ALGORITHM_WAYSEARCH_H
#define LABYRINTH2D_ALGORITHM_WAYSEARCH_H

/*!
 *  \file WaySearch.h
 *  \brief Generate a perfect labyrinth by randomly searching a way
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <algorithm>
#include <chrono>
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
         *  A random start with a random direction is chosen. A cell is chosen along
         *  the established way with a random possible direction. A wall is opened
         *  between the two cells.
         */
        struct WaySearch
        {
            /*!
             *  \brief DepthFirstSearch will choose the last possible intersection
             *         in the stack, Prim will choose the first possible intersection
             *         and HuntAndKill will choose a random possible intersection.
             */
            enum Type
            {
                DepthFirstSearch,
                Prim,
                HuntAndKill
            };

            Type type;

            /*!
             *  \brief Constructor
             *
             *  \param type_: type used in each step of the algorithm
             */
            WaySearch(Type type_);

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
void Labyrinth2d::Algorithm::WaySearch::operator()(URNG& g, SubGrid const& subGrid,
                                                   std::function<void(std::chrono::milliseconds)> const& sleep,
                                                   size_t cycleOperations, std::chrono::milliseconds const& cyclePause,
                                                   std::chrono::milliseconds const* timeout)
{
    auto const t{std::chrono::steady_clock::now()};

    auto const height{subGrid.height()};
    auto const width{subGrid.width()};
    auto const rows{subGrid.rows()};
    auto const columns{subGrid.columns()};

    for (size_t i{1}; i < height - 3; i += 2)
    {
        for (size_t j{2}; j < width - 1; j += 2)
        {
            subGrid.set(i, j);
            subGrid.set(i + 1, j - 1);
        }
    }

    for (size_t i{2}; i < height - 1; i += 2)
        subGrid.set(i, width - 2);

    for (size_t j{2}; j < width - 1; j += 2)
        subGrid.set(height - 2, j);

    class Location
    {
        public:
            Location(size_t startI, size_t startJ) : startI_{startI}, startJ_{startJ},
                                                     directions_{std::make_pair(2, 0), std::make_pair(-2, 0),
                                                                 std::make_pair(0, 2), std::make_pair(0, -2)}
            {
            }

            void change()
            {
                if (directions_.empty())
                    return;

                directions_.pop_back();
            }

            bool spent() const
            {
                return directions_.empty();
            }

            size_t startI() const
            {
                return startI_;
            }

            size_t startJ() const
            {
                return startJ_;
            }

            int di() const
            {
                if (directions_.empty())
                    return 0;

                return directions_.back().first;
            }

            int dj() const
            {
                if (directions_.empty())
                    return 0;

                return directions_.back().second;
            }

            size_t i() const
            {
                return startI_ + di();
            }

            size_t j() const
            {
                return startJ_ + dj();
            }

            std::vector<std::pair<int, int> >::iterator begin()
            {
                return directions_.begin();
            }

            std::vector<std::pair<int, int> >::iterator end()
            {
                return directions_.end();
            }

        private:
            size_t startI_;
            size_t startJ_;
            std::vector<std::pair<int, int> > directions_;
    };

    std::vector<Location> history{Location{2 * (g() % rows)+ 1, 2 * (g() % columns) + 1}};
    std::shuffle(history.back().begin(), history.back().end(), g);

    std::vector<bool> visitedCells(rows * columns, false);

    visitedCells[(history.back().startI() - 1) / 2 * columns + (history.back().startJ() - 1) / 2] = true;

    size_t openedWalls{0};
    bool updatedHistory{false};

    while (openedWalls < (height - 1) * (width - 1) / 4 - 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (cycleOperations && cyclePause.count() && !(openedWalls % cycleOperations))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        size_t i{0};

        if (type == DepthFirstSearch)
            i = history.size() - 1;
        else if (type == Prim)
            i = g() % history.size();
        else// if (type == HuntAndKill)
        {
            if (updatedHistory)
            {
                i = g() % history.size();

                updatedHistory = false;
            }
            else
                i = history.size() - 1;
        }

        if (history[i].i() < height && history[i].j() < width
            && !visitedCells[(history[i].i() - 1) / 2 * columns + (history[i].j() - 1) / 2])
        {
            ++openedWalls;

            subGrid.toggle(history[i].startI() + history[i].di() / 2, history[i].startJ() + history[i].dj() / 2);

            visitedCells[(history[i].i() - 1) / 2 * columns + (history[i].j() - 1) / 2] = true;

            history.push_back(Location{history[i].i(), history[i].j()});
            std::shuffle(history.back().begin(), history.back().end(), g);
        }

        if (!history[i].spent())
            history[i].change();
        else
        {
            history[i] = history.back();
            history.pop_back();
            updatedHistory = true;
        }
    }
}

#endif // LABYRINTH2D_ALGORITHM_WAYSEARCH_H
