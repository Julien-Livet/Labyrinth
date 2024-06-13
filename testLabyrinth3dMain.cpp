#include <iostream>
#include <fstream>
#include <future>
#include <random>
#include <thread>

#include <QApplication>
#include <QGuiApplication>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimerEvent>
#include <QWidget>

#include "Labyrinth3d/Labyrinth.h"
#include "Labyrinth3d/Algorithm/Algorithm.h"
#include "Labyrinth3d/Qt/GlWidget.h"
#include "Labyrinth3d/Solver/Solver.h"

using namespace Labyrinth3d;

int main(int argc, char** argv)
{
    size_t const cycleOperations(0 * 1);
    std::chrono::milliseconds const cyclePause(0 * 1 * 1);
    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(10 * 50);

    //size_t const seed(1717503823494194900);
    size_t const seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine g(seed);
    std::cout << seed << std::endl;/*
    //std::default_random_engine g(156320);
    std::random_device g;*/

    auto const sleep{
        [] (std::chrono::milliseconds const& ms) -> void
        {
#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
            std::this_thread::sleep_for(ms);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1
        }
    };

    //Labyrinth l(2, 2, 2);
    //Labyrinth l(3, 3, 3);
    //Labyrinth l(5, 5, 5);
    Labyrinth l(9, 9, 9);
    //Labyrinth l(20, 20, 20);
/*
    //Not working
    Algorithm::Kruskal ka;
    l.generate(g, ka, sleep, cycleOperations, cyclePause);*/
    Algorithm::CellFusion cfa;
    l.generate(g, cfa, sleep, cycleOperations, cyclePause);/*
    //Not working
    Algorithm::RecursiveDivision rda;
    l.generate(g, rda, sleep, cycleOperations, cyclePause);*/
/*
    //Not working
    Algorithm::WaySearch wsa(Algorithm::WaySearch::DepthFirstSearch);
    //Algorithm::WaySearch wsa(Algorithm:WaySearch::Prim);
    //Algorithm::WaySearch wsa(Algorithm::WaySearch::HuntAndKill);

    //l.generate(g, wsa, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, Algorithm::WaySearch>,
                           &l, std::ref(g), std::ref(wsa), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*/
    QApplication application(argc, argv);

    std::string const path{"C:/Users/juju0/AppData/Roaming/FreeCAD/Macro"};

    QVector3D const wallsSize{5, 5, 5};
    QVector3D const waysSize{20, 20, 20}; //diameter of 16 mm for a school ball

    std::ofstream ofs{path + "/labyrinth3d.FCMacro"};

    if (ofs)
    {
        for (size_t k(0); k < l.grid().depth(); ++k)
        {
            for (size_t i(0); i < l.grid().height(); ++i)
            {
                for (size_t j(0); j < l.grid().width(); ++j)
                {
                    if (l.grid().at(i, j, k))
                    {
                        ofs << "cube = App.ActiveDocument.addObject(\"Part::Box\", \"Cube\")\n"
                            << "cube.Length = " << (j % 2 ? waysSize.x() : wallsSize.x()) << "\n"
                            << "cube.Width = " << (i % 2 ? waysSize.y() : wallsSize.y()) << "\n"
                            << "cube.Height = " << (k % 2 ? waysSize.z() : wallsSize.z()) << "\n"
                            << "cube.Placement = App.Placement(App.Vector(" << (j + 1) / 2 * wallsSize.x() + j / 2 * waysSize.x() << ", "
                                                                            << (i + 1) / 2 * wallsSize.y() + i / 2 * waysSize.y() << ", "
                                                                            << (k + 1) / 2 * wallsSize.z() + k / 2 * waysSize.z()
                                                                            << "), App.Rotation(App.Vector(0, 0, 1), 0))\n";
                    }
                }
            }
        }

        ofs.close();
    }

    size_t const player1Id(l.addPlayer(0, 0, 0, {l.grid().rows() - 1}, {l.grid().columns() - 1}, {l.grid().floors() - 1}, true));
    size_t const player2Id(l.addPlayer(l.grid().rows() - 1, l.grid().columns() - 1, l.grid().floors() - 1, {0}, {0}, {0}, true));
    size_t const player3Id(l.addPlayer(0, l.grid().columns() - 1, l.grid().floors() - 1, {l.grid().rows() - 1}, {0}, {0}, true));

    Solver::AStar ass;
/*
    //l.player(player1d).solve(g, ass, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer1(&Player::solve<std::default_random_engine, Solver::AStar>, &l.player(player1Id),
                               std::ref(g), std::ref(ass), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);
*/
    Solver::Blind bs;

    //l.player(player2Id).solve(g, bs, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer2(&Player::solve<std::default_random_engine, Solver::AStar>, &l.player(player2Id),
                               std::ref(g), std::ref(ass), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);
    //l.player(player3Id).solve(g, bs, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer3(&Player::solve<std::default_random_engine, Solver::Blind>, &l.player(player3Id),
                               std::ref(g), std::ref(bs), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);

    //thSolvePlayer1.detach();
    thSolvePlayer2.detach();
    thSolvePlayer3.detach();

    bool constexpr displayTrace{true};

    GLWidget glWidget(l, wallsSize, waysSize);
    glWidget.show();
    glWidget.setFixedSize(800, 600);
    glWidget.move(QRect(QPoint(), QGuiApplication::screenAt(QPoint())->size()).center() - glWidget.rect().center());
    glWidget.setClearColor(Qt::white);
    glWidget.setPlayerDisplay(player1Id, GLWidget::PlayerDisplay{QColor{255, 0, 0, 255}, displayTrace, QColor{255, 0, 0, 127}});
    glWidget.setPlayerDisplay(player2Id, GLWidget::PlayerDisplay{QColor{0, 255, 0, 255}, displayTrace, QColor{0, 255, 0, 127}});
    glWidget.setPlayerDisplay(player3Id, GLWidget::PlayerDisplay{QColor{0, 0, 255, 255}, displayTrace, QColor{0, 0, 255, 127}});

    auto const result{application.exec()};
/*
    //thSolvePlayer1.join();
    thSolvePlayer2.join();
    thSolvePlayer3.join();
*/
    std::cout << "Player 1 state: " << l.player(player1Id).state() << std::endl;
    std::cout << "Player 2 state: " << l.player(player2Id).state() << std::endl;
    std::cout << "Player 3 state: " << l.player(player3Id).state() << std::endl;

    if (l.state() & Labyrinth::Generated)
        std::cout << "Labyrinth generated in " << l.generationDuration().count() << " ms" << std::endl;

    if (l.player(player1Id).state() & Player::Solved)
        std::cout << "Player 1 solved in " << l.player(player1Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player1Id).state() & Player::Finished)
        std::cout << "Player 1 finished in " << l.player(player1Id).finishingDuration().count() << " ms" << std::endl;

    if (l.player(player2Id).state() & Player::Solved)
        std::cout << "Player 2 solved in " << l.player(player2Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player2Id).state() & Player::Finished)
        std::cout << "Player 2 finished in " << l.player(player2Id).finishingDuration().count() << " ms" << std::endl;

    if (l.player(player3Id).state() & Player::Solved)
        std::cout << "Player 3 solved in " << l.player(player3Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player3Id).state() & Player::Finished)
        std::cout << "Player 3 finished in " << l.player(player3Id).finishingDuration().count() << " ms" << std::endl;

    return result;//0;
}
