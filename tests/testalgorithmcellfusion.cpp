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
        void writeAndRead();
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
#include <sstream>

void TestAlgorithmCellFusion::generate1x1()
{
    Labyrinth2d::Labyrinth l{1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
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
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
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
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
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
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::solveAStar2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmCellFusion::solveLeftWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmCellFusion::solveRightWallHand2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};

    QCOMPARE(l.player(playerId).solve(g, whs), true);
}

void TestAlgorithmCellFusion::solveBlind2x2()
{
    Labyrinth2d::Labyrinth l{2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {1}, {1})};
    Labyrinth2d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmCellFusion::solve8x8()
{
    Labyrinth2d::Labyrinth l{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, {7}, {7})};
    Labyrinth2d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmCellFusion::writeAndRead()
{
    Labyrinth2d::Labyrinth l1{8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth2d::Algorithm::CellFusion cfa;

    l1.generate(g, cfa);

    auto const playerId{l1.addPlayer(0, 0, {7}, {7}, true)};
    Labyrinth2d::Solver::AStar ass;

    l1.player(playerId).solve(g, ass);

    std::queue<char> data;

    l1.write(data);

    Labyrinth2d::Labyrinth l2{1, 1};

    l2.read(data);

    QCOMPARE(data.empty(), true);

    Labyrinth2d::Renderer::String sr1{l1, ' ', '#'};
    sr1.playerRenderers[playerId] = Labyrinth2d::Renderer::String::PlayerRenderer('X', '.');
    sr1.playerRenderers[playerId].displayTrace = true;

    Labyrinth2d::Renderer::String sr2{l2, sr1.waysDirection, sr1.wallsDirection};
    sr2.playerRenderers[playerId] = sr1.playerRenderers[playerId];

    QCOMPARE(sr1(), sr2());
}

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/Algorithm/CellFusion.h"
#include "Labyrinth3d/Solver/Solver.h"

void TestAlgorithmCellFusion::generate1x1x1()
{
    Labyrinth3d::Labyrinth l{1, 1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

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

void TestAlgorithmCellFusion::generate2x1x1()
{
    Labyrinth3d::Labyrinth l{2, 1, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

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

void TestAlgorithmCellFusion::generate1x2x1()
{
    Labyrinth3d::Labyrinth l{1, 2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

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

void TestAlgorithmCellFusion::generate1x1x2()
{
    Labyrinth3d::Labyrinth l{1, 1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

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

void TestAlgorithmCellFusion::generate2x2x1()
{
    Labyrinth3d::Labyrinth l{2, 2, 1};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate2x1x2()
{
    Labyrinth3d::Labyrinth l{2, 1, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate1x2x2()
{
    Labyrinth3d::Labyrinth l{1, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::generate2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);
}

void TestAlgorithmCellFusion::solveAStar2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

void TestAlgorithmCellFusion::solveBlind2x2x2()
{
    Labyrinth3d::Labyrinth l{2, 2, 2};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, 0, {1}, {1}, {1})};
    Labyrinth3d::Solver::Blind bs;

    QCOMPARE(l.player(playerId).solve(g, bs), true);
}

void TestAlgorithmCellFusion::solve8x8x8()
{
    Labyrinth3d::Labyrinth l{8, 8, 8};
    std::default_random_engine g{static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    Labyrinth3d::Algorithm::CellFusion cfa;

    l.generate(g, cfa);

    auto const playerId{l.addPlayer(0, 0, 0, {7}, {7}, {7})};
    Labyrinth3d::Solver::AStar ass;

    QCOMPARE(l.player(playerId).solve(g, ass), true);
}

QTEST_MAIN(TestAlgorithmCellFusion)
#include "testalgorithmcellfusion.moc"
