#include <cassert>

#include "Labyrinth3d/Player.h"
#include "Labyrinth3d/Labyrinth.h"

Labyrinth3d::Player::Player(Labyrinth const& labyrinth,
                            size_t startI, size_t startJ, size_t startK,
                            std::vector<size_t> const& finishI, std::vector<size_t> const&finishJ, std::vector<size_t> const&finishK,
                            bool enabledTrace, bool blockingFinish,
                            bool keptFullTrace) : startI_{startI}, startJ_{startJ}, startK_(startK),
                                                  finishI_{finishI}, finishJ_{finishJ}, finishK_{finishK},
                                                  i_{startI_}, j_{startJ_}, k_{startK_},
                                                  labyrinth_{labyrinth}, movements_{0},
                                                  state_{0},
                                                  enabledTrace_{enabledTrace},
                                                  blockingFinish_{blockingFinish}, keptFullTrace_{keptFullTrace}
{
    enableTrace(enabledTrace);

	assert(finishI_.size() && finishI_.size() == finishJ_.size() && finishI_.size() == finishK_.size());

	for (std::size_t i{0}; i < finishI_.size(); ++i)
	{
		if (i_ == finishI_[i] && j_ == finishJ_[i] && k_ == finishK_[i])
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
    return startI_;
}

size_t Labyrinth3d::Player::startJ() const
{
    return startJ_;
}

size_t Labyrinth3d::Player::startK() const
{
    return startK_;
}

std::vector<size_t> const& Labyrinth3d::Player::finishI() const
{
    return finishI_;
}

std::vector<size_t> const& Labyrinth3d::Player::finishJ() const
{
    return finishJ_;
}

std::vector<size_t> const& Labyrinth3d::Player::finishK() const
{
    return finishK_;
}

size_t Labyrinth3d::Player::i() const
{
    return i_;
}

size_t Labyrinth3d::Player::j() const
{
    return j_;
}

size_t Labyrinth3d::Player::k() const
{
    return k_;
}

size_t Labyrinth3d::Player::startRow() const
{
    return (startI_ - 1) / 2;
}

size_t Labyrinth3d::Player::startColumn() const
{
    return (startJ_ - 1) / 2;
}

size_t Labyrinth3d::Player::startFloor() const
{
    return (startK_ - 1) / 2;
}

std::vector<size_t> Labyrinth3d::Player::finishRows() const
{
	std::vector<size_t> rows;
	rows.reserve(finishI_.size());

	for (auto&& i : finishI_)
		rows.emplace_back((i - 1) / 2);

    return rows;
}

std::vector<size_t> Labyrinth3d::Player::finishColumns() const
{
	std::vector<size_t> columns;
	columns.reserve(finishJ_.size());

	for (auto&& j : finishJ_)
		columns.emplace_back((j - 1) / 2);

    return columns;
}

std::vector<size_t> Labyrinth3d::Player::finishFloors() const
{
	std::vector<size_t> floors;
	floors.reserve(finishK_.size());

	for (auto&& k : finishK_)
		floors.emplace_back((k - 1) / 2);

    return floors;
}

size_t Labyrinth3d::Player::row() const
{
    return (i_ - 1) / 2;
}

size_t Labyrinth3d::Player::column() const
{
    return (j_ - 1) / 2;
}

size_t Labyrinth3d::Player::floor() const
{
    return (k_ - 1) / 2;
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

    i_ = startI_;
    j_ = startJ_;
    k_ = startK_;
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
                                 size_t movements, size_t operationsCycle, std::chrono::milliseconds cyclePause)
{
    if ((labyrinth_.state() & Labyrinth::Generating) || (blockingFinish_ && (state_ & Finished)) || (state_ & Moving))
        return 0;

    state_ |= Moving;

    size_t const i(i_);
    size_t const j(j_);
    size_t const k(k_);

    size_t realizedMovements(0);

    while (movements)
    {
        if (operationsCycle && cyclePause.count() && !(realizedMovements % operationsCycle))
            sleep(cyclePause);

        size_t const iTmp(i_);
        size_t const jTmp(j_);
        size_t const kTmp(k_);

        switch (direction)
        {
            case Front:
                ++j_;
                break;

            case Right:
                --i_;
                break;

            case Back:
                --j_;
                break;

            case Left:
                ++i_;
                break;

            case Up:
                ++k_;
                break;

            case Down:
                --k_;
                break;
        }

        if (labyrinth_.grid()(i_, j_, k_))
        {
            i_ = iTmp;
            j_ = jTmp;
            k_ = kTmp;
            break;
        }
        else
        {
            if (keptFullTrace_)
                fullTrace_.emplace_back(std::make_tuple(iTmp, jTmp, kTmp));

            ++realizedMovements;
            --movements;

            bool finished{false};

            for (std::size_t n{0}; n < finishI_.size(); ++n)
            {
                if (i_ == finishI_[n] && j_ == finishJ_[n] && k_ == finishK_[n])
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
            if (i_ == finishI_[n] && j_ == finishJ_[n] && k_ == finishK_[n])
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
        if (std::get<0>(traceIntersections_.back()) == i_
            && std::get<1>(traceIntersections_.back()) == j_
            && std::get<2>(traceIntersections_.back()) == k_)
            traceIntersections_.pop_back();
        else
        {
            if ((std::get<0>(traceIntersections_.back()) == i && i != i_)
                || (std::get<1>(traceIntersections_.back()) == j && j != j_)
                || (std::get<2>(traceIntersections_.back()) == k && k != k_))
                traceIntersections_.push_back(std::make_tuple(i, j, k));
        }
    }
    else
        traceIntersections_.push_back(std::make_tuple(i, j, k));

    state_ &= ~Moving;

    return realizedMovements;
}

size_t Labyrinth3d::Player::stepBack(std::function<void(std::chrono::milliseconds)> const& sleep,
                                     size_t movements, size_t operationsCycle,
                                     std::chrono::milliseconds const& cyclePause)
{
    size_t operations(0);

    while ((!(state_ & Player::Finished) || !blockingFinish_)
           && !traceIntersections_.empty() && (!movements || (operations < movements)))
    {
        if (operationsCycle && cyclePause.count() && !(operations % operationsCycle))
            sleep(cyclePause);

        if (i_ == std::get<0>(traceIntersections_.back())
            && j_ == std::get<1>(traceIntersections_.back())
            && k_ == std::get<2>(traceIntersections_.back()))
        {
            traceIntersections_.pop_back();
            continue;
        }

        Direction direction;

        if (i_ == std::get<0>(traceIntersections_.back()))
        {
            if (j_ == std::get<1>(traceIntersections_.back()))
            {
                if (k_ < std::get<2>(traceIntersections_.back()))
                    direction = Up;
                else if (k_ > std::get<2>(traceIntersections_.back()))
                    direction = Down;
                else
                    assert(0);
            }
            else if (k_ == std::get<2>(traceIntersections_.back()))
            {
                if (j_ < std::get<1>(traceIntersections_.back()))
                    direction = Front;
                else if (j_ > std::get<1>(traceIntersections_.back()))
                    direction = Back;
                else
                    assert(0);
            }
            else
                assert(0);
        }
        else if (j_ == std::get<1>(traceIntersections_.back()))
        {
            if (i_ == std::get<0>(traceIntersections_.back()))
            {
                if (k_ < std::get<2>(traceIntersections_.back()))
                    direction = Up;
                else if (k_ > std::get<2>(traceIntersections_.back()))
                    direction = Down;
                else
                    assert(0);
            }
            else if (k_ == std::get<2>(traceIntersections_.back()))
            {
                if (i_ < std::get<0>(traceIntersections_.back()))
                    direction = Left;
                else if (i_ > std::get<0>(traceIntersections_.back()))
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
