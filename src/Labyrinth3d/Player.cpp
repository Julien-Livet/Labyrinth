#include <cassert>

#include "Labyrinth3d/Player.h"
#include "Labyrinth3d/Labyrinth.h"

Labyrinth3d::Player::Player(Labyrinth const& labyrinth,
                            size_t startI, size_t startJ, size_t startK,
                            std::vector<size_t> const& finishI, std::vector<size_t> const&finishJ, std::vector<size_t> const&finishK,
                            bool enabledTrace, bool blockingFinish,
                            bool keptFullTrace) : start_{std::make_tuple(startI, startJ, startK)},
                                                  finishes_{},
                                                  current_{start_},
                                                  labyrinth_{labyrinth}, movements_{0},
                                                  state_{0},
                                                  enabledTrace_{enabledTrace},
                                                  blockingFinish_{blockingFinish}, keptFullTrace_{keptFullTrace}
{
    enableTrace(enabledTrace);

    assert(finishI.size() && finishI.size() == finishJ.size() && finishI.size() == finishK.size());

    finishes_.reserve(finishI.size());

    for (std::size_t i{0}; i < finishI.size(); ++i)
        finishes_.emplace_back(std::make_tuple(finishI[i], finishJ[i], finishK[i]));

    for (auto const& p : finishes_)
    {
        if (p == current_)
        {
			state_ |= State::Finished;
			break;
		}
	}
}

Labyrinth3d::Player::Player(Labyrinth const& labyrinth,
                            std::tuple<size_t, size_t, size_t> const& start,
                            std::vector<std::tuple<size_t, size_t, size_t> > const&finishes,
                            bool enabledTrace, bool blockingFinish,
                            bool keptFullTrace) : start_{start},
                                                  finishes_{finishes},
                                                  current_{start_},
                                                  labyrinth_{labyrinth}, movements_{0},
                                                  state_{0},
                                                  enabledTrace_{enabledTrace},
                                                  blockingFinish_{blockingFinish}, keptFullTrace_{keptFullTrace}
{
    enableTrace(enabledTrace);

    assert(!finishes_.empty());

    for (auto const& p : finishes_)
    {
        if (p == current_)
        {
            state_ |= State::Finished;
            break;
        }
    }
}

Labyrinth3d::Player::~Player()
{
    while (state_ & Solving)
        stopSolving();
}

size_t Labyrinth3d::Player::startI() const
{
    return std::get<0>(start_);
}

size_t Labyrinth3d::Player::startJ() const
{
    return std::get<1>(start_);
}

size_t Labyrinth3d::Player::startK() const
{
    return std::get<2>(start_);
}

std::tuple<size_t, size_t, size_t> const& Labyrinth3d::Player::start() const
{
    return start_;
}

std::vector<size_t> Labyrinth3d::Player::finishI() const
{
    std::vector<size_t> finishI;
    finishI.reserve(finishes_.size());

    for (auto const& f : finishes_)
        finishI.emplace_back(std::get<0>(f));

    return finishI;
}

std::vector<size_t> Labyrinth3d::Player::finishJ() const
{
    std::vector<size_t> finishJ;
    finishJ.reserve(finishes_.size());

    for (auto const& f : finishes_)
        finishJ.emplace_back(std::get<1>(f));

    return finishJ;
}

std::vector<size_t> Labyrinth3d::Player::finishK() const
{
    std::vector<size_t> finishK;
    finishK.reserve(finishes_.size());

    for (auto const& f : finishes_)
        finishK.emplace_back(std::get<2>(f));

    return finishK;
}

std::vector<std::tuple<size_t, size_t, size_t> > const& Labyrinth3d::Player::finishes() const
{
    return finishes_;
}

size_t Labyrinth3d::Player::i() const
{
    return std::get<0>(current_);
}

size_t Labyrinth3d::Player::j() const
{
    return std::get<1>(current_);
}

size_t Labyrinth3d::Player::k() const
{
    return std::get<2>(current_);
}

std::tuple<size_t, size_t, size_t> const& Labyrinth3d::Player::current() const
{
    return current_;
}

size_t Labyrinth3d::Player::startRow() const
{
    return (std::get<0>(start_) - 1) / 2;
}

size_t Labyrinth3d::Player::startColumn() const
{
    return (std::get<1>(start_) - 1) / 2;
}

size_t Labyrinth3d::Player::startFloor() const
{
    return (std::get<2>(start_) - 1) / 2;
}

std::vector<size_t> Labyrinth3d::Player::finishRows() const
{
	std::vector<size_t> rows;
    rows.reserve(finishes_.size());

    for (auto const& f : finishes_)
        rows.emplace_back((std::get<0>(f) - 1) / 2);

    return rows;
}

std::vector<size_t> Labyrinth3d::Player::finishColumns() const
{
	std::vector<size_t> columns;
    columns.reserve(finishes_.size());

    for (auto const& f : finishes_)
        columns.emplace_back((std::get<1>(f) - 1) / 2);

    return columns;
}

std::vector<size_t> Labyrinth3d::Player::finishFloors() const
{
	std::vector<size_t> floors;
    floors.reserve(finishes_.size());

    for (auto const& f : finishes_)
        floors.emplace_back((std::get<2>(f) - 1) / 2);

    return floors;
}

size_t Labyrinth3d::Player::row() const
{
    return (std::get<0>(current_) - 1) / 2;
}

size_t Labyrinth3d::Player::column() const
{
    return (std::get<1>(current_) - 1) / 2;
}

size_t Labyrinth3d::Player::floor() const
{
    return (std::get<2>(current_) - 1) / 2;
}

size_t Labyrinth3d::Player::movements() const
{
    return movements_;
}

std::chrono::milliseconds const& Labyrinth3d::Player::finishingDuration() const
{
    return finishingDuration_;
}

std::chrono::milliseconds const& Labyrinth3d::Player::solvingDuration() const
{
    return solvingDuration_;
}

size_t Labyrinth3d::Player::state() const
{
    return state_;
}

bool Labyrinth3d::Player::enabledTrace() const
{
    return enabledTrace_;
}

void Labyrinth3d::Player::enableTrace(bool enabledTrace)
{
    enabledTrace_ = enabledTrace;

    if (!enabledTrace_)
        traceIntersections_.clear();
}

bool Labyrinth3d::Player::keptFullTrace() const
{
    return keptFullTrace_;
}

void Labyrinth3d::Player::keepFullTrace(bool keptFullTrace)
{
    keptFullTrace_ = keptFullTrace;

    if (!keptFullTrace_)
		fullTrace_.clear();
}

std::vector<std::tuple<size_t, size_t, size_t> > const& Labyrinth3d::Player::traceIntersections() const
{
    return traceIntersections_;
}

bool Labyrinth3d::Player::restart()
{
    if (state_ & Solving)
        return false;

    current_ = start_;
    movements_ = 0;
    state_ = 0;
    finishingDuration_ = std::chrono::milliseconds();
    solvingDuration_ = std::chrono::milliseconds();
    startTime_ = std::chrono::time_point<std::chrono::steady_clock>();
    traceIntersections_.clear();

    return true;
}

Labyrinth3d::Labyrinth const& Labyrinth3d::Player::labyrinth() const
{
    return labyrinth_;
}

bool Labyrinth3d::Player::blockingFinish() const
{
    return blockingFinish_;
}

void Labyrinth3d::Player::changeBlockingFinish(bool blockingFinish)
{
    if (state_ & Started)
        return;

    blockingFinish_ = blockingFinish;
}

void Labyrinth3d::Player::stopSolving()
{
    state_ |= StoppedSolving;
}

std::vector<std::tuple<size_t, size_t, size_t> > const& Labyrinth3d::Player::fullTrace() const
{
	return fullTrace_;
}

size_t Labyrinth3d::Player::move(Direction direction,
                                 std::function<void(std::chrono::milliseconds)> const& sleep,
                                 size_t movements, size_t cycleOperations, std::chrono::milliseconds cyclePause)
{
    if ((labyrinth_.state() & Labyrinth::Generating) || (blockingFinish_ && (state_ & Finished)) || (state_ & Moving))
        return 0;

    state_ |= Moving;

    auto const pos{current_};

    size_t realizedMovements{0};

    while (movements)
    {
        if (cycleOperations && cyclePause.count() && !(realizedMovements % cycleOperations))
            sleep(cyclePause);

        auto const posTmp{current_};

        switch (direction)
        {
            case Front:
                ++std::get<1>(current_);
                break;

            case Right:
                --std::get<0>(current_);
                break;

            case Back:
                --std::get<1>(current_);
                break;

            case Left:
                ++std::get<0>(current_);
                break;

            case Up:
                ++std::get<2>(current_);
                break;

            case Down:
                --std::get<2>(current_);
                break;
        }

        if (labyrinth_.grid()(current_))
        {
            current_ = posTmp;
            break;
        }
        else
        {
            if (keptFullTrace_)
                fullTrace_.emplace_back(posTmp);

            ++realizedMovements;
            --movements;

            bool finished{false};

            for (auto const& f : finishes_)
            {
                if (current_ == f)
                {
                    finished = true;
                    break;
                }
            }

            if (finished)
                break;
        }
    }

    if (!(state_ & Started))
    {
        startTime_ = std::chrono::steady_clock::now();

        state_ |= Started;
    }

    if (!(state_ & Finished))
    {
        bool finished{false};

        for (auto const& f : finishes_)
        {
            if (current_ == f)
            {
                finished = true;
                break;
            }
        }

        if (finished)
        {
            state_ |= Finished;
            finishingDuration_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime_);
        }
    }

    movements_ += realizedMovements;

    if (!traceIntersections_.empty())
    {
        if (traceIntersections_.back() == current_)
            traceIntersections_.pop_back();
        else
        {
            if ((std::get<0>(traceIntersections_.back()) == std::get<0>(pos) && std::get<0>(pos) != std::get<0>(current_))
                || (std::get<1>(traceIntersections_.back()) == std::get<1>(pos) && std::get<1>(pos) != std::get<1>(current_))
                || (std::get<2>(traceIntersections_.back()) == std::get<2>(pos) && std::get<2>(pos) != std::get<2>(current_)))
                traceIntersections_.push_back(pos);
        }
    }
    else
        traceIntersections_.push_back(pos);

    state_ &= ~Moving;

    return realizedMovements;
}

size_t Labyrinth3d::Player::stepBack(std::function<void(std::chrono::milliseconds)> const& sleep,
                                     size_t movements, size_t cycleOperations,
                                     std::chrono::milliseconds const& cyclePause)
{
    size_t operations{0};

    while ((!(state_ & Player::Finished) || !blockingFinish_)
           && !traceIntersections_.empty() && (!movements || (operations < movements)))
    {
        if (cycleOperations && cyclePause.count() && !(operations % cycleOperations))
            sleep(cyclePause);

        if (current_ == traceIntersections_.back())
        {
            traceIntersections_.pop_back();
            continue;
        }

        Direction direction;

        if (std::get<0>(current_) == std::get<0>(traceIntersections_.back()))
        {
            if (std::get<1>(current_) == std::get<1>(traceIntersections_.back()))
            {
                if (std::get<2>(current_) < std::get<2>(traceIntersections_.back()))
                    direction = Up;
                else if (std::get<2>(current_) > std::get<2>(traceIntersections_.back()))
                    direction = Down;
                else
                    assert(0);
            }
            else if (std::get<2>(current_) == std::get<2>(traceIntersections_.back()))
            {
                if (std::get<1>(current_) < std::get<1>(traceIntersections_.back()))
                    direction = Front;
                else if (std::get<1>(current_) > std::get<1>(traceIntersections_.back()))
                    direction = Back;
                else
                    assert(0);
            }
            else
                assert(0);
        }
        else if (std::get<1>(current_) == std::get<1>(traceIntersections_.back()))
        {
            if (std::get<0>(current_) == std::get<0>(traceIntersections_.back()))
            {
                if (std::get<2>(current_) < std::get<2>(traceIntersections_.back()))
                    direction = Up;
                else if (std::get<2>(current_) > std::get<2>(traceIntersections_.back()))
                    direction = Down;
                else
                    assert(0);
            }
            else if (std::get<2>(current_) == std::get<2>(traceIntersections_.back()))
            {
                if (std::get<0>(current_) < std::get<0>(traceIntersections_.back()))
                    direction = Left;
                else if (std::get<0>(current_) > std::get<0>(traceIntersections_.back()))
                    direction = Right;
                else
                    assert(0);
            }
            else
                assert(0);
        }
        else
            assert(0);

        if (move(direction, sleep))
            ++operations;
    }

    return operations;
}
