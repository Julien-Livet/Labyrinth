#include <stdexcept>
#include <cassert>

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/utility.h"

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

Labyrinth3d::Grid const& Labyrinth3d::Labyrinth::grid() const
{
    return grid_;
}

Labyrinth3d::Grid& Labyrinth3d::Labyrinth::grid()
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

size_t Labyrinth3d::Labyrinth::addPlayer(std::tuple<size_t, size_t, size_t> const& start,
                                         std::vector<std::tuple<size_t, size_t, size_t> > const& finishes,
                                         bool enabledTrace, bool blockingFinish, bool keptFullTrace)
{
    if (std::get<0>(start) >= grid_.rows()
        || std::get<1>(start) >= grid_.columns()
        || std::get<2>(start) >= grid_.floors())
        throw std::invalid_argument("Invalid start");

    std::vector<std::tuple<size_t, size_t, size_t> > f;
    f.reserve(finishes.size());

    for (auto const& t : finishes)
    {
        if (std::get<0>(t) >= grid_.rows())
            throw std::invalid_argument("Invalid finish");
        if (std::get<1>(t) >= grid_.columns())
            throw std::invalid_argument("Invalid finish");
        if (std::get<2>(t) >= grid_.floors())
            throw std::invalid_argument("Invalid finish");

        f.emplace_back(std::make_tuple(2 * std::get<0>(t) + 1,
                                       2 * std::get<1>(t) + 1,
                                       2 * std::get<2>(t) + 1));
    }

    while (players_.count(idCounter_))
        ++idCounter_;

    players_.emplace(idCounter_,
                     std::unique_ptr<Player>(new Player(*this, std::make_tuple(2 * std::get<0>(start) + 1,
                                                                               2 * std::get<1>(start) + 1,
                                                                               2 * std::get<2>(start) + 1),
                                                        f, enabledTrace, blockingFinish, keptFullTrace)));

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

std::queue<char>& Labyrinth3d::Labyrinth::read(std::queue<char>& data)
{
    size_t rows;
    Labyrinth3d::read(data, rows);
    size_t columns;
    Labyrinth3d::read(data, columns);
    size_t floors;
    Labyrinth3d::read(data, floors);

    grid_ = Grid{*this, rows, columns, floors};

    for (size_t k{1}; k < grid_.depth() - 1; k += 2)
    {
        for (size_t i{2}; i < grid_.height() - 1; i += 2)
        {
            for (size_t j{1}; j < grid_.width() - 1; j += 2)
            {
                bool b{false};
                Labyrinth3d::read(data, b);
                grid_.change(i, j, k, b);
            }
        }
    }

    for (size_t k{1}; k < grid_.depth() - 1; k += 2)
    {
        for (size_t j{2}; j < grid_.width() - 1; j += 2)
        {
            for (size_t i{1}; i < grid_.height() - 1; i += 2)
            {
                bool b{false};
                Labyrinth3d::read(data, b);
                grid_.change(i, j, k, b);
            }
        }
    }

    for (size_t j{1}; j < grid_.width() - 1; j += 2)
    {
        for (size_t k{2}; k < grid_.depth() - 1; k += 2)
        {
            for (size_t i{1}; i < grid_.height() - 1; i += 2)
            {
                bool b{false};
                Labyrinth3d::read(data, b);
                grid_.change(i, j, k, b);
            }
        }
    }

    for (size_t j{1}; j < grid_.width() - 1; j += 2)
    {
        for (size_t i{2}; i < grid_.height() - 1; i += 2)
        {
            for (size_t k{1}; k < grid_.depth() - 1; k += 2)
            {
                bool b{false};
                Labyrinth3d::read(data, b);
                grid_.change(i, j, k, b);
            }
        }
    }

    for (size_t i{1}; i < grid_.height() - 1; i += 2)
    {
        for (size_t k{2}; k < grid_.depth() - 1; k += 2)
        {
            for (size_t j{1}; j < grid_.width() - 1; j += 2)
            {
                bool b{false};
                Labyrinth3d::read(data, b);
                grid_.change(i, j, k, b);
            }
        }
    }

    for (size_t i{1}; i < grid_.height() - 1; i += 2)
    {
        for (size_t j{2}; j < grid_.width() - 1; j += 2)
        {
            for (size_t k{1}; k < grid_.depth() - 1; k += 2)
            {
                bool b{false};
                Labyrinth3d::read(data, b);
                grid_.change(i, j, k, b);
            }
        }
    }

    Labyrinth3d::read(data, state_);
    size_t count;
    Labyrinth3d::read(data, count);
    generationDuration_ = std::chrono::milliseconds{count};
    players_.clear();
    size_t playersSize;
    Labyrinth3d::read(data, playersSize);
    for (size_t i{0}; i < playersSize; ++i)
    {
        size_t id{0};
        Labyrinth3d::read(data, id);
        std::tuple<size_t, size_t, size_t> start;
        Labyrinth3d::read(data, std::get<0>(start));
        Labyrinth3d::read(data, std::get<1>(start));
        Labyrinth3d::read(data, std::get<2>(start));
        std::vector<std::tuple<size_t, size_t, size_t> > finishes;
        size_t finishesSize;
        Labyrinth3d::read(data, finishesSize);
        finishes.reserve(finishesSize);
        for (size_t j{0}; j < finishesSize; ++j)
        {
            std::tuple<size_t, size_t, size_t> index{std::make_tuple(0, 0, 0)};
            Labyrinth3d::read(data, std::get<0>(index));
            Labyrinth3d::read(data, std::get<1>(index));
            Labyrinth3d::read(data, std::get<2>(index));
            finishes.emplace_back(index);
        }
        players_.emplace(id, std::unique_ptr<Player>{new Player{*this, start, finishes, false, false, false}});
        Labyrinth3d::read(data, std::get<0>(players_[id]->current_));
        Labyrinth3d::read(data, std::get<1>(players_[id]->current_));
        Labyrinth3d::read(data, std::get<2>(players_[id]->current_));
        Labyrinth3d::read(data, players_[id]->movements_);
        Labyrinth3d::read(data, count);
        players_[id]->finishingDuration_ = std::chrono::milliseconds{count};
        Labyrinth3d::read(data, count);
        players_[id]->solvingDuration_ = std::chrono::milliseconds{count};
        Labyrinth3d::read(data, players_[id]->state_);
        Labyrinth3d::read(data, count);
        players_[id]->startTime_ = std::chrono::time_point<std::chrono::steady_clock>{std::chrono::steady_clock::duration{count}};
        Labyrinth3d::read(data, players_[id]->enabledTrace_);
        size_t traceIntersectionsSize;
        Labyrinth3d::read(data, traceIntersectionsSize);
        players_[id]->traceIntersections_.clear();
        players_[id]->traceIntersections_.reserve(traceIntersectionsSize);
        for (size_t j{0}; j < traceIntersectionsSize; ++j)
        {
            std::tuple<size_t, size_t, size_t> t{std::make_tuple(0, 0, 0)};
            Labyrinth3d::read(data, std::get<0>(t));
            Labyrinth3d::read(data, std::get<1>(t));
            Labyrinth3d::read(data, std::get<2>(t));
            players_[id]->traceIntersections_.emplace_back(t);
        }
        Labyrinth3d::read(data, players_[id]->blockingFinish_);
        Labyrinth3d::read(data, players_[id]->keptFullTrace_);
        size_t fullTraceSize;
        Labyrinth3d::read(data, fullTraceSize);
        players_[id]->fullTrace_.clear();
        players_[id]->fullTrace_.reserve(fullTraceSize);
        for (size_t j{0}; j < fullTraceSize; ++j)
        {
            std::tuple<size_t, size_t, size_t> t{std::make_tuple(0, 0, 0)};
            Labyrinth3d::read(data, std::get<0>(t));
            Labyrinth3d::read(data, std::get<1>(t));
            Labyrinth3d::read(data, std::get<2>(t));
            players_[id]->fullTrace_.emplace_back(t);
        }
    }
    Labyrinth3d::read(data, idCounter_);

    return data;
}

std::queue<char>& Labyrinth3d::Labyrinth::write(std::queue<char>& data) const
{
    Labyrinth3d::write(data, grid_.rows());
    Labyrinth3d::write(data, grid_.columns());
    Labyrinth3d::write(data, grid_.floors());

    for (size_t k{1}; k < grid_.depth() - 1; k += 2)
    {
        for (size_t i{2}; i < grid_.height() - 1; i += 2)
        {
            for (size_t j{1}; j < grid_.width() - 1; j += 2)
                Labyrinth3d::write(data, grid_(i, j, k));
        }
    }

    for (size_t k{1}; k < grid_.depth() - 1; k += 2)
    {
        for (size_t j{2}; j < grid_.width() - 1; j += 2)
        {
            for (size_t i{1}; i < grid_.height() - 1; i += 2)
                Labyrinth3d::write(data, grid_(i, j, k));
        }
    }

    for (size_t j{1}; j < grid_.width() - 1; j += 2)
    {
        for (size_t k{2}; k < grid_.depth() - 1; k += 2)
        {
            for (size_t i{1}; i < grid_.height() - 1; i += 2)
                Labyrinth3d::write(data, grid_(i, j, k));
        }
    }

    for (size_t j{1}; j < grid_.width() - 1; j += 2)
    {
        for (size_t i{2}; i < grid_.height() - 1; i += 2)
        {
            for (size_t k{1}; k < grid_.depth() - 1; k += 2)
                Labyrinth3d::write(data, grid_(i, j, k));
        }
    }

    for (size_t i{1}; i < grid_.height() - 1; i += 2)
    {
        for (size_t k{2}; k < grid_.depth() - 1; k += 2)
        {
            for (size_t j{1}; j < grid_.width() - 1; j += 2)
                Labyrinth3d::write(data, grid_(i, j, k));
        }
    }

    for (size_t i{1}; i < grid_.height() - 1; i += 2)
    {
        for (size_t j{2}; j < grid_.width() - 1; j += 2)
        {
            for (size_t k{1}; k < grid_.depth() - 1; k += 2)
                Labyrinth3d::write(data, grid_(i, j, k));
        }
    }

    Labyrinth3d::write(data, state_);
    Labyrinth3d::write(data, generationDuration_.count());

    Labyrinth3d::write(data, players_.size());
    for (auto it{players_.begin()}; it != players_.end(); ++it)
    {
        Labyrinth3d::write(data, it->first);
        Labyrinth3d::write(data, std::get<0>(it->second->start_));
        Labyrinth3d::write(data, std::get<1>(it->second->start_));
        Labyrinth3d::write(data, std::get<2>(it->second->start_));
        Labyrinth3d::write(data, it->second->finishes_.size());
        for (auto const& index : it->second->finishes_)
        {
            Labyrinth3d::write(data, std::get<0>(index));
            Labyrinth3d::write(data, std::get<1>(index));
            Labyrinth3d::write(data, std::get<2>(index));
        }
        Labyrinth3d::write(data, std::get<0>(it->second->current_));
        Labyrinth3d::write(data, std::get<1>(it->second->current_));
        Labyrinth3d::write(data, std::get<2>(it->second->current_));
        Labyrinth3d::write(data, it->second->movements_);
        Labyrinth3d::write(data, it->second->finishingDuration_.count());
        Labyrinth3d::write(data, it->second->solvingDuration_.count());
        Labyrinth3d::write(data, it->second->state_);
        Labyrinth3d::write(data, it->second->startTime_.time_since_epoch().count());
        Labyrinth3d::write(data, it->second->enabledTrace_);
        Labyrinth3d::write(data, it->second->traceIntersections_.size());
        for (auto const& t : it->second->traceIntersections_)
        {
            Labyrinth3d::write(data, std::get<0>(t));
            Labyrinth3d::write(data, std::get<1>(t));
            Labyrinth3d::write(data, std::get<2>(t));
        }
        Labyrinth3d::write(data, it->second->blockingFinish_);
        Labyrinth3d::write(data, it->second->keptFullTrace_);
        Labyrinth3d::write(data, it->second->fullTrace_.size());
        for (auto const& t : it->second->fullTrace_)
        {
            Labyrinth3d::write(data, std::get<0>(t));
            Labyrinth3d::write(data, std::get<1>(t));
            Labyrinth3d::write(data, std::get<2>(t));
        }
    }
    Labyrinth3d::write(data, idCounter_);

    return data;
}
