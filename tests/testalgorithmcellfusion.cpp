#include <QTest>

class TestAlgorithmCellFusion : public QObject
{
    Q_OBJECT

private slots:
    void generate1x1();
    void generate2x1();
    void generate1x2();
    void generate2x2();
    void solveAStar2x2();
    void solveLeftWallHand2x2();
    void solveRightWallHand2x2();
    void solveBlind2x2();
    void solve8x8();
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

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Algorithm/CellFusion.h"
#include "Labyrinth2d/Renderer/String.h"
#include "Labyrinth2d/Solver/Solver.h"

#include <random>
#include <thread>

void TestAlgorithmCellFusion::generate1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmCellFusion::generate2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmCellFusion::generate1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmCellFusion::generate2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::solveAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmCellFusion::solveLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmCellFusion::solveRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmCellFusion::solveBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmCellFusion::solve8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/Algorithm/CellFusion.h"
#include "Labyrinth3d/Solver/Solver.h"

void TestAlgorithmCellFusion::generate1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate2x1x2()
{
    Labyrinth3d::Labyrinth l{2, 1, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate1x2x2()
{
    Labyrinth3d::Labyrinth l{1, 2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::solveAStar2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmCellFusion::solveBlind2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmCellFusion::solve8x8x8()
{
    Labyrinth3d::Labyrinth l{8, 8, 8};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, 0, {7}, {7}, {7})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

QTEST_MAIN(TestAlgorithmCellFusion)
#include "testalgorithmcellfusion.moc"
