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
#include <thread>

void TestAlgorithmRandomSlicer::generate1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;
    Labyrinth2d::Algorithm::RecursiveDivision rda;
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};
    Labyrinth3d::Algorithm::RandomSlicer<
        Labyrinth2d::Algorithm::CellFusion,
        Labyrinth2d::Algorithm::RecursiveDivision,
        Labyrinth2d::Algorithm::WaySearch> rsa{cfa, rda, wsa};

    l.generate(g, rsa);
}

void TestAlgorithmRandomSlicer::generate2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
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
