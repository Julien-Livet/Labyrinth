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

size_t Labyrinth2d::Labyrinth::addPlayer(std::pair<size_t, size_t> const& start,
                                         std::vector<std::pair<size_t, size_t> > const& finishes,
                                         bool enabledTrace, bool blockingFinish, bool keptFullTrace)
{
    if (start.first >= grid_.rows() || start.second >= grid_.columns())
        throw std::invalid_argument("Invalid start");

    std::vector<std::pair<size_t, size_t> > f;
    f.reserve(finishes.size());

    for (auto const& p : finishes)
    {
        if (p.first >= grid_.rows())
            throw std::invalid_argument("Invalid finish");
        if (p.second >= grid_.columns())
            throw std::invalid_argument("Invalid finish");

        f.emplace_back(std::make_pair(2 * p.first + 1, 2 * p.second + 1));
    }

    while (players_.count(idCounter_))
        ++idCounter_;

    players_.emplace(idCounter_,
                     std::unique_ptr<Player>(new Player(*this, std::make_pair(2 * start.first + 1, 2 * start.second + 1),
                                                        f, enabledTrace, blockingFinish, keptFullTrace)));

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
    Labyrinth2d::read(data, rows);
    size_t columns;
    Labyrinth2d::read(data, columns);

    grid_ = Grid{*this, rows, columns};

    for (size_t i{2}; i < grid_.height() - 1; i += 2)
    {
        for (size_t j{1}; j < grid_.width() - 1; j += 2)
        {
            bool b{false};
            Labyrinth2d::read(data, b);
            grid_.change(i, j, b);
        }
    }

    for (size_t j{2}; j < grid_.width() - 1; j += 2)
    {
        for (size_t i{1}; i < grid_.height() - 1; i += 2)
        {
            bool b{false};
            Labyrinth2d::read(data, b);
            grid_.change(i, j, b);
        }
    }

    Labyrinth2d::read(data, state_);
    size_t count;
    Labyrinth2d::read(data, count);
    generationDuration_ = std::chrono::milliseconds{count};
    players_.clear();
    size_t playersSize;
    Labyrinth2d::read(data, playersSize);
    for (size_t i{0}; i < playersSize; ++i)
    {
        size_t id{0};
        Labyrinth2d::read(data, id);
        std::pair<size_t, size_t> start;
        Labyrinth2d::read(data, start.first);
        Labyrinth2d::read(data, start.second);
        std::vector<std::pair<size_t, size_t> > finishes;
        size_t finishesSize;
        Labyrinth2d::read(data, finishesSize);
        finishes.reserve(finishesSize);
        for (size_t j{0}; j < finishesSize; ++j)
        {
            std::pair<size_t, size_t> index{std::make_pair(0, 0)};
            Labyrinth2d::read(data, index.first);
            Labyrinth2d::read(data, index.second);
            finishes.emplace_back(index);
        }
        players_.emplace(id, std::unique_ptr<Player>{new Player{*this, start, finishes, false, false, false}});
        Labyrinth2d::read(data, players_[id]->current_.first);
        Labyrinth2d::read(data, players_[id]->current_.second);
        Labyrinth2d::read(data, players_[id]->movements_);
        Labyrinth2d::read(data, count);
        players_[id]->finishingDuration_ = std::chrono::milliseconds{count};
        Labyrinth2d::read(data, count);
        players_[id]->solvingDuration_ = std::chrono::milliseconds{count};
        Labyrinth2d::read(data, players_[id]->state_);
        Labyrinth2d::read(data, count);
        players_[id]->startTime_ = std::chrono::time_point<std::chrono::steady_clock>{std::chrono::steady_clock::duration{count}};
        Labyrinth2d::read(data, players_[id]->enabledTrace_);
        size_t traceIntersectionsSize;
        Labyrinth2d::read(data, traceIntersectionsSize);
        players_[id]->traceIntersections_.clear();
        players_[id]->traceIntersections_.reserve(traceIntersectionsSize);
        for (size_t j{0}; j < traceIntersectionsSize; ++j)
        {
            std::pair<size_t, size_t> p{std::make_pair(0, 0)};
            Labyrinth2d::read(data, p.first);
            Labyrinth2d::read(data, p.second);
            players_[id]->traceIntersections_.emplace_back(p);
        }
        Labyrinth2d::read(data, players_[id]->blockingFinish_);
        Labyrinth2d::read(data, players_[id]->keptFullTrace_);
        size_t fullTraceSize;
        Labyrinth2d::read(data, fullTraceSize);
        players_[id]->fullTrace_.clear();
        players_[id]->fullTrace_.reserve(fullTraceSize);
        for (size_t j{0}; j < fullTraceSize; ++j)
        {
            std::pair<size_t, size_t> p{std::make_pair(0, 0)};
            Labyrinth2d::read(data, p.first);
            Labyrinth2d::read(data, p.second);
            players_[id]->fullTrace_.emplace_back(p);
        }
    }
    Labyrinth2d::read(data, idCounter_);

    return data;
}

std::queue<char>& Labyrinth2d::Labyrinth::write(std::queue<char>& data) const
{
    Labyrinth2d::write(data, grid_.rows());
    Labyrinth2d::write(data, grid_.columns());

    for (size_t i{2}; i < grid_.height() - 1; i += 2)
    {
        for (size_t j{1}; j < grid_.width() - 1; j += 2)
            Labyrinth2d::write(data, grid_(i, j));
    }

    for (size_t j{2}; j < grid_.width() - 1; j += 2)
    {
        for (size_t i{1}; i < grid_.height() - 1; i += 2)
            Labyrinth2d::write(data, grid_(i, j));
    }

    Labyrinth2d::write(data, state_);
    Labyrinth2d::write(data, generationDuration_.count());

    Labyrinth2d::write(data, players_.size());
    for (auto it{players_.begin()}; it != players_.end(); ++it)
    {
        Labyrinth2d::write(data, it->first);
        Labyrinth2d::write(data, it->second->start_.first);
        Labyrinth2d::write(data, it->second->start_.second);
        Labyrinth2d::write(data, it->second->finishes_.size());
        for (auto const& index : it->second->finishes_)
        {
            Labyrinth2d::write(data, index.first);
            Labyrinth2d::write(data, index.second);
        }
        Labyrinth2d::write(data, it->second->current_.first);
        Labyrinth2d::write(data, it->second->current_.second);
        Labyrinth2d::write(data, it->second->movements_);
        Labyrinth2d::write(data, it->second->finishingDuration_.count());
        Labyrinth2d::write(data, it->second->solvingDuration_.count());
        Labyrinth2d::write(data, it->second->state_);
        Labyrinth2d::write(data, it->second->startTime_.time_since_epoch().count());
        Labyrinth2d::write(data, it->second->enabledTrace_);
        Labyrinth2d::write(data, it->second->traceIntersections_.size());
        for (auto const& p : it->second->traceIntersections_)
        {
            Labyrinth2d::write(data, p.first);
            Labyrinth2d::write(data, p.second);
        }
        Labyrinth2d::write(data, it->second->blockingFinish_);
        Labyrinth2d::write(data, it->second->keptFullTrace_);
        Labyrinth2d::write(data, it->second->fullTrace_.size());
        for (auto const& p : it->second->fullTrace_)
        {
            Labyrinth2d::write(data, p.first);
            Labyrinth2d::write(data, p.second);
        }
    }
    Labyrinth2d::write(data, idCounter_);

    return data;
}
