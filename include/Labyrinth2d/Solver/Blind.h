#ifndef LABYRINTH2D_SOLVER_BLIND_H
#define LABYRINTH2D_SOLVER_BLIND_H

/*!
 *  \file Blind.h
 *  \brief Solve a player in a labyrinth like if the player is blind
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <functional>
#include <map>
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
         *  \brief Solve a player in a labyrinth like if the player is blind
         *
         *  In each step, a random direction is chosen among not yet visited.
         *  If the player is blocked, the player go back by marking his way as
         *  blocked and if all directions at an intersection are visited, a
         *  random direction is chosen. If all ways ar blocked, the resolution
         *  fails.
         */
        struct Blind
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
void Labyrinth2d::Solver::Blind::operator()(URNG& g, Player& player,
                                            std::function<void(std::chrono::milliseconds)> const& sleep,
                                            size_t finishIndex, size_t movements,
                                            size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                            std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    auto const& grid(player.labyrinth().grid());

    enum CellState
    {
        Visited,
        Unvisited,
        Blocked
    };

    std::vector<CellState> cellStates(grid.height() * grid.width(), Unvisited);

    cellStates[player.i() * grid.width() + player.j()] = Visited;

    std::vector<Direction> directions{Up, Left, Down, Right};
    Direction chosenDirection;

    while (1)
    {
        if (player.state() & Player::StoppedSolving)
            return;

        size_t const k(g() % directions.size());

        chosenDirection = directions[k];

        directions[k] = directions.back();

        directions.pop_back();

        size_t i(player.i());
        size_t j(player.j());

        switch (chosenDirection)
        {
            case Labyrinth2d::Up:
                --i;
                break;

            case Labyrinth2d::Right:
                ++j;
                break;

            case Labyrinth2d::Down:
                ++i;
                break;

            case Labyrinth2d::Left:
                --j;
                break;
        }

        if (i < grid.height() && j < grid.width() && !grid(i, j))
            break;

        if (directions.empty())
            return;
    }

    size_t previousI(player.i());
    size_t previousJ(player.j());

    player.move(chosenDirection, sleep);

    cellStates[player.i() * grid.width() + player.j()] = Visited;

    size_t operations(1);

    std::map<Direction, Direction> const opposedDirections{std::make_pair(Up, Down),
                                                           std::make_pair(Right, Left),
                                                           std::make_pair(Down, Up),
                                                           std::make_pair(Left, Right)};

    while (!(player.state() & Player::Finished) && !(player.i() == player.finishI()[finishIndex] && player.j() == player.finishJ()[finishIndex]) && (!movements || (operations < movements)))
    {
        if (player.state() & Player::StoppedSolving)
            return;

        if (operationsCycle && cyclePause.count() && !(operations % operationsCycle))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        directions = {Up, Right, Down, Left};

        std::vector<Direction> unvisitedDirections;
        std::vector<Direction> visitedDirections;
        std::vector<Direction> blockedDirections;

        for (size_t k(0); k < directions.size(); ++k)
        {
            if (directions[k] == opposedDirections.at(chosenDirection))
                continue;

            size_t i(player.i());
            size_t j(player.j());

            switch (directions[k])
            {
                case Up:
                    --i;
                    break;

                case Right:
                    ++j;
                    break;

                case Down:
                    ++i;
                    break;

                case Left:
                    --j;
                    break;
            }

            if (i < grid.height() && j < grid.width())
            {
                if (grid(i, j))
                    blockedDirections.push_back(directions[k]);
                else
                {
                    switch (cellStates[i * grid.width() + j])
                    {
                        case Visited:
                            visitedDirections.push_back(directions[k]);
                            break;

                        case Unvisited:
                            unvisitedDirections.push_back(directions[k]);
                            break;

                        case Blocked:
                            blockedDirections.push_back(directions[k]);
                            break;
                    }
                }
            }
            else
                blockedDirections.push_back(directions[k]);
        }

        if (blockedDirections.size() == 3)
        {
            chosenDirection = opposedDirections.at(chosenDirection);
            cellStates[player.i() * grid.width() + player.j()] = Blocked;
        }
        else
        {
            if (blockedDirections.size() == 2)
            {
                if (!unvisitedDirections.empty())
                    chosenDirection = unvisitedDirections.back();
                else
                {
                    chosenDirection = visitedDirections.back();

                    if (cellStates[previousI * grid.width() + previousJ])
                        cellStates[player.i() * grid.width() + player.j()] = Blocked;
                }
            }
            else
            {
                if (unvisitedDirections.empty())
                {
                    if (!visitedDirections.empty())
                        chosenDirection = visitedDirections[g() % visitedDirections.size()];
                }
                else
                    chosenDirection = unvisitedDirections[g() % unvisitedDirections.size()];
            }
        }

        previousI = player.i();
        previousJ = player.j();

        {
            size_t i(player.i());
            size_t j(player.j());

            switch (chosenDirection)
            {
                case Up:
                    --i;
                    break;

                case Right:
                    ++j;
                    break;

                case Down:
                    ++i;
                    break;

                case Left:
                    --j;
                    break;
            }

            if (cellStates[i * grid.width() + j] == Blocked)
                throw FailureException();
        }

        player.move(chosenDirection, sleep);

        cellStates[player.i() * grid.width() + player.j()] = Visited;

        ++operations;
    }
}

#endif // LABYRINTH2D_SOLVER_BLIND_H
