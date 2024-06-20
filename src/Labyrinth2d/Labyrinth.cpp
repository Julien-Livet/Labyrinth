#include <cassert>
#include <stdexcept>

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/utility.h"

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

std::queue<char>& Labyrinth2d::Labyrinth::read(std::queue<char>& data)
{
    size_t rows;
    ::read(data, rows);
    size_t columns;
    ::read(data, columns);

    grid_ = Grid{*this, rows, columns};

    for (size_t i{2}; i < grid_.height() - 1; i += 2)
    {
        for (size_t j{1}; j < grid_.width() - 1; j += 2)
        {
            bool b{false};
            ::read(data, b);
            grid_.change(i, j, b);
        }
    }

    for (size_t j{2}; j < grid_.width() - 1; j += 2)
    {
        for (size_t i{1}; i < grid_.height() - 1; i += 2)
        {
            bool b{false};
            ::read(data, b);
            grid_.change(i, j, b);
        }
    }

    ::read(data, state_);
    size_t count;
    ::read(data, count);
    generationDuration_ = std::chrono::milliseconds{count};
    players_.clear();
    size_t playersSize;
    ::read(data, playersSize);
    for (size_t i{0}; i < playersSize; ++i)
    {
        size_t id{0};
        ::read(data, id);
        size_t startI;
        ::read(data, startI);
        size_t startJ;
        ::read(data, startJ);
        std::vector<size_t> finishI;
        size_t finishISize;
        ::read(data, finishISize);
        finishI.reserve(finishISize);
        for (size_t j{0}; j < finishISize; ++j)
        {
            size_t index{0};
            ::read(data, index);
            finishI.emplace_back(index);
        }
        std::vector<size_t> finishJ;
        size_t finishJSize;
        ::read(data, finishJSize);
        finishJ.reserve(finishJSize);
        for (size_t j{0}; j < finishJSize; ++j)
        {
            size_t index{0};
            ::read(data, index);
            finishJ.emplace_back(index);
        }
        players_.emplace(id, std::unique_ptr<Player>{new Player{*this, startI, startJ,
                                                                finishI, finishJ, false, false, false}});
        ::read(data, players_[id]->i_);
        ::read(data, players_[id]->j_);
        ::read(data, players_[id]->movements_);
        ::read(data, count);
        players_[id]->finishingDuration_ = std::chrono::milliseconds{count};
        ::read(data, count);
        players_[id]->solvingDuration_ = std::chrono::milliseconds{count};
        ::read(data, players_[id]->state_);
        ::read(data, count);
        players_[id]->startTime_ = std::chrono::time_point<std::chrono::steady_clock>{std::chrono::steady_clock::duration{count}};
        ::read(data, players_[id]->enabledTrace_);
        size_t traceIntersectionsSize;
        ::read(data, traceIntersectionsSize);
        players_[id]->traceIntersections_.clear();
        players_[id]->traceIntersections_.reserve(traceIntersectionsSize);
        for (size_t j{0}; j < traceIntersectionsSize; ++j)
        {
            std::pair<size_t, size_t> p{std::make_pair(0, 0)};
            ::read(data, p.first);
            ::read(data, p.second);
            players_[id]->traceIntersections_.emplace_back(p);
        }
        ::read(data, players_[id]->blockingFinish_);
        ::read(data, players_[id]->keptFullTrace_);
        size_t fullTraceSize;
        ::read(data, fullTraceSize);
        players_[id]->fullTrace_.clear();
        players_[id]->fullTrace_.reserve(fullTraceSize);
        for (size_t j{0}; j < fullTraceSize; ++j)
        {
            std::pair<size_t, size_t> p{std::make_pair(0, 0)};
            ::read(data, p.first);
            ::read(data, p.second);
            players_[id]->fullTrace_.emplace_back(p);
        }
    }
    ::read(data, idCounter_);

    return data;
}

std::queue<char>& Labyrinth2d::Labyrinth::write(std::queue<char>& data) const
{
    ::write(data, grid_.rows());
    ::write(data, grid_.columns());

    for (size_t i{2}; i < grid_.height() - 1; i += 2)
    {
        for (size_t j{1}; j < grid_.width() - 1; j += 2)
            ::write(data, grid_(i, j));
    }

    for (size_t j{2}; j < grid_.width() - 1; j += 2)
    {
        for (size_t i{1}; i < grid_.height() - 1; i += 2)
            ::write(data, grid_(i, j));
    }

    ::write(data, state_);
    ::write(data, generationDuration_.count());

    ::write(data, players_.size());
    for (auto it{players_.begin()}; it != players_.end(); ++it)
    {
        ::write(data, it->first);
        ::write(data, it->second->startI_);
        ::write(data, it->second->startJ_);
        ::write(data, it->second->finishI_.size());
        for (auto&& i : it->second->finishI_)
            ::write(data, i);
        ::write(data, it->second->finishJ_.size());
        for (auto&& j : it->second->finishJ_)
            ::write(data, j);
        ::write(data, it->second->i_);
        ::write(data, it->second->j_);
        ::write(data, it->second->movements_);
        ::write(data, it->second->finishingDuration_.count());
        ::write(data, it->second->solvingDuration_.count());
        ::write(data, it->second->state_);
        ::write(data, it->second->startTime_.time_since_epoch().count());
        ::write(data, it->second->enabledTrace_);
        ::write(data, it->second->traceIntersections_.size());
        for (auto&& p : it->second->traceIntersections_)
        {
            ::write(data, p.first);
            ::write(data, p.second);
        }
        ::write(data, it->second->blockingFinish_);
        ::write(data, it->second->keptFullTrace_);
        ::write(data, it->second->fullTrace_.size());
        for (auto&& p : it->second->fullTrace_)
        {
            ::write(data, p.first);
            ::write(data, p.second);
        }
    }
    ::write(data, idCounter_);

    return data;
}
