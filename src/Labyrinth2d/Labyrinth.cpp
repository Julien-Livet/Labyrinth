#include <stdexcept>
#include <cassert>

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/utility.h"
#include "Labyrinth2d/Algorithm/Algorithm.h"

Labyrinth2d::Labyrinth::Labyrinth(size_t rows, size_t columns) : grid_(*this, rows, columns), state_(Initialized),
                                                                 idCounter_(0)
{
}

Labyrinth2d::Labyrinth::~Labyrinth()
{
    while (state_ & Generating)
        stopGenerating();

    stopPlayerSolving();
}

size_t Labyrinth2d::Labyrinth::state() const
{
    return state_;
}

std::chrono::milliseconds const& Labyrinth2d::Labyrinth::generationDuration() const
{
    return generationDuration_;
}

std::vector<size_t> Labyrinth2d::Labyrinth::playerIds() const
{
    std::vector<size_t> ids;

    ids.reserve(players_.size());

    for (auto const& p : players_)
        ids.push_back(p.first);

    return ids;
}

Labyrinth2d::Player const& Labyrinth2d::Labyrinth::player(size_t id) const
{
    return *players_.at(id);
}

Labyrinth2d::Player& Labyrinth2d::Labyrinth::player(size_t id)
{
    return *players_.at(id);
}

Labyrinth2d::Grid const& Labyrinth2d::Labyrinth::grid() const
{
    return grid_;
}

Labyrinth2d::Grid& Labyrinth2d::Labyrinth::grid()
{
    return grid_;
}

size_t Labyrinth2d::Labyrinth::addPlayer(size_t startRow, size_t startColumn,
                                         std::vector<size_t> const& finishRows,
                                         std::vector<size_t> const& finishColumns,
                                         bool enabledTrace, bool blockingFinish, bool keptFullTrace)
{
    if (startRow >= grid_.rows() || startColumn >= grid_.columns())
        throw std::invalid_argument("Invalid start");

    if (finishRows.size() != finishColumns.size())
        throw std::invalid_argument("Invalid finishes");

	std::vector<size_t> finishI;
	finishI.reserve(finishRows.size());

	for (auto r : finishRows)
	{
		if (r >= grid_.rows())
			throw std::invalid_argument("Invalid finish");
		
		finishI.emplace_back(2 * r + 1);
	}

	std::vector<size_t> finishJ;
	finishJ.reserve(finishColumns.size());

	for (auto c : finishColumns)
	{
		if (c >= grid_.columns())
			throw std::invalid_argument("Invalid finish");
		
		finishJ.emplace_back(2 * c + 1);
	}

    while (players_.count(idCounter_))
        ++idCounter_;

    players_.emplace(idCounter_,
                     std::unique_ptr<Player>(new Player(*this, 2 * startRow + 1, 2 * startColumn + 1,
                                                        finishI, finishJ, enabledTrace, blockingFinish, keptFullTrace)));

    return idCounter_;
}

bool Labyrinth2d::Labyrinth::removePlayer(size_t id)
{
    if (!players_.count(id))
        return false;

    players_.erase(id);

    return true;
}

void Labyrinth2d::Labyrinth::restart()
{
    if (!(state_ & Generated))
        return;

    for (auto& p : players_)
        p.second->restart();
}

void Labyrinth2d::Labyrinth::stopPlayerSolving()
{
    for (auto id : playerIds())
    {
        while (players_.at(id)->state() & Player::Solving)
            players_.at(id)->stopSolving();
    }
}

void Labyrinth2d::Labyrinth::stopGenerating()
{
    state_ |= StopGenerating;
}
