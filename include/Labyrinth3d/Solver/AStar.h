#ifndef LABYRINTH2D_SOLVER_ASTAR_H
#define LABYRINTH2D_SOLVER_ASTAR_H

/*!
 *  \file AStar.h
 *  \brief Solve a player in a labyrinth with A* (A-star) algorithm
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <vector>
#include <thread>
#include <chrono>
#include <set>
#include <stack>
#include <stdexcept>
#include <tuple>

#include "../Player.h"
#include "../Labyrinth.h"
#include "FailureException.h"
#include "TimeoutException.h"

namespace Labyrinth3d
{
    namespace Solver
    {
        /*!
         *  \brief Solve a player in a labyrinth with A* (A-star) algorithm
         *
         *  The algorithm finds a way (not necessary the quickest) to reach
         *  the player exit (except if the player can not be solved)
         */
        struct AStar
        {
            /*!
             *  \brief Solve the player
             *
             *  \param g: uniform random number generator
             *  \param player: player to solve
             *  \param finishIndex: finish index to reach
             *  \param movements: movements done by the player (0 for a complete resolution if possible)
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort solving
             */
            template <class URNG>
            void operator()(URNG& g, Player& player, size_t finishIndex = 0, size_t movements = 0, size_t operationsCycle = 0,
                            std::chrono::milliseconds cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth3d::Solver::AStar::operator()(URNG&, Player& player, size_t finishIndex, size_t movements,
                                            size_t operationsCycle, std::chrono::milliseconds cyclePause,
                                            std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    struct Node : public std::tuple<size_t, size_t, size_t>
    {
        double appliedCost;
        double heuristicCost;
        size_t parentNodeIndex;

        Node(size_t i = 0, size_t j = 0, size_t k = 0, double _appliedCost = 0.0,
             double estimatedCost = 0.0, size_t _parentNodeIndex = -1) : std::tuple<size_t, size_t, size_t>(i, j, k),
                                                                         appliedCost(_appliedCost),
                                                                         heuristicCost(0.0), parentNodeIndex(_parentNodeIndex)
        {
            changeEstimatedCost(estimatedCost);
        }

        Node& operator=(std::tuple<size_t, size_t, size_t> const& tuple)
        {
            for (size_t i(0); i < 3; ++i)
                std::get<i>(*this) = std::get<i>(tuple);

            return *this;
        }

        double estimatedCost() const
        {
            return heuristicCost - appliedCost;
        }

        void changeEstimatedCost(double estimatedCost)
        {
            heuristicCost = appliedCost + estimatedCost;
        }

        bool operator==(std::tuple<size_t, size_t, size_t> const& tuple) const
        {
            for (size_t i(0); i < 3; ++i)
                if (std::get<i>(*this) != std::get<i>(tuple))
                    return false;

            return true;
        }

        bool operator!=(std::tuple<size_t, size_t, size_t> const& tuple) const
        {
            return !operator==(tuple);
        }
    };

    struct CompareNodes
    {
        bool operator()(Node const& n1, Node const& n2) const
        {
            return (n1.heuristicCost < n2.heuristicCost);
        }
    };

    auto calculateEstimatedCost([]
                                (std::tuple<size_t, size_t, size_t> const& t1, std::tuple<size_t, size_t, size_t> const& t2)
                                {
                                    return std::sqrt(std::pow(static_cast<double>(std::get<0>(t1)) - static_cast<double>(std::get<0>(t2)), 2)
                                                     + std::pow(static_cast<double>(std::get<1>(t1)) - static_cast<double>(std::get<1>(t2)), 2)
                                                     + std::pow(static_cast<double>(std::get<2>(t1)) - static_cast<double>(std::get<2>(t2)), 2));
                                });

    size_t operations(0);

    auto const finish(std::make_tuple(player.finishI(), player.finishJ(), player.finishK()));
    Grid const& grid(player.labyrinth().grid());

    std::vector<Node> closedList;
    std::vector<Node> openedList{Node(player.i(), player.j(), player.k(), 0.0,
                                      calculateEstimatedCost(std::make_pair(player.i(), player.j(), player.k()), finish))};

    while (!openedList.empty())
    {
        if (player.state() & Player::StoppedSolving)
            return;

        Node currentNode(openedList.back());

        openedList.pop_back();

        closedList.push_back(currentNode);

        if (currentNode == finish)
        {
            std::stack<size_t> path;

            path.push(closedList.size() - 1);

            while (static_cast<int>(currentNode.parentNodeIndex) != -1)
            {
                if (player.state() & Player::StoppedSolving)
                    return;

                path.push(currentNode.parentNodeIndex);

                currentNode = closedList[currentNode.parentNodeIndex];
            }

            path.pop();

            while (!path.empty() && (!movements || operations < movements))
            {
                if (player.state() & Player::StoppedSolving)
                    return;

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
                if (operationsCycle && cyclePause.count() && !(operations % operationsCycle))
                    std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

                if (timeout != nullptr)
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                        throw TimeoutException();

                size_t const i(path.top());

                path.pop();

                ++operations;

                if (std::get<0>(closedList[i]) == player.i())
                {
                    if (std::get<2>(closedList[i]) == player.k())
                    {
                        if (std::get<1>(closedList[i]) > player.j())
                            player.move(Right);
                        else if (std::get<1>(closedList[i]) < player.j())
                            player.move(Left);
                        else
                            --operations;
                    }
                    else
                    {
                        assert(std::get<1>(closedList[i]) == player.j());

                        if (std::get<2>(closedList[i]) > player.k())
                            player.move(Up);
                        else if (std::get<2>(closedList[i]) < player.k())
                            player.move(Down);
                        else
                            --operations;
                    }
                }
                else
                {
                    assert(std::get<1>(closedList[i]) == player.j() && std::get<2>(closedList[i]) == player.k());

                    if (std::get<0>(closedList[i]) > player.i())
                        player.move(Back);
                    else if (std::get<0>(closedList[i]) < player.i())
                        player.move(Front);
                    else
                        --operations;
                }
            }

            return;
        }

        std::vector<Direction> directions{Up, Left, Down, Right, Front, Back};

        while (!directions.empty())
        {
            if (player.state() & Player::StoppedSolving)
                return;

            Node node(currentNode);

            switch (directions.back())
            {
                case Front:
                    --std::get<0>(node);
                    break;

                case Left:
                    ++std::get<1>(node);
                    break;

                case Back:
                    ++std::get<0>(node);
                    break;

                case Right:
                    --std::get<1>(node);
                    break;

                case Up:
                    --std::get<2>(node);
                    break;

                case Down:
                    ++std::get<2>(node);
                    break;
            }

            directions.pop_back();

            if (std::get<0>(node) < grid.height() && std::get<1>(node) < grid.width() && std::get<2>(node) < grid.depth()
                && !grid(std::get<0>(node), std::get<1>(node), std::get<2>(node)))
            {
                node.changeEstimatedCost(calculateEstimatedCost(node, finish));
                node.parentNodeIndex = closedList.size() - 1;

                auto it(std::find(closedList.begin(), closedList.end(), static_cast<std::tuple<size_t, size_t, size_t> >(node)));

                bool ok(it == closedList.end());

                if (!ok)
                    ok = (it->heuristicCost > node.heuristicCost);

                if (ok)
                {
                    it = std::find(openedList.begin(), openedList.end(), static_cast<std::tuple<size_t, size_t, size_t> >(node));

                    ok = (it == openedList.end());

                    if (!ok)
                        ok = (it->heuristicCost > node.heuristicCost);

                    if (ok)
                    {
                        std::multiset<Node, CompareNodes> multiset(openedList.begin(), openedList.end());

                        multiset.insert(node);

                        openedList = std::vector<Node>(multiset.begin(), multiset.end());
                    }
                }
            }
        }
    }

    throw FailureException();
}

#endif // LABYRINTH2D_SOLVER_ASTAR_H
