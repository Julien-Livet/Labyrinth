#include <QTest>

class TestAlgorithmWaySearch : public QObject
{
    Q_OBJECT

	private slots:
        void generateDepthFirstSearch1x1();
        void generateDepthFirstSearch2x1();
        void generateDepthFirstSearch1x2();
        void generateDepthFirstSearch2x2();
        void generatePrim1x1();
        void generatePrim2x1();
        void generatePrim1x2();
        void generatePrim2x2();
        void generateHuntAndKill1x1();
        void generateHuntAndKill2x1();
        void generateHuntAndKill1x2();
        void generateHuntAndKill2x2();
        void solveDepthFirstSearchAStar2x2();
        void solveDepthFirstSearchLeftWallHand2x2();
        void solveDepthFirstSearchRightWallHand2x2();
        void solveDepthFirstSearchBlind2x2();
        void solvePrimAStar2x2();
        void solvePrimLeftWallHand2x2();
        void solvePrimRightWallHand2x2();
        void solvePrimBlind2x2();
        void solveHuntAndKillAStar2x2();
        void solveHuntAndKillLeftWallHand2x2();
        void solveHuntAndKillRightWallHand2x2();
        void solveHuntAndKillBlind2x2();
        void solveDepthFirstSearch8x8();
        void solvePrim8x8();
        void solveHuntAndKill8x8();

        void generatePrim1x1x1();
        void generatePrim2x1x1();
        void generatePrim1x2x1();
        void generatePrim1x1x2();
        void generatePrim2x2x1();
        void generatePrim2x1x2();
        void generatePrim1x2x2();
        void generatePrim2x2x2();
        void solvePrimAStar2x2x2();
        void solvePrimBlind2x2x2();
        void solvePrim8x8x8();
        void writeAndReadPrim8x8x8();
};

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Algorithm/WaySearch.h"
#include "Labyrinth2d/Renderer/String.h"
#include "Labyrinth2d/Solver/Solver.h"

#include <random>

void TestAlgorithmWaySearch::generateDepthFirstSearch1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmWaySearch::generateDepthFirstSearch2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmWaySearch::generateDepthFirstSearch1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};
	
    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmWaySearch::generateDepthFirstSearch2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::generatePrim1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmWaySearch::generatePrim2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmWaySearch::generatePrim1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmWaySearch::generatePrim2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::generateHuntAndKill1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmWaySearch::generateHuntAndKill2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmWaySearch::generateHuntAndKill1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmWaySearch::generateHuntAndKill2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::solveDepthFirstSearchAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::solveDepthFirstSearchLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmWaySearch::solveDepthFirstSearchRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmWaySearch::solveDepthFirstSearchBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmWaySearch::solvePrimAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::solvePrimLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmWaySearch::solvePrimRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmWaySearch::solvePrimBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmWaySearch::solveHuntAndKillAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::solveHuntAndKillLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmWaySearch::solveHuntAndKillRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmWaySearch::solveHuntAndKillBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmWaySearch::solveDepthFirstSearch8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::DepthFirstSearch};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::solvePrim8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::solveHuntAndKill8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::WaySearch wsa{Labyrinth2d::Algorithm::WaySearch::HuntAndKill};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/Algorithm/WaySearch.h"
#include "Labyrinth3d/Solver/Solver.h"

void TestAlgorithmWaySearch::generatePrim1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

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

void TestAlgorithmWaySearch::generatePrim2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

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

void TestAlgorithmWaySearch::generatePrim1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

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

void TestAlgorithmWaySearch::generatePrim1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

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

void TestAlgorithmWaySearch::generatePrim2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::generatePrim2x1x2()
{
    Labyrinth3d::Labyrinth l{2, 1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::generatePrim1x2x2()
{
    Labyrinth3d::Labyrinth l{1, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::generatePrim2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);
}

void TestAlgorithmWaySearch::solvePrimAStar2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(std::make_tuple(0, 0, 0),
                                    {std::make_tuple(1, 1, 1)})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::solvePrimBlind2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(std::make_tuple(0, 0, 0),
                                    {std::make_tuple(1, 1, 1)})};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmWaySearch::solvePrim8x8x8()
{
    Labyrinth3d::Labyrinth l{8, 8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l.generate(g, wsa);

    auto const playerId{l.addPlayer(std::make_tuple(0, 0, 0),
                                    {std::make_tuple(7, 7, 7)})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmWaySearch::writeAndReadPrim8x8x8()
{
    Labyrinth3d::Labyrinth l1{8, 8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::WaySearch wsa{Labyrinth3d::Algorithm::WaySearch::Prim};

    l1.generate(g, wsa);

    auto const playerId{l1.addPlayer(std::make_tuple(0, 0, 0),
                                     {std::make_tuple(7, 7, 7)},
                                     true)};
    Labyrinth3d::Solver::AStar ass;

    l1.player(playerId).solve(g, ass);

    std::queue<char> data;

    l1.write(data);

    Labyrinth3d::Labyrinth l2{1, 1, 1};

    l2.read(data);

    QCOMPARE(data.empty(), true);

    QCOMPARE(l1.grid(), l2.grid());
}

QTEST_MAIN(TestAlgorithmWaySearch)
#include "testalgorithmwaysearch.moc"
