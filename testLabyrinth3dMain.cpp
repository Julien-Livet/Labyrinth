#include <fstream>
#include <future>
#include <iostream>
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

#include "include/Labyrinth3d/Labyrinth.h"
#include "include/Labyrinth3d/Algorithm/Algorithm.h"
#include "include/Labyrinth3d/Qt/GlWidget.h"
#include "include/Labyrinth3d/Solver/Solver.h"

using namespace Labyrinth3d;

int main(int argc, char** argv)
{
    size_t const cycleOperations(0 * 1);
    std::chrono::milliseconds const cyclePause(0 * 1 * 1);
    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(1 * 50);//1 * 50);

    //size_t const seed(2761453349);//3339863251);
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

    Labyrinth l(2, 2, 2);
    //Labyrinth l(5, 5, 5);
    //Labyrinth l(9, 9, 9);
/*
    //Not working
    Algorithm::Kruskal ka;
    l.generate(g, ka, sleep, cycleOperations, cyclePause);*/
    //Not working
    Algorithm::CellFusion cfa;
    l.generate(g, cfa, sleep, cycleOperations, cyclePause);/*
    //Not working
    Algorithm::RecursiveDivision rda;
    l.generate(g, rda, sleep, cycleOperations, cyclePause);*/

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

    Solver::AStar ass;

    //l.player(player1d).solve(g, ass, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer1(&Player::solve<std::default_random_engine, Solver::AStar>, &l.player(player1Id),
                               std::ref(g), std::ref(ass), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);

    Solver::Blind bs;

    //l.player(player2Id).solve(g, bs, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer2(&Player::solve<std::default_random_engine, Solver::Blind>, &l.player(player2Id),
                               std::ref(g), std::ref(bs), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);

    thSolvePlayer1.detach();
    thSolvePlayer2.detach();

    GLWidget glWidget(l);
    glWidget.show();
    glWidget.setFixedSize(800, 600);

    glWidget.setClearColor(Qt::white);
    glWidget.makeBox(QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 1.0, 1.0));
    //glWidget.makeBox(QVector3D(-0.2, -0.2, -0.2), QVector3D(0.0, 0.0, 0.0));
    //glWidget.makeBox(QVector3D(0.0, 0.0, 0.0), QVector3D(0.2, 0.2, 0.2));
    for (unsigned int i(0); i < 6; ++i)
    {
        glWidget.setTexture(0, i, QImage("C:/Users/juju0/Documents/GitHub/Labyrinth/resources/wall_pattern.png"));
        QImage image(128, 128, QImage::Format_ARGB32);
        //image.fill(Qt::black);
        image.fill(QColor(255, float(i) / 5 * 255, 0, 255));
        glWidget.setTexture(0, i, image);
    }/*
    for (unsigned int i(0); i < 6; ++i)
        glWidget.setTexture(1, i, QImage("C:/Users/juju0/Documents/GitHub/Labyrinth/resources/wall_pattern.png"));*/

    auto const result{application.exec()};
/*
    //thSolvePlayer1.join();
    thSolvePlayer2.join();
*/
    std::cout << "Player 1 state: " << l.player(player1Id).state() << std::endl;
    std::cout << "Player 2 state: " << l.player(player2Id).state() << std::endl;

    if (l.state() & Labyrinth::Generated)
        std::cout << "Labyrinth generated in " << l.generationDuration().count() << " ms" << std::endl;

    if (l.player(player1Id).state() & Player::Solved)
        std::cout << "Player 1 solved in " << l.player(player1Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player1Id).state() & Player::Finished)
        std::cout << "Player 1 finished in " << l.player(player1Id).finishingDuration().count() << " ms" << std::endl;

    if (l.player(player2Id).state() & Player::Solved)
        std::cout << "Player 2 solved in " << l.player(player2Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player1Id).state() & Player::Finished)
        std::cout << "Player 2 finished in " << l.player(player2Id).finishingDuration().count() << " ms" << std::endl;

    return result;//0;
}
