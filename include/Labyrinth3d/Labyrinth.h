#ifndef LABYRINTH3D_LABYRINTH_H
#define LABYRINTH3D_LABYRINTH_H

/*!
 *  \file Labyrinth.h
 *  \brief Represent a labyrinth with players
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <cassert>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <queue>

#include "Grid.h"
#include "utility.h"

namespace Labyrinth3d
{
    class Player;

    /*!
     *  \brief Represent a labyrinth with players
     */
    class Labyrinth
    {
        public:
            /*!
             *  \brief Labyrinth state
             */
            enum State
            {
                Initialized = 0x1,
                Generating = 0x2,
                Generated = 0x4,
                StopGenerating = 0x8
            };

            /*!
             *  \brief Construct a rows x columns x floors sized labyrinth with border and intersection walls
             */
            Labyrinth(size_t rows, size_t columns, size_t floors);

            /*!
             *  \brief Destructor
             */
            ~Labyrinth();

            /*!
             *  \return The labyrinth state
             */
            size_t state() const;

            /*!
             *  \return The generation duration
             */
            std::chrono::milliseconds const& generationDuration() const;

            /*!
             *  \return The labyrinth player ids
             */
            std::vector<size_t> playerIds() const;

            /*!
             *  \param id: player id
             *
             *  \return A constant reference to the player which matches with the desired id
             */
            Player const& player(size_t id) const;

            /*!
             *  \param id: player id
             *
             *  \return A reference to the player which matches with the desired id
             */
            Player& player(size_t id);

            /*!
             *  \return A constant reference to the grid representing the cells
             */
            Grid const& grid() const;

            /*!
             *  \return A reference to the grid representing the cells
             */
            Grid& grid();

            /*!
             *  \brief Add a player in the labyrinth
             *
             *  \param startRow: row start index in the labyrinth in [0; rows() - 1]
             *  \param startColumn: start column index in the labyrinth in [0; columns() - 1]
             *  \param startFloor: start floor index in the labyrinth in [0; floors() - 1]
             *  \param finishRows: finish row indexes in the labyrinth in [0; rows() - 1]
             *  \param finishColumns: finish column indexes in the labyrinth in [0; columns() - 1]
             *  \param finishFloors: finish floor indexes in the labyrinth in [0; floors() - 1]
             *  \param enabledTrace: a boolean which is true if the player keep a trace of his path
             *  \param blockingFinish: a boolean which is true if the player can not move when he reaches his finish
             *
             *  \return An id corresponding to the added player
             */
            size_t addPlayer(size_t startRow, size_t startColumn, size_t startFloor,
                             std::vector<size_t> const& finishRows, std::vector<size_t> const& finishColumns, std::vector<size_t> const& finishFloors,
                             bool enabledTrace = false, bool blockingFinish = true, bool keptFullTrace = false);

            /*!
             *  \brief Add a player in the labyrinth
             *
             *  \param start: start row and column and floor index in the labyrinth in [0; rows() - 1]x[0; columns() - 1]x[0; floors() - 1]
             *  \param finishes: finish row and column and florr indexes in the labyrinth in [0; rows() - 1]x[0; columns() - 1]x[0; floors() - 1]
             *  \param enabledTrace: a boolean which is true if the player keep a trace of his path
             *  \param blockingFinish: a boolean which is true if the player can not move when he reaches his finish
             *
             *  \return An id corresponding to the added player
             */
            size_t addPlayer(std::tuple<size_t, size_t, size_t> const& start,
                             std::vector<std::tuple<size_t, size_t, size_t> > const& finishes,
                             bool enabledTrace = false, bool blockingFinish = true, bool keptFullTrace = false);

            /*!
             *  \brief Remove a player in the labyrinth
             *
             *  \param id: player id
             *
             *  \return A boolean wich is true if the player was removed
             */
            bool removePlayer(size_t id);

            /*!
             *  \brief Restart the labyrinth, eg. with the same walls but players start from scratch
             */
            void restart();

            /*!
             *  \brief Generate a labyrinth
             *
             *  \param g: uniform random number generator
             *  \param algorithm: algorithm to generate the walls
             *  \param sleep: sleep function
             *  \param cycleOperations: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort generation
             */
            template <class URG, class Algorithm>
            void generate(URG& g, Algorithm& algorithm,
                          std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                          size_t cycleOperations = 0,
                          std::chrono::milliseconds cyclePause = std::chrono::milliseconds(0),
                          std::chrono::milliseconds const* timeout = nullptr);

            /*!
             *  \brief Generate a perfect labyrinth and degenerate it to make it imperfect
             *
             *  \param g1: uniform random number generator for perfect algorithm
             *  \param g2: uniform random number generator for degenerative algorithm
             *  \param perfectAlgorithm: perfect algorithm to generate the walls
             *  \param degenerativeAlgorithm: algorithm to destruct walls
             *  \param sleep: sleep function
             *  \param cycleOperations: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort generation
             */
            template <class URG1, class URG2, class PerfectAlgorithm, class DegenerativeAlgorithm>
            void generate(URG1& g1, URG2& g2, PerfectAlgorithm& perfectAlgorithm,
                          DegenerativeAlgorithm& degenerativeAlgorithm,
                          std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                          size_t cycleOperations = 0,
                          std::chrono::milliseconds cyclePause = std::chrono::milliseconds(0),
                          std::chrono::milliseconds const* timeout = nullptr);

            /*!
             *  \brief Stop generating labyrinth
             */
            void stopGenerating();

            /*!
             *  \brief Stop solving players
             */
            void stopPlayerSolving();

            /*!
             *  \brief Read from binary stream as raw data
             */
            std::queue<char>& read(std::queue<char>& data);

            /*!
             *  \brief Write into binary stream as raw data
             */
            std::queue<char>& write(std::queue<char>& data) const;

        private:
            Grid grid_;
            unsigned int state_;
            std::chrono::milliseconds generationDuration_;
            std::map<size_t, std::unique_ptr<Player> > players_;
            size_t idCounter_;
    };
}

#include "Player.h"
#include "Algorithm/TimeoutException.h"

template <class URG, class Algorithm>
void Labyrinth3d::Labyrinth::generate(URG& g, Algorithm& algorithm,
                                      std::function<void(std::chrono::milliseconds)> const& sleep,
                                      size_t cycleOperations,
                                      std::chrono::milliseconds cyclePause,
                                      std::chrono::milliseconds const* timeout)
{
    if (state_ & Generating)
        return;

    stopPlayerSolving();

    state_ |= Generating;
    state_ &= ~Generated;
    state_ &= ~StopGenerating;

    if (!(state_ & Initialized))
    {
        state_ |= Initialized;
        grid_ = Grid(*this, grid_.rows(), grid_.columns(), grid_.floors());
        generationDuration_ = std::chrono::milliseconds();
    }

    SubGrid subGrid(grid_, SubGrid::SetAndReset);

    auto const t(std::chrono::steady_clock::now());

    try
    {
        algorithm(g, subGrid, sleep, cycleOperations, cyclePause, timeout);
    }
    catch (Labyrinth3d::Algorithm::TimeoutException const&)
    {
        state_ = Initialized;
        grid_ = Grid(*this, grid_.rows(), grid_.columns(), grid_.floors());

        for (auto& p : players_)
            p.second->restart();

        throw;
    }

    generationDuration_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t);
    state_ &= ~Initialized;
    state_ |= Generated;

    for (auto& p : players_)
        p.second->restart();

    state_ &= ~Generating;
}

template <class URG1, class URG2, class PerfectAlgorithm, class DegenerativeAlgorithm>
void Labyrinth3d::Labyrinth::generate(URG1& g1, URG2& g2, PerfectAlgorithm& perfectAlgorithm,
                                      DegenerativeAlgorithm& degenerativeAlgorithm,
                                      std::function<void(std::chrono::milliseconds)> const& sleep,
                                      size_t cycleOperations,
                                      std::chrono::milliseconds cyclePause,
                                      std::chrono::milliseconds const* timeout)
{
    if (state_ & Generating)
        return;

    stopPlayerSolving();

    state_ |= Generating;
    state_ &= ~Generated;
    state_ &= ~StopGenerating;

    if (!(state_ & Initialized))
    {
        state_ |= Initialized;
        grid_ = Grid(grid_.rows(), grid_.columns(), grid_.floors());
        generationDuration_ = std::chrono::milliseconds();
    }

    SubGrid perfectSubGrid(grid_, SubGrid::SetAndReset);

    auto const t(std::chrono::steady_clock::now());

    try
    {
        perfectAlgorithm(g1, perfectSubGrid, sleep, cycleOperations, cyclePause, timeout);
    }
    catch (Labyrinth3d::Algorithm::TimeoutException const&)
    {
        state_ = Initialized;
        grid_ = Grid(*this, grid_.rows(), grid_.columns());

        for (auto& p : players_)
            p.second->restart();

        throw;
    }

    SubGrid degenerativeSubGrid(grid_, SubGrid::Reset);

    if (timeout != nullptr)
    {
        try
        {
            auto const timeoutTmp(*timeout -
                                  (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t)));

            degenerativeAlgorithm(g2, degenerativeSubGrid, sleep, cycleOperations, cyclePause, &timeoutTmp);
        }
        catch (Labyrinth3d::Algorithm::TimeoutException const&)
        {
            state_ = Initialized;
            grid_ = Grid(*this, grid_.rows(), grid_.columns());

            for (auto& p : players_)
                p.second->restart();

            throw;
        }
    }
    else
        degenerativeAlgorithm(g2, degenerativeSubGrid, sleep, cycleOperations, cyclePause);

    generationDuration_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t);
    state_ &= ~Initialized;
    state_ |= Generated;

    for (auto& p : players_)
        p.second->restart();

    state_ &= ~Generating;
}

#endif // LABYRINTH3D_LABYRINTH_H
