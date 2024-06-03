#ifndef LABYRINTH3D_PLAYER_H
#define LABYRINTH3D_PLAYER_H

/*!
 *  \file Player.h
 *  \brief Represent a player in a labyrinth
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <functional>
#include <tuple>
#include <vector>

#include "Grid.h"
#include "utility.h"

namespace Labyrinth3d
{
    class Labyrinth;

    /*!
     *  \brief Represent a player in a labyrinth
     */
    class Player
    {
        public:
            /*!
             *  \brief Player state
             */
            enum State
            {
                Started = 0x1,
                Finished = 0x2,
                Solving = 0x4,
                StoppedSolving = 0x8,
                LaunchedSolving = 0x10,
                Solved = 0x20,
                Moving = 0x40
            };

            /*!
             *  \brief Destructor
             */
            ~Player();

            /*!
             *  \return The start index in [0; height() - 1] in the labyrinth grid
             */
            size_t startI() const;

            /*!
             *  \return The start index in [0; width() - 1] in the labyrinth grid
             */
            size_t startJ() const;

            /*!
             *  \return The start index in [0; depth() - 1] in the labyrinth grid
             */
            size_t startK() const;

            /*!
             *  \return The finish indexes in [0; height() - 1] in the labyrinth grid
             */
            std::vector<size_t> const& finishI() const;

            /*!
             *  \return The finish indexes in [0; width() - 1] in the labyrinth grid
             */
            std::vector<size_t> const& finishJ() const;

            /*!
             *  \return The finish indexes in [0; depth() - 1] in the labyrinth grid
             */
            std::vector<size_t> const& finishK() const;

            /*!
             *  \return The current position index in [0; height() - 1] in the labyrinth grid
             */
            size_t i() const;

            /*!
             *  \return The current position index in [0; width() - 1] in the labyrinth grid
             */
            size_t j() const;

            /*!
             *  \return The current position index in [0; depth() - 1] in the labyrinth grid
             */
            size_t k() const;

            /*!
             *  \return The start row in [0; rows() - 1] in the labyrinth grid
             */
            size_t startRow() const;

            /*!
             *  \return The start column in [0; column() - 1] in the labyrinth grid
             */
            size_t startColumn() const;

            /*!
             *  \return The start floor in [0; floors() - 1] in the labyrinth grid
             */
            size_t startFloor() const;

            /*!
             *  \return The finish row in [0; rows() - 1] in the labyrinth grid
             */
            std::vector<size_t> finishRows() const;

            /*!
             *  \return The finish columns in [0; column() - 1] in the labyrinth grid
             */
            std::vector<size_t> finishColumns() const;

            /*!
             *  \return The finish floors in [0; column() - 1] in the labyrinth grid
             */
            std::vector<size_t> finishFloors() const;

            /*!
             *  \return The current position row in [0; rows() - 1] in the labyrinth grid
             */
            size_t row() const;

            /*!
             *  \return The current position column in [0; columns() - 1] in the labyrinth grid
             */
            size_t column() const;

            /*!
             *  \return The current position floor in [0; floors() - 1] in the labyrinth grid
             */
            size_t floor() const;

            /*!
             *  \return The accomplished movements
             */
            size_t movements() const;

            /*!
             *  \return The finishing duration
             */
            std::chrono::milliseconds const& finishingDuration() const;

            /*!
             *  \return The solving duration
             */
            std::chrono::milliseconds const& solvingDuration() const;

            /*!
             *  \brief Move in a direction
             *
             *  \param direction: the movement direction
             *  \param sleep: sleep function
             *  \param movements: the number of movements in this direction
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *
             *  \return The accomplished movements in the considered direction
             */
            size_t move(Direction direction,
                        std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                        size_t movements = 1, size_t operationsCycle = 0,
                        std::chrono::milliseconds cyclePause = std::chrono::milliseconds(0));

            /*!
             *  \return The player state
             */
            size_t state() const;

            /*!
             *  \return A boolean which is true if the player keep a trace of his path
             */
            bool enabledTrace() const;

            /*!
             *  \brief Enable the trace, only possible until the player has started
             *
             *  \param enabledTrace: a boolean which is true if the player keep a trace of his path
             */
            void enableTrace(bool enabledTrace);

            /*!
             *  \return A boolean which is true if the player keep a full trace of his path (included unsuccesfull path)
             */
            bool keptFullTrace() const;
			
            /*!
             *  \brief Keep the full trace (included unsuccesfull path), only possible until the player has started
             *
             *  \param keptFullTrace: a boolean which is true if the player keep a full trace of his path
             */
            void keepFullTrace(bool keptFullTrace);

            /*!
             *  \return A vector of trace intersections (indices in [0; heigth() - 1] x [0; width() - 1] x [0; depth() - 1])
             */
            std::vector<std::tuple<size_t, size_t, size_t> > const& traceIntersections() const;

            /*!
             *  \brief Start from scratch
             *
             *  \return A boolean which is true if the player has been restarted
             */
            bool restart();

            /*!
             *   \return A constant reference to the labyrinth
             */
            Labyrinth const& labyrinth() const;

            /*!
             *  \return A boolean which is true if the player can not move when he reaches his finish
             */
            bool blockingFinish() const;

            /*!
             *  \brief Block the player at the finish, only possible until the player has started
             *
             *  \param blockingFinish: a boolean which is true if the player can not move when he reaches his finish
             */
            void changeBlockingFinish(bool blockingFinish);

            /*!
             *  \brief Step back the player if trace is enabled
             *
             *  \param sleep: sleep function
             *  \param movements: the number of movements executed
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *
             *  \return Effective step back movements
             */
            size_t stepBack(std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                            size_t movements = 1, size_t operationsCycle = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0));

            /*!
             *  \brief Solve the player in his labyrinth by finding his finish
             *
             *  \param g: uniform random number generator
             *  \param solver: solver which moves the player to reach his finish
             *  \param sleep: sleep function
             *  \param finishIndex: finish index to reach
             *  \param movements: the number of movements executed by the solver
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort solving
             *
             *  \return A boolean which is true if the player has reached his finish
             */
            template <class URG, class Solver>
            bool solve(URG& g, Solver& solver,
                       std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                       size_t finishIndex = 0, size_t movements = 0,
                       size_t operationsCycle = 0, std::chrono::milliseconds cyclePause = std::chrono::milliseconds(0),
                       std::chrono::milliseconds const* timeout = nullptr);

            /*!
             *  \brief Stop solving player
             */
            void stopSolving();
			
			/*!
             *  \brief Return the full trace as a vector of tuples
			 */
            std::vector<std::tuple<size_t, size_t, size_t> > const& fullTrace() const;

        private:
            size_t startI_;
            size_t startJ_;
            size_t startK_;
            std::vector<size_t> finishI_;
            std::vector<size_t> finishJ_;
            std::vector<size_t> finishK_;
            size_t i_;
            size_t j_;
            size_t k_;
            Labyrinth const& labyrinth_;
            size_t movements_;
            std::chrono::milliseconds finishingDuration_;
            std::chrono::milliseconds solvingDuration_;
            size_t state_;
            std::chrono::time_point<std::chrono::steady_clock> startTime_;
            bool enabledTrace_;
            std::vector<std::tuple<size_t, size_t, size_t> > traceIntersections_;
            bool blockingFinish_;
			bool keptFullTrace_;
            std::vector<std::tuple<size_t, size_t, size_t> > fullTrace_;

            friend class Labyrinth;

            Player(Labyrinth const& labyrinth,
                   size_t startI, size_t startJ, size_t startK,
                   std::vector<size_t> const& finishI, std::vector<size_t> const& finishJ, std::vector<size_t> const& finishK,
                   bool enabledTrace = false, bool blockingFinish = true, bool keptFullTrace = false);
            Player(Player const&) = delete;
            Player& operator=(Player const&) = delete;
    };
}

#include "Labyrinth.h"
#include "Solver/TimeoutException.h"

template <class URG, class Solver>
bool Labyrinth3d::Player::solve(URG& g, Solver& solver,
                                std::function<void(std::chrono::milliseconds)> const& sleep,
                                size_t finishIndex, size_t movements,
                                size_t operationsCycle, std::chrono::milliseconds cyclePause,
                                std::chrono::milliseconds const* timeout)
{
    if ((state_ & Solving) || (state_ & Moving))
        return false;

    if (state_ & Solved)
        return true;

    state_ |= Solving;
    state_ |= LaunchedSolving;
    state_ &= ~StoppedSolving;

    while (labyrinth_.state() & Labyrinth::Generating)
    {
        if (state_ & StoppedSolving)
        {
            state_ &= ~Solving;

            return false;
        }

        sleep(dummyThreadSpleepingDuration);
    }

    if (state_ & StoppedSolving)
    {
        state_ &= ~Solving;

        return false;
    }

    auto const t(std::chrono::steady_clock::now());

    try
    {
        solver(g, *this, sleep, finishIndex, movements, operationsCycle, cyclePause, timeout);
    }
    catch (Labyrinth3d::Solver::TimeoutException const&)
    {
        state_ &= ~Solving;

        throw;
    }

    if (state_ & Finished)
        solvingDuration_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t);

    state_ &= ~Solving;

    if (state_ & Finished)
    {
        state_ |= Solved;

        return true;
    }

    return false;
}

#endif // LABYRINTH3D_PLAYER_H
