#include <QTest>

class TestAlgorithmRecursiveDivision : public QObject
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
};

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Algorithm/RecursiveDivision.h"
#include "Labyrinth2d/Renderer/String.h"
#include "Labyrinth2d/Solver/Solver.h"

#include <random>
#include <thread>

void TestAlgorithmRecursiveDivision::generate1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmRecursiveDivision::generate2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestAlgorithmRecursiveDivision::generate1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;
	
    l.generate(g, rda);

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestAlgorithmRecursiveDivision::generate2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);
}

void TestAlgorithmRecursiveDivision::solveAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmRecursiveDivision::solveLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmRecursiveDivision::solveRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmRecursiveDivision::solveBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Algorithm::RecursiveDivision rda;

    l.generate(g, rda);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

QTEST_MAIN(TestAlgorithmRecursiveDivision)
#include "testalgorithmrecursivedivision.moc"
