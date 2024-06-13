#include <QTest>

class TestLabyrinth : public QObject
{
    Q_OBJECT

	private slots:
        void generateEmpty1x1();
        void generateEmpty2x1();
        void generateEmpty1x2();
        void generateEmpty2x2();
        void generateVerticalWalls2x2();
        void generateHorizontalWalls2x2();
        void generateVerticalAndHorizontalWalls2x2();
        void solveAStarEmpty1x1();
        void solveBlindEmpty1x1();
        void solveAStarEmpty2x1();
        void solveBlindEmpty2x1();
        void solveAStarEmpty1x2();
        void solveBlindEmpty1x2();
        void solveAStarEmpty2x2();
        void solveBlindEmpty2x2();
        void solveAStarVerticalWalls2x2();
        void solveAStarHorizontalWalls2x2();
        void solveAStarHorizontalAndHorizontalWalls2x2();
        void generateEmpty1x1x1();
        void solveAStarEmpty1x1x1();
        void solveBlindEmpty1x1x1();
        void solveAStarEmpty2x1x1();
        void solveBlindEmpty2x1x1();
        void solveAStarEmpty1x2x1();
        void solveBlindEmpty1x2x1();
        void solveAStarEmpty1x1x2();
        void solveBlindEmpty1x1x2();
        void solveAStarEmpty2x2x1();
        void solveBlindEmpty2x2x1();
        void solveAStarEmpty2x1x2();
        void solveBlindEmpty2x1x2();
        void solveAStarEmpty1x2x2();
        void solveBlindEmpty1x2x2();
        void solveAStarEmpty2x2x2();
        void solveBlindEmpty2x2x2();
};

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Renderer/String.h"
#include "Labyrinth2d/Solver/Solver.h"

#include <random>
#include <thread>

void TestLabyrinth::generateEmpty1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "###\n");
}

void TestLabyrinth::generateEmpty2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "###\n"
             "# #\n"
             "# #\n"
             "# #\n"
             "###\n");
}

void TestLabyrinth::generateEmpty1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestLabyrinth::generateEmpty2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "# # #\n"
             "#   #\n"
             "#####\n");
}

void TestLabyrinth::generateVerticalWalls2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};

    for (size_t j{2}; j < l.grid().width() - 1; j += 2)
    {
        for (size_t i{1}; i < l.grid().height(); i += 2)
            l.grid().set(i, j);
    }

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "# # #\n"
             "# # #\n"
             "# # #\n"
             "#####\n");
}

void TestLabyrinth::generateHorizontalWalls2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};

    for (size_t i{2}; i < l.grid().height() - 1; i += 2)
    {
        for (size_t j{1}; j < l.grid().width(); j += 2)
            l.grid().set(i, j);
    }

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "#   #\n"
             "#####\n"
             "#   #\n"
             "#####\n");
}

void TestLabyrinth::generateVerticalAndHorizontalWalls2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};

    for (size_t j{2}; j < l.grid().width() - 1; j += 2)
    {
        for (size_t i{1}; i < l.grid().height(); i += 2)
            l.grid().set(i, j);
    }

    for (size_t i{2}; i < l.grid().height() - 1; i += 2)
    {
        for (size_t j{1}; j < l.grid().width(); j += 2)
            l.grid().set(i, j);
    }

    Labyrinth2d::Renderer::String const sr{l, ' ', '#'};

    QCOMPARE(sr(),
             "#####\n"
             "# # #\n"
             "#####\n"
             "# # #\n"
             "#####\n");
}

void TestLabyrinth::solveAStarEmpty1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    auto const playerId{l.addPlayer(0, 0, {0}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    auto const playerId{l.addPlayer(0, 0, {0}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    auto const playerId{l.addPlayer(0, 0, {1}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty2x1()
{
    Labyrinth2d::Labyrinth l{2, 1};
    auto const playerId{l.addPlayer(0, 0, {1}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    auto const playerId{l.addPlayer(0, 0, {0}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty1x2()
{
    Labyrinth2d::Labyrinth l{1, 2};
    auto const playerId{l.addPlayer(0, 0, {0}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarVerticalWalls2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};

    for (size_t j{2}; j < l.grid().width() - 1; j += 2)
    {
        for (size_t i{1}; i < l.grid().height(); i += 2)
            l.grid().set(i, j);
    }

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};

    try
    {
        Labyrinth2d::Solver::AStar ass;
        l.player(playerId).solve(g, ass);
    }
    catch (Labyrinth2d::Solver::FailureException const& e)
    {
    }
}

void TestLabyrinth::solveAStarHorizontalWalls2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};

    for (size_t i{2}; i < l.grid().height() - 1; i += 2)
    {
        for (size_t j{1}; j < l.grid().width(); j += 2)
            l.grid().set(i, j);
    }

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};

    try
    {
        Labyrinth2d::Solver::AStar ass;
        l.player(playerId).solve(g, ass);
    }
    catch (Labyrinth2d::Solver::FailureException const& e)
    {
    }
}

void TestLabyrinth::solveAStarHorizontalAndHorizontalWalls2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};

    for (size_t j{2}; j < l.grid().width() - 1; j += 2)
    {
        for (size_t i{1}; i < l.grid().height(); i += 2)
            l.grid().set(i, j);
    }

    for (size_t i{2}; i < l.grid().height() - 1; i += 2)
    {
        for (size_t j{1}; j < l.grid().width(); j += 2)
            l.grid().set(i, j);
    }

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};

    try
    {
        Labyrinth2d::Solver::AStar ass;
        l.player(playerId).solve(g, ass);
    }
    catch (Labyrinth2d::Solver::FailureException const& e)
    {
    }
}

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/Solver/Solver.h"

void TestLabyrinth::generateEmpty1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};

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

void TestLabyrinth::solveAStarEmpty1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {0}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {0}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {0}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {0}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {1}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {1}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {0}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {0}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {0})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty2x1x2()
{
    Labyrinth3d::Labyrinth l{2, 1, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {0}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty2x1x2()
{
    Labyrinth3d::Labyrinth l{2, 1, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {0}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty1x2x2()
{
    Labyrinth3d::Labyrinth l{1, 2, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty1x2x2()
{
    Labyrinth3d::Labyrinth l{1, 2, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {0}, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestLabyrinth::solveAStarEmpty2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestLabyrinth::solveBlindEmpty2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    std::default_random_engine g{std::chrono::system_clock::now().time_since_epoch().count()};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

QTEST_MAIN(TestLabyrinth)
#include "testlabyrinth.moc"
