#include <QTest>

class TestAlgorithmKruskal : public QObject
{
    Q_OBJECT

	private slots:
        void generateRecursive1x1();
        void generateRecursive2x1();
        void generateRecursive1x2();
        void generateRecursive2x2();
        void generateIterative1x1();
        void generateIterative2x1();
        void generateIterative1x2();
        void generateIterative2x2();
        void solveRecursiveAStar2x2();
        void solveRecursiveLeftWallHand2x2();
        void solveRecursiveRightWallHand2x2();
        void solveRecursiveBlind2x2();
        void solveIterativeAStar2x2();
        void solveIterativeLeftWallHand2x2();
        void solveIterativeRightWallHand2x2();
        void solveIterativeBlind2x2();
        void solveRecursive8x8();
        void solveIterative8x8();
};

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Algorithm/Kruskal.h"
#include "Labyrinth2d/Renderer/String.h"
#include "Labyrinth2d/Solver/Solver.h"

#include <random>

void TestAlgorithmKruskal::generateRecursive1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmKruskal::generateRecursive2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};
	
    l.generate(g, ka);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmKruskal::generateRecursive1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};
	
    l.generate(g, ka);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmKruskal::generateRecursive2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);
}

void TestAlgorithmKruskal::generateIterative1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmKruskal::generateIterative2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmKruskal::generateIterative1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmKruskal::generateIterative2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);
}

void TestAlgorithmKruskal::solveRecursiveAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmKruskal::solveRecursiveLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmKruskal::solveRecursiveRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmKruskal::solveRecursiveBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmKruskal::solveIterativeAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmKruskal::solveIterativeLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmKruskal::solveIterativeRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmKruskal::solveIterativeBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmKruskal::solveRecursive8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Recursive};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmKruskal::solveIterative8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::Kruskal ka{Labyrinth2d::Algorithm::Kruskal::Iterative};

    l.generate(g, ka);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

QTEST_MAIN(TestAlgorithmKruskal)
#include "testalgorithmkruskal.moc"
