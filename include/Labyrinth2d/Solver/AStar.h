#ifndef LABYRINTH2D_SOLVER_ASTAR_H
#define LABYRINTH2D_SOLVER_ASTAR_H

/*!
 *  \file AStar.h
 *  \brief Solve a player in a labyrinth with A* (A-star) algorithm
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <functional>
#include <set>
#include <stack>
#include <stdexcept>
#include <vector>

#include "../Labyrinth.h"
#include "../Player.h"
#include "FailureException.h"
#include "TimeoutException.h"

namespace Labyrinth2d
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
             *  \param sleep: sleep function
             *  \param finishIndex: finish index to reach
             *  \param movements: movements done by the player (0 for a complete resolution if possible)
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort solving
             */
            template <class URNG>
            void operator()(URNG& g, Player& player,
                            std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                            size_t finishIndex = 0, size_t movements = 0, size_t operationsCycle = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth2d::Solver::AStar::operator()(URNG& /*g*/, Player& player,
                                            std::function<void(std::chrono::milliseconds)> const& sleep,
                                            size_t finishIndex, size_t movements,
                                            size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                            std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    struct Node : public std::pair<size_t, size_t>
    {
        double appliedCost;
        double heuristicCost;
        size_t parentNodeIndex;

        Node(size_t i = 0, size_t j = 0, double _appliedCost = 0.0,
             double estimatedCost = 0.0, size_t _parentNodeIndex = -1) : std::pair<size_t, size_t>(i, j), appliedCost(_appliedCost),
                                                                         heuristicCost(0.0), parentNodeIndex(_parentNodeIndex)
        {
            changeEstimatedCost(estimatedCost);
        }

        Node& operator=(std::pair<size_t, size_t> const& pair)
        {
            first = pair.first;
            second = pair.second;

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

        bool operator==(std::pair<size_t, size_t> const& pair) const
        {
            return (first == pair.first && second == pair.second);
        }

        bool operator!=(std::pair<size_t, size_t> const& pair) const
        {
            return !operator==(pair);
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
                                (std::pair<size_t, size_t> const& p1, std::pair<size_t, size_t> const& p2) -> double
                                {
                                    return std::sqrt(std::pow(static_cast<double>(p1.first) - static_cast<double>(p2.first), 2)
                                                     + std::pow(static_cast<double>(p1.second) - static_cast<double>(p2.second), 2));
                                });

    size_t operations(0);

    auto const finish(std::make_pair(player.finishI()[finishIndex],
                                     player.finishJ()[finishIndex]));
    auto const& grid(player.labyrinth().grid());

    std::vector<Node> closedList;
    std::vector<Node> openedList{Node(player.i(), player.j(), 0.0,
                                      calculateEstimatedCost(std::make_pair(player.i(), player.j()), finish))};

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

                if (operationsCycle && cyclePause.count()
                    && !(operations % operationsCycle))
                    sleep(cyclePause);

                if (timeout != nullptr)
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                        throw TimeoutException();

                size_t const i(path.top());

                path.pop();

                ++operations;

                if (closedList[i].first == player.i())
                {
                    if (closedList[i].second > player.j())
                        player.move(Right, sleep);
                    else if (closedList[i].second < player.j())
                        player.move(Left, sleep);
                    else
                        --operations;
                }
                else
                {
                    if (closedList[i].first > player.i())
                        player.move(Down, sleep);
                    else if (closedList[i].first < player.i())
                        player.move(Up, sleep);
                    else
                        --operations;
                }
            }

            return;
        }

        std::vector<Direction> directions{Up, Left, Down, Right};

        while (!directions.empty())
        {
            if (player.state() & Player::StoppedSolving)
                return;

            Node node(currentNode);

            switch (directions.back())
            {
                case Up:
                    --node.first;
                    break;

                case Left:
                    ++node.second;
                    break;

                case Down:
                    ++node.first;
                    break;

                case Right:
                    --node.second;
                    break;
            }

            directions.pop_back();

            if (node.first < grid.height()
                && node.second < grid.width()
                && !grid(node.first, node.second))
            {
                node.changeEstimatedCost(calculateEstimatedCost(node, finish));
                node.parentNodeIndex = closedList.size() - 1;

                auto it(std::find(closedList.begin(), closedList.end(),
                                  static_cast<std::pair<size_t, size_t> >(node)));

                bool ok(it == closedList.end());

                if (!ok)
                    ok = (it->heuristicCost > node.heuristicCost);

                if (ok)
                {
                    it = std::find(openedList.begin(), openedList.end(),
                                   static_cast<std::pair<size_t, size_t> >(node));

                    ok = (it == openedList.end());

                    if (!ok)
                        ok = (it->heuristicCost > node.heuristicCost);

                    if (ok)
                    {
                        std::multiset<Node, CompareNodes> multiset(openedList.begin(),
                                                                   openedList.end());

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
