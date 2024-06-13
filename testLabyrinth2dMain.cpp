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

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Algorithm/Algorithm.h"
#include "Labyrinth2d/Renderer/Renderer.h"
#include "Labyrinth2d/Solver/Solver.h"
#include "Labyrinth2d/Mover/QKeyPress.h"

using namespace Labyrinth2d;

void stringRenderGeneratingLabyrinth(Renderer::String& stringRenderer, std::chrono::milliseconds const& cyclePause)
{
    if (!cyclePause.count())
        return;

     while (!(stringRenderer.labyrinth.state() & Labyrinth::Generated))
     {
         std::cout << stringRenderer() << std::endl;

         std::this_thread::sleep_for(cyclePause / 2);
    }
}

void stringRenderSolvingLabyrinth(Renderer::String& stringRenderer, std::chrono::milliseconds const& cyclePause)
{
    if (!cyclePause.count())
        return;

    size_t n(0);

    for (auto id : stringRenderer.labyrinth.playerIds())
        if (stringRenderer.labyrinth.player(id).state() & Player::Finished)
            ++n;

     while (n != stringRenderer.labyrinth.playerIds().size())
     {
         std::cout << stringRenderer() << std::endl;

         std::this_thread::sleep_for(cyclePause / 2);

         n = 0;

         for (auto id : stringRenderer.labyrinth.playerIds())
             if (stringRenderer.labyrinth.player(id).state() & Player::Finished)
                 ++n;
    }
}

class WidgetRenderer : public QWidget
{
    public:
        Renderer::QPainter qPainterRenderer;

        WidgetRenderer(int refreshCycle, Labyrinth const& labyrinth,
                       QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f), qPainterRenderer(labyrinth),
                                                                                     _solvingTimerId(-1),
                                                                                     _margins(10, 10, 10, 10)
        {
            _solvingTimerId = startTimer(refreshCycle);
            setFixedSize(1, 1);
        }

        WidgetRenderer(int refreshCycle, Renderer::QPainter const& _qPainterRenderer,
                       QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f), qPainterRenderer(_qPainterRenderer),
                                                                                     _solvingTimerId(-1),
                                                                                     _margins(10, 10, 10, 10)
        {
            _solvingTimerId = startTimer(refreshCycle);
            setFixedSize(1, 1);
        }

    protected:
        virtual void paintEvent(QPaintEvent* event)
        {
            assert(event != 0);

            QWidget::paintEvent(event);

            QPainter painter(this);

            QPoint const offset(QRect(QPoint(), size()).center()
                                - QRect(QPoint(), qPainterRenderer.size()).center());

            qPainterRenderer(&painter,
                             event->region().translated(-offset).intersected(QRect(QPoint(), qPainterRenderer.size())),
                             offset);

            if (_previousSize != qPainterRenderer.size())
            {
                _previousSize = qPainterRenderer.size();

                setFixedSize((QRect(QPoint(), _previousSize) + _margins).size());
            }
        }

        virtual void timerEvent(QTimerEvent* event)
        {
            assert(event != 0);

            QWidget::timerEvent(event);

            if (event->timerId() == _solvingTimerId)
            {
                update();

                size_t n(0);

                for (auto id : qPainterRenderer.labyrinth.playerIds())
                    if (qPainterRenderer.labyrinth.player(id).state() & Player::Finished)
                        ++n;

                if (n == qPainterRenderer.labyrinth.playerIds().size())
                {
                    killTimer(_solvingTimerId);
                    _solvingTimerId = -1;
                }
            }
        }

    private:
        int _solvingTimerId;
        QSize _previousSize;
        QMargins _margins;
};

int main(int argc, char** argv)
{
    size_t const cycleOperations(0 * 1);
    std::chrono::milliseconds const cyclePause(0 * 1 * 1);
    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(1 * 50);//1 * 50);

    size_t const seed(2761453349);//3339863251);
    //size_t const seed(std::chrono::system_clock::now().time_since_epoch().count());
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

    //Labyrinth l(2, 2);
    //Labyrinth l(3, 9);
    Labyrinth l(9, 9);
    //Labyrinth l(10, 50);
    //Labyrinth l(40, 100);
    //Labyrinth l(800, 600);
    //Labyrinth l(1440, 900);
    //Labyrinth l(20, 75);
    //Labyrinth l(5000, 5000);

    Renderer::String sr(l, ' ', '#');
    //Renderer::String sr(l, ' ');
    //Renderer::String sr(l);

    //sr.swapWallsAndWaysDirection();
/*
    std::thread th1(stringRenderGeneratingLabyrinth, std::ref(sr), cyclePause);

    th1.detach();
*//*
    Algorithm::RandomDegenerative rda(10.0);//25.0);

    //Algorithm::Kruskal ka;
    Algorithm::Kruskal ka(Algorithm::Kruskal::Iterative);

    //l.generate(g, ka, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine,
                                                Algorithm::Kruskal>,
                           &l, std::ref(g), std::ref(ka), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*/
    //l.generate(g, g, ka, rda, sleep, cycleOperations, cyclePause);
/*
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, std::default_random_engine,
                                                Algorithm::Kruskal, Algorithm::RandomDegenerative>,
                           &l, std::ref(g), std::ref(g), std::ref(ka), std::ref(rda), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*/
    Algorithm::CellFusion cfa;

    //l.generate(g, cfa, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, Algorithm::CellFusion>,
                           &l, std::ref(g), std::ref(cfa), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
/*
    Algorithm::RecursiveDivision rda;

    //l.generate(g, rda, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, Algorithm::RecursiveDivision>,
                           &l, std::ref(g), std::ref(rda), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*//*
    Algorithm::WaySearch wsa(Algorithm::WaySearch::DepthFirstSearch);
    //Algorithm::WaySearch wsa(Algorithm:WaySearch::Prim);
    //Algorithm::WaySearch wsa(Algorithm::WaySearch::HuntAndKill);

    //l.generate(g, wsa, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, Algorithm::WaySearch>,
                           &l, std::ref(g), std::ref(wsa), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*//*
    Algorithm::Fractal fa;

    //l.generate(g, fa, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, Algorithm::Fractal>,
                           &l, std::ref(g), std::ref(fa), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*//*
    Algorithm::Kruskal ka;
    Algorithm::Fractal fa;
    Algorithm::Random<Algorithm::Kruskal, Algorithm::Fractal> ra(ka, fa);

    Algorithm::Recursive<Algorithm::Random<Algorithm::Kruskal, Algorithm::Fractal> > r(ra, 50, 2, 2);

    //l.generate(g, r, sleep, cycleOperations, cyclePause);
    std::thread thGenerate(&Labyrinth::generate<std::default_random_engine, Algorithm::Recursive<Algorithm::Random<Algorithm::Kruskal, Algorithm::Fractal> > >,
                           &l, std::ref(g), std::ref(r), sleep, cycleOperations, cyclePause, nullptr);
    thGenerate.detach();
*/
    //th1.join();
/*
    std::cout << sr() << std::endl;

    {
        std::ofstream ofs("labyrinth.txt");

        ofs << sr() << std::endl;
    }

    std::cout << l.generationDuration().count() << " ms" << std::endl;
*/

    QApplication application(argc, argv);

    QScrollArea scrollArea;/*
    scrollArea.horizontalScrollBar()->setTracking(false);
    scrollArea.verticalScrollBar()->setTracking(false);*/
    WidgetRenderer* wr = new WidgetRenderer(static_cast<int>(cyclePauseSolving.count()), l, &scrollArea);
    //WidgetRenderer* wr = new WidgetRenderer(10, l, &scrollArea);
    scrollArea.setWidget(wr);
    scrollArea.setFocusProxy(wr);
    scrollArea.resize(800, 600);
    scrollArea.setAlignment(Qt::AlignCenter);
    scrollArea.show();
    scrollArea.move(QRect(QPoint(), QGuiApplication::screenAt(scrollArea.pos())->size()).center() - QRect(QPoint(), scrollArea.size()).center());
    scrollArea.setFocusPolicy(Qt::WheelFocus);

    Renderer::QPainter& qpr(wr->qPainterRenderer);
    //qpr.rendererData.displayWalls = false;
    //qpr.rendererData.displayWays = false;
    qpr.changeWallsSize(QSize(1, 1));
    qpr.changeWaysSize(QSize(10, 10));
    qpr.changeWallsTexture(Renderer::QPainter::Texture(Qt::black, 50.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //qpr.changeWaysTexture(Renderer::QPainter::Texture(Qt::blue, 50.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //qpr.changeWaysTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/way_pattern.png"), Renderer::QPainter::Texture::Pattern, Qt::IgnoreAspectRatio));
    //qpr.changeWallsTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/wall_pattern.png"), Renderer::QPainter::Texture::Pattern, Qt::IgnoreAspectRatio));
    //qpr.changeWaysTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/way_image.png"), Renderer::QPainter::Texture::Background, Qt::KeepAspectRatioByExpanding));
    qpr.changeWallsTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/background_image.png"), Renderer::QPainter::Texture::Background, Qt::KeepAspectRatioByExpanding));
    //qpr.changeWaysTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/way_image.png"), Renderer::QPainter::Texture::Mosaic));
    //qpr.changeWallsTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/wall_image.png"), Renderer::QPainter::Texture::Mosaic));
    //qpr.changeBackgroundTexture(Renderer::QPainter::Texture(QPixmap(":/Images/resources/background_image.png"), Renderer::QPainter::Texture::Mosaic));
    //qpr.changeBbackgroundTexture(Renderer::QPainter::Texture(Qt::green));

    std::string const path{"C:/Users/juju0/AppData/Roaming/FreeCAD/Macro"};

    QSize const wallsSize{5, 5};
    QSize const waysSize{20, 20}; //diameter of 16 mm for a school ball

    std::ofstream ofs{path + "/labyrinth2d.FCMacro"};

    if (ofs)
    {
        for (size_t i(0); i < l.grid().height(); ++i)
        {
            for (size_t j(0); j < l.grid().width(); ++j)
            {
                if (l.grid().at(i, j))
                {
                    ofs << "cube = App.ActiveDocument.addObject(\"Part::Box\", \"Cube\")\n"
                        << "cube.Length = " << (j % 2 ? waysSize.width() : wallsSize.width()) << "\n"
                        << "cube.Width = " << (i % 2 ? waysSize.height() : wallsSize.height()) << "\n"
                        << "cube.Height = " << 2 * std::max(wallsSize.width(), wallsSize.height()) << "\n"
                        << "cube.Placement = App.Placement(App.Vector(" << (j + 1) / 2 * wallsSize.width() + j / 2 * waysSize.width() << ", "
                                                                        << (i + 1) / 2 * wallsSize.height() + i / 2 * waysSize.height()
                                                                        << ", 0), App.Rotation(App.Vector(0, 0, 1), 0))\n";
                }
            }
        }

        ofs.close();
    }

    size_t const player1Id(l.addPlayer(0, 0, {l.grid().rows() - 1}, {l.grid().columns() - 1}, true));
    size_t const player2Id(l.addPlayer(l.grid().rows() - 1, 0, {0}, {l.grid().columns() - 1}, true));
    size_t const player3Id(l.addPlayer(l.grid().rows() - 1, l.grid().columns() - 1, {0}, {0}, true));
    size_t const player4Id(l.addPlayer(0, l.grid().columns() - 1, {l.grid().rows() - 1}, {0}, true));

    sr.playerRenderers[player1Id] = Labyrinth2d::Renderer::String::PlayerRenderer('X', '.');
    sr.playerRenderers[player1Id].displayTrace = true;
/*
    qpr.playerRenderers[player1Id].playerTexture.change(QPixmap("nyancat.png"), Renderer::QPainter::Texture::Pattern, Qt::KeepAspectRatio);
    QLinearGradient linearGradient(QPointF(,  qpr.rect().bottomRight());
    linearGradient.setColorAt(0.0 / 5.0, Qt::red);
    linearGradient.setColorAt(1.0 / 5.0, QColor(255, 153, 0));
    linearGradient.setColorAt(2.0 / 5.0, Qt::yellow);
    linearGradient.setColorAt(3.0 / 5.0, QColor(51, 255, 0));
    linearGradient.setColorAt(4.0 / 5.0, QColor(0, 153, 255));
    linearGradient.setColorAt(5.0 / 5.0, QColor(102, 51, 255));
    qpr.playerRenderers[player1Id].traceTexture.change(linearGradient, 25.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);*/
    //qpr.playerRenderers[player1Id].playerTexture.change(QColor(255, 0, 0, 25), 75.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    //qpr.playerRenderers[player1Id].traceTexture.change(QColor(255, 0, 0, 127), 25.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    //qpr.playerRenderers[player1Id].displayPlayer = true;
    qpr.playerRenderers[player2Id].playerTexture.change(QColor(0, 255, 0, 255), 75.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    qpr.playerRenderers[player2Id].traceTexture.change(QColor(0, 255, 0, 127), 25.0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    //qpr.playerRenderers[player2Id].displayPlayer = false;
    qpr.playerRenderers[player3Id].playerTexture.change(QColor(0, 0, 255, 255), 75.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    qpr.playerRenderers[player3Id].traceTexture.change(QColor(0, 0, 255, 127), 25.0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    //qpr.playerRenderers[player3Id].displayPlayer = false;
    qpr.playerRenderers[player4Id].playerTexture.change(QColor(255, 255, 0, 255), 75.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    qpr.playerRenderers[player4Id].traceTexture.change(QColor(255, 255, 0, 127), 25.0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    //qpr.playerRenderers[player4Id].displayPlayer = false;

    //std::thread th2(stringRenderSolvingLabyrinth, std::ref(sr), cyclePauseSolving);

    //thGenerate.join(); //generete system_error exception but what...

    Solver::AStar ass;
    Solver::Blind bs;
    Solver::WallHand whsl(Solver::WallHand::Left);
    //Solver::WallHand whsr(Solver::WallHand::Right);

    //l.player(player1Id).solve(g, ass, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);

    /*std::thread thSolvePlayer1(&Player::solve<std::default_random_engine, Solver::WallHand>, &l.player(player1Id),
                                 std::ref(g), std::ref(whsr), sleep, 0, 0,
                                 cycleOperationsSolving, cyclePauseSolving, nullptr);*//*
    std::thread thSolvePlayer2(&Player::solve<std::default_random_engine, Solver::AStar>, &l.player(player2Id),
                               std::ref(g), std::ref(ass), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);
    std::thread thSolvePlayer3(&Player::solve<std::default_random_engine, Solver::Blind>, &l.player(player3Id),
                               std::ref(g), std::ref(bs), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);
    std::thread thSolvePlayer4(&Player::solve<std::default_random_engine, Solver::WallHand>, &l.player(player4Id),
                               std::ref(g), std::ref(whsl), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);

    //thSolvePlayer1.detach();
    thSolvePlayer2.detach();
    thSolvePlayer3.detach();
    thSolvePlayer4.detach();
*/
    Mover::QKeyPress kpm(l, player1Id);

    wr->installEventFilter(&kpm);
/*
    //Solver::WallHand whs(Solver::WallHand::Right);
    Solver::WallHandwhs(Solver::WallHand::Left);

    l.player(id).solve(g, whs, 0, 0, sleep, cycleOperationsSolving, cyclePauseSolving);
*//*
    Solver::Blind bs;

    l.player(id).solve(g, bs, 0, 0, sleep, cycleOperationsSolving, cyclePauseSolving);
*/
    //th2.join();
/*
    th3.join();

    std::cout << sr() << std::endl;

    {
        std::ofstream ofs("labyrinthSolved.txt");

        ofs << sr() << std::endl;
    }
*/
    auto const result{application.exec()};
/*
    thSolvePlayer3d.join();

    //thSolvePlayer1.join();
    thSolvePlayer2.join();
    thSolvePlayer3.join();
    thSolvePlayer4.join();
*/
    std::cout << "Player 1 state: " << l.player(player1Id).state() << std::endl;
    std::cout << "Player 2 state: " << l.player(player2Id).state() << std::endl;
    std::cout << "Player 3 state: " << l.player(player3Id).state() << std::endl;
    std::cout << "Player 4 state: " << l.player(player4Id).state() << std::endl;

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

    if (l.player(player3Id).state() & Player::Solved)
        std::cout << "Player 3 solved in " << l.player(player3Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player3Id).state() & Player::Finished)
        std::cout << "Player 3 finished in " << l.player(player3Id).finishingDuration().count() << " ms" << std::endl;

    if (l.player(player4Id).state() & Player::Solved)
        std::cout << "Player 4 solved in " << l.player(player4Id).solvingDuration().count() << " ms" << std::endl;
    else if (l.player(player4Id).state() & Player::Finished)
        std::cout << "Player 4 finished in " << l.player(player4Id).finishingDuration().count() << " ms" << std::endl;

    return result;//0;
}
