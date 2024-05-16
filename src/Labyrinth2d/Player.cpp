#include <thread>
#include <cassert>

#include "Labyrinth2d/Player.h"
#include "Labyrinth2d/Labyrinth.h"

Labyrinth2d::Player::Player(Labyrinth const& labyrinth,
                            size_t startI, size_t startJ,
                            std::vector<size_t> const& finishI, std::vector<size_t> const& finishJ,
                            bool enabledTrace, bool blockingFinish,
                            bool keptFullTrace) : startI_{startI}, startJ_{startJ},
                                                  finishI_{finishI}, finishJ_{finishJ},
                                                  i_{startI_}, j_{startJ_},
                                                  labyrinth_{labyrinth}, movements_{0},
                                                  state_{0},
                                                  enabledTrace_{enabledTrace},
                                                  blockingFinish_{blockingFinish}, keptFullTrace_{keptFullTrace}
{
    enableTrace(enabledTrace);
	
	assert(finishI_.size() && finishI_.size() == finishJ_.size());
	
	for (std::size_t i{0}; i < finishI_.size(); ++i)
	{
		if (i_ == finishI_[i] && j_ == finishJ_[i])
		{
			state_ |= State::Finished;
			break;
		}
	}
}

Labyrinth2d::Player::~Player()
{
    while (state_ & Solving)
    {
        stopSolving();

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
        std::this_thread::sleep_for(dummyThreadSpleepingDuration);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1
    }
}

size_t Labyrinth2d::Player::startI() const
{
    return startI_;
}

size_t Labyrinth2d::Player::startJ() const
{
    return startJ_;
}

std::vector<size_t> const& Labyrinth2d::Player::finishI() const
{
    return finishI_;
}

std::vector<size_t> const& Labyrinth2d::Player::finishJ() const
{
    return finishJ_;
}

size_t Labyrinth2d::Player::i() const
{
    return i_;
}

size_t Labyrinth2d::Player::j() const
{
    return j_;
}

size_t Labyrinth2d::Player::startRow() const
{
    return (startI_ - 1) / 2;
}

size_t Labyrinth2d::Player::startColumn() const
{
    return (startJ_ - 1) / 2;
}

std::vector<size_t> Labyrinth2d::Player::finishRows() const
{
	std::vector<size_t> rows;
	rows.reserve(finishI_.size());
	
	for (auto&& i : finishI_)
		rows.emplace_back((i - 1) / 2);
	
    return rows;
}

std::vector<size_t> Labyrinth2d::Player::finishColumns() const
{
	std::vector<size_t> columns;
	columns.reserve(finishJ_.size());
	
	for (auto&& j : finishJ_)
		columns.emplace_back((j - 1) / 2);
	
    return columns;
}

size_t Labyrinth2d::Player::row() const
{
    return (i_ - 1) / 2;
}

size_t Labyrinth2d::Player::column() const
{
    return (j_ - 1) / 2;
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

size_t Labyrinth2d::Player::move(Direction direction, size_t movements, size_t operationsCycle, std::chrono::milliseconds cyclePause)
{
    if ((labyrinth_.state() & Labyrinth::Generating) || (blockingFinish_ && (state_ & Finished)) || (state_ & Moving))
        return 0;

    state_ |= Moving;
	
    size_t const i(i_);
    size_t const j(j_);

    size_t realizedMovements(0);

    while (movements)
    {
#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
        if (operationsCycle && cyclePause.count() && !(realizedMovements % operationsCycle))
            std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

        size_t const iTmp(i_);
        size_t const jTmp(j_);

        switch (direction)
        {
            case Up:
                --i_;
                break;

            case Right:
                ++j_;
                break;

            case Down:
                ++i_;
                break;

            case Left:
                --j_;
                break;
        }

        if (labyrinth_.grid()(i_, j_))
        {
            i_ = iTmp;
            j_ = jTmp;
            break;
        }
        else
        {
			if (keptFullTrace_)
				fullTrace_.emplace_back(std::make_pair(iTmp, jTmp));

            ++realizedMovements;
            --movements;

			bool finished{false};

            for (std::size_t n{0}; n < finishI_.size(); ++n)
			{
                if (i_ == finishI_[n] && j_ == finishJ_[n])
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
		
        for (std::size_t n{0}; n < finishI_.size(); ++n)
		{
            if (i_ == finishI_[n] && j_ == finishJ_[n])
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
            if (traceIntersections_.back().first == i_ && traceIntersections_.back().second == j_)
                traceIntersections_.pop_back();
            else
            {
                if ((traceIntersections_.back().first == i && i != i_) || (traceIntersections_.back().second == j && j != j_))
                    traceIntersections_.push_back(std::make_pair(i, j));
            }
        }
        else
            traceIntersections_.push_back(std::make_pair(i, j));
    }

    state_ &= ~Moving;

    return realizedMovements;
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

    i_ = startI_;
    j_ = startJ_;
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

size_t Labyrinth2d::Player::stepBack(size_t movements, size_t operationsCycle, std::chrono::milliseconds cyclePause)
{
    size_t operations(0);

    while ((!(state_ & Player::Finished) || !blockingFinish_)
           && !traceIntersections_.empty() && (!movements || (operations < movements)))
    {
#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
                if (operationsCycle && cyclePause.count() && !(operations % operationsCycle))
                    std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

        if (i_ == traceIntersections_.back().first && j_ == traceIntersections_.back().second)
        {
            traceIntersections_.pop_back();
            continue;
        }

        Direction direction;

        if (i_ == traceIntersections_.back().first)
        {
            if (j_ < traceIntersections_.back().second)
                direction = Right;
            else if (j_ > traceIntersections_.back().second)
                direction = Left;
            else
                assert(0);
        }
        else if (j_ == traceIntersections_.back().second)
        {
            if (i_ < traceIntersections_.back().first)
                direction = Down;
            else if (i_ > traceIntersections_.back().first)
                direction = Up;
            else
                assert(0);
        }
        else
            assert(0);

        if (move(direction))
            ++operations;
    }

    return operations;
}

void Labyrinth2d::Player::stopSolving()
{
    state_ |= StoppedSolving;
}

std::vector<std::pair<size_t, size_t> > const& Labyrinth2d::Player::fullTrace() const
{
	return fullTrace_;
}
