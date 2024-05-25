#ifndef LABYRINTH3D_ALGORITHM_WAYSEARCH_H
#define LABYRINTH3D_ALGORITHM_WAYSEARCH_H

/*!
 *  \file WaySearch.h
 *  \brief Generate a perfect labyrinth by randomly searching a way
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
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
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort generation
             */
            template <class URNG>
            void operator()(URNG& g, SubGrid<bool> const& subGrid,
                            std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                            size_t operationsCycle = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth3d::Algorithm::WaySearch::operator()(URNG& g, SubGrid<bool> const& subGrid,
                                                   std::function<void(std::chrono::milliseconds)> const& sleep,
                                                   size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                   std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const floors(subGrid.floors());
    size_t const rows(subGrid.rows());
    size_t const columns(subGrid.columns());
    size_t const depth(subGrid.depth());

    for (size_t k(1); k < depth; k += 2)
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

    for (size_t i(1); i < height; i += 2)
        for (size_t k(2); k < depth - 1; k += 2)
            for (size_t j(1); j < height - 1; j += 2)
                subGrid.set(i, j, k);

    class Location
    {
        public:
            enum Operation
            {
                Oppose,
                SwapIj,
                SwapAndOpposeIj,
                SwapIk,
                SwapAndOpposeIk,
                SwapJk,
                SwapAndOpposeJk,
                RotateToLeft,
                RotateToLeftAndOppose,
                RotateToRight,
                RotateToRightAndOppose,
            };

            Location(size_t startI, size_t startJ, size_t startK,
                     size_t randomNumber) : startI_(startI), startJ_(startJ), startK_(startK),
                                            dStartI_(2), dStartJ_(0), dStartK_(0),
                                            di_(dStartI_), dj_(dStartJ_), dk_(dStartK_),
                                            operations_{Oppose,
                                                        SwapIj, SwapAndOpposeIj,
                                                        SwapIk, SwapAndOpposeIk,
                                                        SwapJk, SwapAndOpposeJk,
                                                        RotateToLeft, RotateToLeftAndOppose,
                                                        RotateToRight, RotateToRightAndOppose}
            {
                std::array<std::tuple<int, int, int>, 6> constexpr d{std::make_tuple(2, 0, 0), std::make_tuple(-2, 0, 0),
                                                                     std::make_tuple(0, 2, 0), std::make_tuple(0, -2, 0),
                                                                     std::make_tuple(0, 0, 2), std::make_tuple(0, 0, -2)};

                dStartI_ = std::get<0>(d[randomNumber % d.size()]);
                dStartJ_ = std::get<1>(d[randomNumber % d.size()]);
                dStartK_ = std::get<2>(d[randomNumber % d.size()]);

                di_ = dStartI_;
                dj_ = dStartJ_;
                dk_ = dStartK_;
            }

            void change(size_t randomNumber)
            {
                if (operations_.empty())
                    return;

                size_t const i(randomNumber % operations_.size());

                switch (operations_[i])
                {
                    case Oppose:
                        di_ = -dStartI_;
                        dj_ = -dStartJ_;
                        dk_ = -dStartK_;
                        break;

                    case SwapIj:
                        di_ = dStartJ_;
                        dj_ = dStartI_;
                        dk_ = dStartK_;
                        break;

                    case SwapAndOpposeIj:
                        di_ = -dStartJ_;
                        dj_ = -dStartI_;
                        dk_ = dStartK_;
                        break;

                    case SwapIk:
                        di_ = dStartK_;
                        dj_ = dStartJ_;
                        dk_ = dStartI_;
                        break;

                    case SwapAndOpposeIk:
                        di_ = -dStartK_;
                        dj_ = dStartJ_;
                        dk_ = -dStartI_;
                        break;

                    case SwapJk:
                        di_ = dStartI_;
                        dj_ = dStartK_;
                        dk_ = dStartJ_;
                        break;

                    case SwapAndOpposeJk:
                        di_ = dStartI_;
                        dj_ = -dStartK_;
                        dk_ = -dStartJ_;
                        break;

                    case RotateToLeft:
                        di_ = dStartJ_;
                        dj_ = dStartK_;
                        dk_ = dStartI_;
                        break;

                    case RotateToLeftAndOppose:
                        di_ = -dStartJ_;
                        dj_ = -dStartK_;
                        dk_ = -dStartI_;
                        break;

                    case RotateToRight:
                        di_ = dStartK_;
                        dj_ = dStartI_;
                        dk_ = dStartJ_;
                        break;

                    case RotateToRightAndOppose:
                        di_ = -dStartK_;
                        dj_ = -dStartI_;
                        dk_ = -dStartJ_;
                        break;
                }

                operations_[i] = operations_.back();
                operations_.pop_back();
            }

            bool spent() const
            {
                return operations_.empty();
            }

            size_t startI() const
            {
                return startI_;
            }

            size_t startJ() const
            {
                return startJ_;
            }

            size_t startK() const
            {
                return startK_;
            }

            int dStartI() const
            {
                return dStartI_;
            }

            int dStartJ() const
            {
                return dStartJ_;
            }

            int dStartK() const
            {
                return dStartK_;
            }

            int di() const
            {
                return di_;
            }

            int dj() const
            {
                return dj_;
            }

            int dk() const
            {
                return dk_;
            }

            size_t i() const
            {
                return startI_ + di_;
            }

            size_t j() const
            {
                return startJ_ + dj_;
            }

            size_t k() const
            {
                return startK_ + dk_;
            }

        private:
            size_t startI_;
            size_t startJ_;
            size_t startK_;
            int dStartI_;
            int dStartJ_;
            int dStartK_;
            int di_;
            int dj_;
            int dk_;
            std::vector<Operation> operations_;
    };

    std::vector<Location> history{Location(2 * (g() % rows)+ 1, 2 * (g() % columns) + 1,  2 * (g() % floors) + 1, g())};

    std::vector<bool> visitedCells(rows * columns * floors, false);

    visitedCells[((history.back().startK() - 1) * rows + history.back().startI() - 1) / 2 * columns
                 + (history.back().startJ() - 1) / 2] = true;

    size_t openedWalls(0);
    bool updatedHistory(false);

    while (openedWalls < (height - 1) * (width - 1) * (depth - 1) / 8 - 1)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (operationsCycle && cyclePause.count() && !(openedWalls % operationsCycle))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        size_t i(0);

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

        if (history[i].i() < height && history[i].j() < width && history[i].k() < depth
            && !visitedCells[((history[i].k() - 1) * rows + history[i].i() - 1) / 2 * columns + (history[i].j() - 1) / 2])
        {
            ++openedWalls;

            subGrid.toggle(history[i].startI() + history[i].di() / 2,
                           history[i].startJ() + history[i].dj() / 2,
                           history[i].startK() + history[i].dk() / 2);

            visitedCells[((history[i].k() - 1) * rows + history[i].i() - 1) / 2 * columns + (history[i].j() - 1) / 2] = true;

            history.push_back(Location(history[i].i(), history[i].j(), history[i].k(), g()));
        }

        if (!history[i].spent())
            history[i].change(g());
        else
        {
            history[i] = history.back();
            history.pop_back();
            updatedHistory = true;
        }
    }
}

#endif // LABYRINTH3D_ALGORITHM_WAYSEARCH_H
