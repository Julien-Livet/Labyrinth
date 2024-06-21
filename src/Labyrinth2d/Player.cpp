#include <cassert>

#include "Labyrinth2d/Player.h"
#include "Labyrinth2d/Labyrinth.h"

Labyrinth2d::Player::Player(Labyrinth const& labyrinth,
                            size_t startI, size_t startJ,
                            std::vector<size_t> const& finishI, std::vector<size_t> const& finishJ,
                            bool enabledTrace, bool blockingFinish,
                            bool keptFullTrace) : start_{std::make_pair(startI, startJ)},
                                                  finishes_{},
                                                  current_{start_},
                                                  labyrinth_{labyrinth}, movements_{0},
                                                  state_{0},
                                                  enabledTrace_{enabledTrace},
                                                  blockingFinish_{blockingFinish}, keptFullTrace_{keptFullTrace}
{
    enableTrace(enabledTrace);
	
    assert(finishI.size() && finishI.size() == finishJ.size());

    finishes_.reserve(finishI.size());

    for (std::size_t i{0}; i < finishI.size(); ++i)
        finishes_.emplace_back(std::make_pair(finishI[i], finishJ[i]));

    for (auto const& p : finishes_)
	{
        if (p == current_)
		{
			state_ |= State::Finished;
			break;
		}
	}
}

Labyrinth2d::Player::Player(Labyrinth const& labyrinth,
                            std::pair<size_t, size_t> const& start,
                            std::vector<std::pair<size_t, size_t> > const& finishes,
                            bool enabledTrace, bool blockingFinish,
                            bool keptFullTrace) : start_{start},
                                                  finishes_{finishes},
                                                  current_{start},
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

Labyrinth2d::Player::~Player()
{
    while (state_ & Solving)
        stopSolving();
}

size_t Labyrinth2d::Player::startI() const
{
    return start_.first;
}

size_t Labyrinth2d::Player::startJ() const
{
    return start_.second;
}

std::pair<size_t, size_t> const& Labyrinth2d::Player::start() const
{
    return start_;
}

std::vector<size_t> Labyrinth2d::Player::finishI() const
{
    std::vector<size_t> finishI;
    finishI.reserve(finishes_.size());

    for (auto p : finishes_)
        finishI.emplace_back(p.first);

    return finishI;
}

std::vector<size_t> Labyrinth2d::Player::finishJ() const
{
    std::vector<size_t> finishJ;
    finishJ.reserve(finishes_.size());

    for (auto p : finishes_)
        finishJ.emplace_back(p.second);

    return finishJ;
}

std::vector<std::pair<size_t, size_t> > const& Labyrinth2d::Player::finishes() const
{
    return finishes_;
}

size_t Labyrinth2d::Player::i() const
{
    return current_.first;
}

size_t Labyrinth2d::Player::j() const
{
    return current_.second;
}

std::pair<size_t, size_t> const& Labyrinth2d::Player::current() const
{
    return current_;
}

size_t Labyrinth2d::Player::startRow() const
{
    return (start_.first - 1) / 2;
}

size_t Labyrinth2d::Player::startColumn() const
{
    return (start_.second - 1) / 2;
}

std::vector<size_t> Labyrinth2d::Player::finishRows() const
{
	std::vector<size_t> rows;
    rows.reserve(finishes_.size());
	
    for (auto const& p : finishes_)
        rows.emplace_back((p.first - 1) / 2);
	
    return rows;
}

std::vector<size_t> Labyrinth2d::Player::finishColumns() const
{
	std::vector<size_t> columns;
    columns.reserve(finishes_.size());
	
    for (auto const& p : finishes_)
        columns.emplace_back((p.second - 1) / 2);
	
    return columns;
}

size_t Labyrinth2d::Player::row() const
{
    return (current_.first - 1) / 2;
}

size_t Labyrinth2d::Player::column() const
{
    return (current_.second - 1) / 2;
}

size_t Labyrinth2d::Player::movements() const
{
    return movements_;
}

std::chrono::milliseconds const& Labyrinth2d::Player::finishingDuration() const
{
    return finishingDuration_;
}

std::chrono::milliseconds const& Labyrinth2d::Player::solvingDuration() const
{
    return solvingDuration_;
}

size_t Labyrinth2d::Player::state() const
{
    return state_;
}

bool Labyrinth2d::Player::enabledTrace() const
{
    return enabledTrace_;
}

void Labyrinth2d::Player::enableTrace(bool enabledTrace)
{
    enabledTrace_ = enabledTrace;

    if (!enabledTrace_)
        traceIntersections_.clear();
}

bool Labyrinth2d::Player::keptFullTrace() const
{
    return keptFullTrace_;
}

void Labyrinth2d::Player::keepFullTrace(bool keptFullTrace)
{
    keptFullTrace_ = keptFullTrace;
	
    if (!keptFullTrace_)
		fullTrace_.clear();
}

std::vector<std::pair<size_t, size_t> > const& Labyrinth2d::Player::traceIntersections() const
{
    return traceIntersections_;
}

bool Labyrinth2d::Player::restart()
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

Labyrinth2d::Labyrinth const& Labyrinth2d::Player::labyrinth() const
{
    return labyrinth_;
}

bool Labyrinth2d::Player::blockingFinish() const
{
    return blockingFinish_;
}

void Labyrinth2d::Player::changeBlockingFinish(bool blockingFinish)
{
    if (state_ & Started)
        return;

    blockingFinish_ = blockingFinish;
}

void Labyrinth2d::Player::stopSolving()
{
    state_ |= StoppedSolving;
}

std::vector<std::pair<size_t, size_t> > const& Labyrinth2d::Player::fullTrace() const
{
	return fullTrace_;
}

size_t Labyrinth2d::Player::move(Direction direction,
                                 std::function<void(std::chrono::milliseconds)> const& sleep,
                                 size_t movements, size_t cycleOperations,
                                 std::chrono::milliseconds cyclePause)
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
            case Up:
                --current_.first;
                break;

            case Right:
                ++current_.second;
                break;

            case Down:
                ++current_.first;
                break;

            case Left:
                --current_.second;
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

            for (auto const& p : finishes_)
            {
                if (p == current_)
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

        for (auto const& p : finishes_)
        {
            if (p == current_)
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

    if (enabledTrace_)
    {
        if (!traceIntersections_.empty())
        {
            if (traceIntersections_.back() == current_)
                traceIntersections_.pop_back();
            else
            {
                if ((traceIntersections_.back().first == pos.first && pos.first != current_.first)
                    || (traceIntersections_.back().second == pos.second && pos.second != current_.second))
                    traceIntersections_.emplace_back(pos);
            }
        }
        else
            traceIntersections_.emplace_back(pos);
    }

    state_ &= ~Moving;

    return realizedMovements;
}

size_t Labyrinth2d::Player::stepBack(std::function<void(std::chrono::milliseconds)> const& sleep,
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

        if (current_.first == traceIntersections_.back().first)
        {
            if (current_.second < traceIntersections_.back().second)
                direction = Right;
            else if (current_.second > traceIntersections_.back().second)
                direction = Left;
            else
                assert(0);
        }
        else if (current_.second == traceIntersections_.back().second)
        {
            if (current_.first < traceIntersections_.back().first)
                direction = Down;
            else if (current_.first > traceIntersections_.back().first)
                direction = Up;
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
