#ifndef LABYRINTH2D_SOLVER_WALLHAND_H
#define LABYRINTH2D_SOLVER_WALLHAND_H

/*!
 *  \file WallHand.h
 *  \brief Solve a player in a labyrinth by follow a wall with his hand
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <algorithm>
#include <chrono>
#include <functional>
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
         *  \brief Solve a player in a labyrinth by follow a wall with his hand
         *
         *  The player continue by following a wall with one of his hand. If he
         *  reaches again his start position (at the begin of the resolution)
         *  with the same wall to follow, the resolution fails.
         */
        struct WallHand
        {
            enum Direction
            {
                Left,
                Right
            };

            Direction direction;

            /*!
             *  \brief Constructor
             *
             *  \param _direction: direction of hand
             */
            WallHand(Direction _direction = Right);

            /*!
             *  \brief Solve the player
             *
             *  \param g: uniform random number generator
             *  \param player: player to solve
             *  \param sleep: sleep function
             *  \param finishIndex: finish index to reach
             *  \param movements: movements done by the player (0 for a complete resolution if possible)
             *  \param cycleOperations: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort solving
             */
            template <class URNG>
            void operator()(URNG& g, Player& player,
                            std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                            size_t finishIndex = 0, size_t movements = 0, size_t cycleOperations = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth2d::Solver::WallHand::operator()(URNG& g, Player& player, std::function<void(std::chrono::milliseconds)> const& sleep, size_t finishIndex, size_t movements,
                                               size_t cycleOperations, std::chrono::milliseconds const& cyclePause,
                                               std::chrono::milliseconds const* timeout)
{
    auto const t(std::chrono::steady_clock::now());

    auto const& grid(player.labyrinth().grid());

    if (player.state() & Player::Finished)
        return;

    std::vector<Labyrinth2d::Direction> directions{Labyrinth2d::Up, Labyrinth2d::Right,
                                                   Labyrinth2d::Down, Labyrinth2d::Left};
    Labyrinth2d::Direction chosenDirection;

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

    size_t const startI(player.i());
    size_t const startJ(player.j());
    auto const directionStart(chosenDirection);

    player.move(chosenDirection, sleep);

    size_t operations(1);

    while (!(player.state() & Player::Finished) && !(player.i() == player.finishI()[finishIndex] && player.j() == player.finishJ()[finishIndex]) && (!movements || (operations < movements)))
    {
        if (player.state() & Player::StoppedSolving)
            return;

        if (cycleOperations && cyclePause.count() && !(operations % cycleOperations))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        directions.clear();

        switch (chosenDirection)
        {
            case Labyrinth2d::Up:
                directions = {Labyrinth2d::Right, Labyrinth2d::Up, Labyrinth2d::Left, Labyrinth2d::Down};
                break;

            case Labyrinth2d::Right:
                directions = {Labyrinth2d::Down, Labyrinth2d::Right, Labyrinth2d::Up, Labyrinth2d::Left};
                break;

            case Labyrinth2d::Down:
                directions = {Labyrinth2d::Left, Labyrinth2d::Down, Labyrinth2d::Right, Labyrinth2d::Up};
                break;

            case Labyrinth2d::Left:
                directions = {Labyrinth2d::Up, Labyrinth2d::Left, Labyrinth2d::Down, Labyrinth2d::Right};
                break;
        }

        if (direction == Left)
            std::swap(directions[0], directions[2]);

        for (size_t k(0); k < directions.size(); ++k)
        {
            size_t i(player.i());
            size_t j(player.j());

            switch (directions[k])
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
            {
                if (player.i() == startI && player.j() == startJ && directions[k] == directionStart)
                    throw FailureException();

                player.move(directions[k], sleep);

                chosenDirection = directions[k];

                ++operations;

                break;
            }
        }
    }
}

#endif // LABYRINTH2D_SOLVER_WALLHAND_H
