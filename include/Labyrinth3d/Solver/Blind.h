#ifndef LABYRINTH3D_SOLVER_BLIND_H
#define LABYRINTH3D_SOLVER_BLIND_H

/*!
 *  \file Blind.h
 *  \brief Solve a player in a labyrinth like if the player is blind
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <vector>
#include <thread>
#include <chrono>
#include <map>

#include "../Player.h"
#include "../Labyrinth.h"
#include "FailureException.h"
#include "TimeoutException.h"

namespace Labyrinth3d
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
void Labyrinth3d::Solver::Blind::operator()(URNG& g, Player& player, size_t finishIndex, size_t movements,
                                            size_t operationsCycle, std::chrono::milliseconds cyclePause,
                                            std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    Grid const& grid(player.labyrinth().grid());

    enum CellState
    {
        Visited,
        Unvisited,
        Blocked
    };

    std::vector<CellState> cellStates(grid.height() * grid.width() * grid.depth(), Unvisited);

    cellStates[(player.k() * grid.depth() + player.i()) * grid.width() + player.j()] = Visited;

    std::vector<Direction> directions{Up, Left, Down, Right, Front, Back};
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
        size_t k(player.k());

        switch (chosenDirection)
        {
            case Front:
                --i;
                break;

            case Right:
                ++j;
                break;

            case Back:
                ++i;
                break;

            case Left:
                --j;
                break;

            case Front:
                --k;
                break;

            case Back:
                ++k;
                break;
        }

        if (i < grid.height() && j < grid.width() && k < grid.depth() && !grid(i, j, k))
            break;

        if (directions.empty())
            return;
    }

    size_t previousI(player.i());
    size_t previousJ(player.j());
    size_t previousK(player.k());

    player.move(chosenDirection);

    cellStates[(player.k() * grid.depth() + player.i()) * grid.width() + player.j()] = Visited;

    size_t operations(1);

    std::map<Direction, Direction> const opposedDirections{std::make_pair(Up, Down),
                                                           std::make_pair(Right, Left),
                                                           std::make_pair(Down, Up),
                                                           std::make_pair(Left, Right),
                                                           std::make_pair(Front, Back),
                                                           std::make_pair(Back, Front)};

    while (!(player.state() & Player::Finished) && player.i() != player.finishI()[finishIndex] && player.j() != player.finishJ()[finishIndex] && player.k() != player.finishK()[finishIndex] && (movements || (!movements && operations)))
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

        directions = {Up, Right, Down, Left, Front, Back};

        std::vector<Direction> unvisitedDirections;
        std::vector<Direction> visitedDirections;
        std::vector<Direction> blockedDirections;

        for (size_t l(0); l < directions.size(); ++l)
        {
            if (directions[l] == opposedDirections.at(chosenDirection))
                continue;

            size_t i(player.i());
            size_t j(player.j());
            size_t k(player.k());

            switch (directions[l])
            {
                case Front:
                    --i;
                    break;

                case Right:
                    ++j;
                    break;

                case Back:
                    ++i;
                    break;

                case Left:
                    --j;
                    break;

                case Up:
                    --k;
                    break;

                case Down:
                    --k;
                    break;
            }

            if (i < grid.height() && j < grid.width() && k < grid.depth())
            {
                if (grid(i, j, k))
                    blockedDirections.push_back(directions[l]);
                else
                {
                    switch (cellStates[(k * grid.depth() + i) * grid.width() + j])
                    {
                        case Visited:
                            visitedDirections.push_back(directions[l]);
                            break;

                        case Unvisited:
                            unvisitedDirections.push_back(directions[l]);
                            break;

                        case Blocked:
                            blockedDirections.push_back(directions[l]);
                            break;
                    }
                }
            }
            else
                blockedDirections.push_back(directions[l]);
        }

        if (blockedDirections.size() == 3)
        {
            chosenDirection = opposedDirections.at(chosenDirection);
            cellStates[(player.k() * grid.depth() + player.i()) * grid.width() + player.j()] = Blocked;
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

                    if (cellStates[(previousK * grid.depth() + previousI) * grid.width() + previousJ])
                        cellStates[(player.k() * grid.depth() + player.i()) * grid.width() + player.j()] = Blocked;
                }
            }
            else
            {
                if (unvisitedDirections.empty())
                    chosenDirection = visitedDirections[g() % visitedDirections.size()];
                else
                    chosenDirection = unvisitedDirections[g() % unvisitedDirections.size()];
            }
        }

        previousI = player.i();
        previousJ = player.j();
        previousK = player.k();

        {
            size_t i(player.i());
            size_t j(player.j());
            size_t k(player.k());

            switch (chosenDirection)
            {
                case Front:
                    --i;
                    break;

                case Right:
                    ++j;
                    break;

                case Back:
                    ++i;
                    break;

                case Left:
                    --j;
                    break;

                case Up:
                    --k;
                    break;

                case Down:
                    ++k;
                    break;
            }

            if (cellStates[(k * grid.depth() + i) * grid.width() + j] == Blocked)
                throw FailureException();
        }

        player.move(chosenDirection);

        cellStates[(player.k() * grid.depth() + player.i()) * grid.width() + player.j()] = Visited;

        ++operations;
    }
}

#endif // LABYRINTH3D_SOLVER_BLIND_H
