#include <stdexcept>
#include <cassert>

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/utility.h"
#include "Labyrinth3d/Algorithm/Algorithm.h"

Labyrinth3d::Labyrinth::Labyrinth(size_t rows, size_t columns, size_t floors) : grid_(*this, rows, columns, floors),
                                                                                state_(Initialized),
                                                                                idCounter_(0)
{
}

Labyrinth3d::Labyrinth::~Labyrinth()
{
    while (state_ & Generating)
        stopGenerating();

    stopPlayerSolving();
}

size_t Labyrinth3d::Labyrinth::state() const
{
    return state_;
}

std::chrono::milliseconds const& Labyrinth3d::Labyrinth::generationDuration() const
{
    return generationDuration_;
}

std::vector<size_t> Labyrinth3d::Labyrinth::playerIds() const
{
    std::vector<size_t> ids;

    ids.reserve(players_.size());

    for (auto const& p : players_)
        ids.push_back(p.first);

    return ids;
}

Labyrinth3d::Player const& Labyrinth3d::Labyrinth::player(size_t id) const
{
    return *players_.at(id);
}

Labyrinth3d::Player& Labyrinth3d::Labyrinth::player(size_t id)
{
    return *players_.at(id);
}

Labyrinth3d::Grid<bool> const& Labyrinth3d::Labyrinth::grid() const
{
    return grid_;
}

Labyrinth3d::Grid<bool>& Labyrinth3d::Labyrinth::grid()
{
    return grid_;
}

size_t Labyrinth3d::Labyrinth::addPlayer(size_t startRow, size_t startColumn, size_t startFloor,
                                         std::vector<size_t> const& finishRows, std::vector<size_t> const& finishColumns, std::vector<size_t> const& finishFloors,
                                         bool enabledTrace, bool blockingFinish, bool keptFullTrace)
{
    if (startRow >= grid_.rows() || startColumn >= grid_.columns() || startFloor >= grid_.floors())
        throw std::invalid_argument("Invalid start");

    if (finishRows.size() != finishColumns.size() && finishRows.size() != finishFloors.size())
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

	std::vector<size_t> finishK;
	finishK.reserve(finishFloors.size());

	for (auto f : finishFloors)
	{
		if (f >= grid_.floors())
			throw std::invalid_argument("Invalid finish");

		finishK.emplace_back(2 * f + 1);
	}

    while (players_.count(idCounter_))
        ++idCounter_;

    players_.emplace(idCounter_,
                     std::unique_ptr<Player>(new Player(*this, 2 * startRow + 1, 2 * startColumn + 1, 2 * startFloor + 1,
                                                        finishI, finishJ, finishK,
                                                        enabledTrace, blockingFinish, keptFullTrace)));

    return idCounter_;
}

bool Labyrinth3d::Labyrinth::removePlayer(size_t id)
{
    if (!players_.count(id))
        return false;

    players_.erase(id);

    return true;
}

void Labyrinth3d::Labyrinth::restart()
{
    if (!(state_ & Generated))
        return;

    for (auto& p : players_)
        p.second->restart();
}

void Labyrinth3d::Labyrinth::stopPlayerSolving()
{
    for (auto id : playerIds())
    {
        while (players_.at(id)->state() & Player::Solving)
            players_.at(id)->stopSolving();
    }
}

void Labyrinth3d::Labyrinth::stopGenerating()
{
    state_ |= StopGenerating;
}
