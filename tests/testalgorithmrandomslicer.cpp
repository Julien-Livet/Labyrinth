#include <QTest>

class TestAlgorithmRandomSlicer : public QObject
{
    Q_OBJECT

    private slots:
        void generate1x1x1();
        void generate2x1x1();
        void generate1x2x1();
        void generate1x1x2();
        void generate2x2x1();
        void generate2x1x2();
        void generate1x2x2();
        void generate2x2x2();
        void solveAStar2x2x2();
        void solveBlind2x2x2();
        void solve8x8x8();
};

#include "Labyrinth2d/Algorithm/Algorithm.h"
#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/Algorithm/RandomSlicer.h"
#include "Labyrinth3d/Solver/Solver.h"

#include <random>

void TestAlgorithmRandomSlicer::generate1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(i, j, 0), true);
            QCOMPARE(l.grid()(i, j, l.grid().depth() - 1), true);
        }
    }

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t k{0}; k < l.grid().depth(); ++k)
        {
            QCOMPARE(l.grid()(i, 0, k), true);
            QCOMPARE(l.grid()(i, l.grid().width() - 1, k), true);
        }
    }

    for (size_t k{0}; k < l.grid().depth(); ++k)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(0, j, k), true);
            QCOMPARE(l.grid()(l.grid().height() - 1, j, k), true);
        }
    }

    QCOMPARE(l.grid()(1, 1, 1), false);
}

void TestAlgorithmRandomSlicer::generate2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(i, j, 0), true);
            QCOMPARE(l.grid()(i, j, l.grid().depth() - 1), true);
        }
    }

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t k{0}; k < l.grid().depth(); ++k)
        {
            QCOMPARE(l.grid()(i, 0, k), true);
            QCOMPARE(l.grid()(i, l.grid().width() - 1, k), true);
        }
    }

    for (size_t k{0}; k < l.grid().depth(); ++k)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(0, j, k), true);
            QCOMPARE(l.grid()(l.grid().height() - 1, j, k), true);
        }
    }

    QCOMPARE(l.grid()(1, 1, 1), false);
    QCOMPARE(l.grid()(2, 1, 1), false);
    QCOMPARE(l.grid()(3, 1, 1), false);
}

void TestAlgorithmRandomSlicer::generate1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(i, j, 0), true);
            QCOMPARE(l.grid()(i, j, l.grid().depth() - 1), true);
        }
    }

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t k{0}; k < l.grid().depth(); ++k)
        {
            QCOMPARE(l.grid()(i, 0, k), true);
            QCOMPARE(l.grid()(i, l.grid().width() - 1, k), true);
        }
    }

    for (size_t k{0}; k < l.grid().depth(); ++k)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(0, j, k), true);
            QCOMPARE(l.grid()(l.grid().height() - 1, j, k), true);
        }
    }

    QCOMPARE(l.grid()(1, 1, 1), false);
    QCOMPARE(l.grid()(1, 2, 1), false);
    QCOMPARE(l.grid()(1, 3, 1), false);
}

void TestAlgorithmRandomSlicer::generate1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(i, j, 0), true);
            QCOMPARE(l.grid()(i, j, l.grid().depth() - 1), true);
        }
    }

    for (size_t i{0}; i < l.grid().height(); ++i)
    {
        for (size_t k{0}; k < l.grid().depth(); ++k)
        {
            QCOMPARE(l.grid()(i, 0, k), true);
            QCOMPARE(l.grid()(i, l.grid().width() - 1, k), true);
        }
    }

    for (size_t k{0}; k < l.grid().depth(); ++k)
    {
        for (size_t j{0}; j < l.grid().width(); ++j)
        {
            QCOMPARE(l.grid()(0, j, k), true);
            QCOMPARE(l.grid()(l.grid().height() - 1, j, k), true);
        }
    }

    QCOMPARE(l.grid()(1, 1, 1), false);
    QCOMPARE(l.grid()(1, 1, 2), false);
    QCOMPARE(l.grid()(1, 1, 3), false);
}

void TestAlgorithmRandomSlicer::generate2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate2x1x2()
{
    Labyrinth3d::Labyrinth l{2, 1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate1x2x2()
{
    Labyrinth3d::Labyrinth l{1, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::solveAStar2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmRandomSlicer::solveBlind2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmRandomSlicer::solve8x8x8()
{
    Labyrinth3d::Labyrinth l{8, 8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);

    auto const playerId{l.addPlayer(0, 0, 0, {7}, {7}, {7})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

QTEST_MAIN(TestAlgorithmRandomSlicer)
#include "testalgorithmrandomslicer.moc"
