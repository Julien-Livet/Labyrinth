#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics/Shader.hpp>

#include "Labyrinth3d/Sfml/Utility.h"
#include "Labyrinth3d/Sfml/GlBlock.h"
#include "Labyrinth3d/Sfml/GlFixedCamera.h"

#include "include/Labyrinth2d/Algorithm/Algorithm.h"
#include "include/Labyrinth3d/Labyrinth.h"
#include "include/Labyrinth3d/Algorithm/Algorithm.h"
#include "include/Labyrinth3d/Solver/Solver.h"

using namespace Labyrinth3d;

void translateModelView(sf::Vector3f const& waysSize, sf::Vector3f const& wallsSize, Eigen::Vector3f const& offset,
                        Eigen::Transform<float, 3, Eigen::Affine>& mv, size_t i, size_t j, size_t k)
{
    mv.translate((Eigen::Vector3f{} << (j / 2) * waysSize.x + ((j + 1) / 2) * wallsSize.x + offset[0],
                                       (i / 2) * waysSize.y + ((i + 1) / 2) * wallsSize.y + offset[1],
                                       (k / 2) * waysSize.z + ((k + 1) / 2) * wallsSize.z + offset[2]).finished());
}

void displayPlayer(sf::Vector3f const& waysSize, sf::Vector3f const& wallsSize,
                   Labyrinth const& labyrinth, size_t playerId,
                   GlFixedCamera const& glFixedCamera,
                   sf::Shader const& colorShader,
                   std::array<float, 4> const& color,
                   bool displayPlayer,
                   bool displayTrace,
                   std::array<float, 4> const& traceColor,
                   GlBlock& pGlBlock,
                   std::vector<GlBlock>& traceGlBlocks)
{
    auto const& player{labyrinth.player(playerId)};

    float constexpr pRatio{0.25};
    pGlBlock.changeSides(pRatio * Eigen::Vector3f{waysSize.x, waysSize.y, waysSize.z});

    std::array<float, 6 * 4> colors;

    if (displayPlayer)
    {
        for (size_t i{0}; i < 6; ++i)
        {
            for (size_t j{0}; j < 4; ++j)
                colors[i * 4 + j] = color[j];
        }

        pGlBlock.changeFacetColors(colors);

        auto mv{glFixedCamera.modelView()};
        translateModelView(waysSize, wallsSize, pGlBlock.sides() / 2 / pRatio, mv, player.i(), player.j(), player.k());
        pGlBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);
    }

    if (displayTrace)
    {
        if (!player.traceIntersections().empty())
        {
            for (size_t i{0}; i < 6; ++i)
            {
                for (size_t j{0}; j < 4; ++j)
                    colors[i * 4 + j] = traceColor[j];
            }

            float const traceRatio{0.05};

            for (size_t k{0}; k < 2; ++k)
            {
                for (size_t i{0}; i < 2; ++i)
                {
                    for (size_t j{0}; j < 2; ++j)
                        traceGlBlocks[(k * 2 + i) * 2 + j].changeFacetColors(colors);
                }
            }

            for (size_t l{0}; l < player.traceIntersections().size() - 1; ++l)
            {
                size_t i1(std::get<0>(player.traceIntersections()[l]));
                size_t j1(std::get<1>(player.traceIntersections()[l]));
                size_t k1(std::get<2>(player.traceIntersections()[l]));
                size_t const i2(std::get<0>(player.traceIntersections()[l + 1]));
                size_t const j2(std::get<1>(player.traceIntersections()[l + 1]));
                size_t const k2(std::get<2>(player.traceIntersections()[l + 1]));
                long di(i2 - i1);
                long dj(j2 - j1);
                long dk(k2 - k1);

                if (di)
                    di /= std::abs(di);
                if (dj)
                    dj /= std::abs(dj);
                if (dk)
                    dk /= std::abs(dk);

                while (i1 != i2 || j1 != j2 || k1 != k2)
                {
                    auto const& glBlock{traceGlBlocks[((k1 % 2) * 2 + i1 % 2) * 2 + j1 % 2]};

                    auto mv{glFixedCamera.modelView()};
                    translateModelView(waysSize, wallsSize, glBlock.sides() / 2 / traceRatio, mv, i1, j1, k1);
                    glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);

                    i1 += di;
                    j1 += dj;
                    k1 += dk;
                }
            }

            {
                size_t i1(std::get<0>(player.traceIntersections().back()));
                size_t j1(std::get<1>(player.traceIntersections().back()));
                size_t k1(std::get<2>(player.traceIntersections().back()));
                size_t const i2(player.i());
                size_t const j2(player.j());
                size_t const k2(player.k());
                long di(i2 - i1);
                long dj(j2 - j1);
                long dk(k2 - k1);

                if (di)
                    di /= std::abs(di);
                if (dj)
                    dj /= std::abs(dj);
                if (dk)
                    dk /= std::abs(dk);

                while (i1 != i2 || j1 != j2 || k1 != k2)
                {
                    auto const& glBlock{traceGlBlocks[((k1 % 2) * 2 + i1 % 2) * 2 + j1 % 2]};

                    auto mv{glFixedCamera.modelView()};
                    translateModelView(waysSize, wallsSize, glBlock.sides() / 2 / traceRatio, mv, i1, j1, k1);
                    glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);

                    i1 += di;
                    j1 += dj;
                    k1 += dk;
                }
            }
        }
    }
}

int main()
{
    size_t const cycleOperations(0 * 1);
    std::chrono::milliseconds const cyclePause(0 * 1 * 1);
    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(10 * 50);

    //size_t const seed(1717503823494194900);
    size_t const seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine g(seed);
    std::cout << seed << std::endl;
    //std::random_device g;

    auto const sleep{
        [] (std::chrono::milliseconds const& ms) -> void
        {
#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
            std::this_thread::sleep_for(ms);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1
        }
    };

    //Labyrinth l(2, 2, 2);
    Labyrinth l(3, 3, 3);
    //Labyrinth l(5, 5, 5);
    //Labyrinth l(9, 9, 9);
    //Labyrinth l(15, 15, 15);

    Labyrinth2d::Algorithm::CellFusion cfa{};
    Labyrinth2d::Algorithm::RecursiveDivision rda{};
    Algorithm::RandomSlicer<Labyrinth2d::Algorithm::CellFusion, Labyrinth2d::Algorithm::RecursiveDivision> ssa{cfa, rda};
    l.generate(g, ssa, sleep, cycleOperations, cyclePause);
/*
    Algorithm::CellFusion cfa;
    l.generate(g, cfa, sleep, cycleOperations, cyclePause);
*/
    std::string const path{"C:/Users/juju0/AppData/Roaming/FreeCAD/Macro"};

    sf::Vector3f const wallsSize{5, 5, 5};
    sf::Vector3f const waysSize{20, 20, 20}; //diameter of 16 mm for a school ball

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
                            << "cube.Length = " << (j % 2 ? waysSize.x : wallsSize.x) << "\n"
                            << "cube.Width = " << (i % 2 ? waysSize.y : wallsSize.y) << "\n"
                            << "cube.Height = " << (k % 2 ? waysSize.z : wallsSize.z) << "\n"
                            << "cube.Placement = App.Placement(App.Vector(" << (j + 1) / 2 * wallsSize.x + j / 2 * waysSize.x << ", "
                                                                            << (i + 1) / 2 * wallsSize.y + i / 2 * waysSize.y << ", "
                                                                            << (k + 1) / 2 * wallsSize.z + k / 2 * waysSize.z
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
    Solver::Blind bs;

    //l.player(player2Id).solve(g, bs, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer2(&Player::solve<std::default_random_engine, Solver::AStar>, &l.player(player2Id),
                               std::ref(g), std::ref(ass), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);
    std::thread thSolvePlayer3(&Player::solve<std::default_random_engine, Solver::Blind>, &l.player(player3Id),
                               std::ref(g), std::ref(bs), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);

    //thSolvePlayer1.detach();
    thSolvePlayer2.detach();
    thSolvePlayer3.detach();

    sf::Window window{sf::VideoMode{800, 600}, "Labyrinth3DSf", sf::Style::Default,
                      sf::ContextSettings{24, 8, 0, 3, 2, sf::ContextSettings::Default, false}};
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    window.setActive(true);

    glewExperimental = GL_TRUE;
    auto const glewInitResult{glewInit()};

    if (glewInitResult != GLEW_OK)
    {
        std::cerr << "glewInit error: " << glewGetErrorString(glewInitResult) << std::endl;

        return 0;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sf::Shader colorShader{};
    colorShader.loadFromFile("shaders/couleur3D.vert", "shaders/couleur3D.frag");
    sf::Shader textureShader{};
    textureShader.loadFromFile("shaders/texture.vert", "shaders/texture.frag");

    std::vector<GlBlock> glBlocks;
    glBlocks.reserve(2 * 2 * 2);

    std::array<float, 6 * 4> const colors{1.0, 0.0, 0.0, 1.0,
                                          0.5, 0.0, 0.0, 1.0,
                                          0.0, 1.0, 0.0, 1.0,
                                          0.0, 0.5, 0.0, 1.0,
                                          0.0, 0.0, 1.0, 1.0,
                                          0.0, 0.0, 0.5, 1.0};

    for (size_t k{0}; k < 2; ++k)
    {
        for (size_t i{0}; i < 2; ++i)
        {
            for (size_t j{0}; j < 2; ++j)
            {
                Eigen::Vector3f v;
                v[0] = j % 2 ? waysSize.x : wallsSize.x;
                v[1] = i % 2 ? waysSize.y : wallsSize.y;
                v[2] = k % 2 ? waysSize.z : wallsSize.z;
                glBlocks.emplace_back(v);
                glBlocks.back().changeFacetColors(colors);
            }
        }
    }

    float const sfRatio{0.95};
    GlBlock sfGlBlock{sfRatio * Eigen::Vector3f{waysSize.x, waysSize.y, waysSize.z}}; //start and finish

    sf::Image image{};
    image.loadFromFile("resources/wall_pattern_3.png");
    image.flipVertically();
    sf::Texture texture{};
    texture.loadFromImage(image);

    std::array<std::reference_wrapper<const sf::Texture>, 6> const textures{std::cref(texture),
                                                                            std::cref(texture),
                                                                            std::cref(texture),
                                                                            std::cref(texture),
                                                                            std::cref(texture),
                                                                            std::cref(texture)};

    glViewport(0, 0, window.getSize().x, window.getSize().y);

    GlFixedCamera glFixedCamera{glm::perspective(Utility::Double::radians(70.0),
                                                 static_cast<double>(window.getSize().x) / static_cast<double>(window.getSize().y),
                                                 1.0, 1000.0),
                                glm::lookAt(glm::vec3{0.0, 0.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec3{0.0, 0.0, 1.0})};

    bool running{true};
    bool backspacePressed{false};
    bool enterPressed{false};
    bool rPressed{false};
    sf::Event::KeyEvent keyEvent{};
    bool constexpr displayTrace{true};

    std::bitset<6> directionPressed{};
    std::array<std::bitset<3>, 2> modifierPressed{};

    Eigen::Matrix3f rotation{Eigen::Matrix3f::Identity()};
    GlBlock pGlBlock{Eigen::Vector3f{1.0, 1.0, 1.0}}; //player
    std::vector<GlBlock> traceGlBlocks;
    traceGlBlocks.reserve(2 * 2 * 2);

    float constexpr traceRatio{0.05};

    for (size_t k{0}; k < 2; ++k)
    {
        for (size_t i{0}; i < 2; ++i)
        {
            for (size_t j{0}; j < 2; ++j)
            {
                if (traceGlBlocks.size() != 8)
                {
                    Eigen::Vector3f v;
                    v[0] = traceRatio * (j % 2 ? waysSize.x : wallsSize.x);
                    v[1] = traceRatio * (i % 2 ? waysSize.y : wallsSize.y);
                    v[2] = traceRatio * (k % 2 ? waysSize.z : wallsSize.z);
                    traceGlBlocks.emplace_back(v);
                }
            }
        }
    }

    auto const firstPlayerId{player1Id};

    struct PlayerDisplay
    {
        std::array<float, 4> color{1.0, 0.0, 0.0, 1.0};
        bool displayTrace{true};
        std::array<float, 4> traceColor{1.0, 0.0, 0.0, 0.5};
    };

    std::map<size_t, PlayerDisplay> playerDisplays;

    playerDisplays[player1Id] = PlayerDisplay{std::array<float, 4>{1.0, 0.0, 0.0, 1.0},
                                              displayTrace,
                                              std::array<float, 4>{1.0, 0.0, 0.0, 0.5}};
    playerDisplays[player2Id] = PlayerDisplay{std::array<float, 4>{0.0, 1.0, 0.0, 1.0},
                                              displayTrace,
                                              std::array<float, 4>{0.0, 1.0, 0.0, 0.5}};
    playerDisplays[player3Id] = PlayerDisplay{std::array<float, 4>{0.0, 0.0, 1.0, 1.0},
                                              displayTrace,
                                              std::array<float, 4>{0.0, 0.0, 1.0, 0.5}};

    while (running)
    {
        auto const startClock{std::chrono::steady_clock::now()};

        sf::Event event{};

        std::bitset<6> previousDirectionPressed{};

        for (size_t i{0}; i < 6; ++i)
            previousDirectionPressed[i] = directionPressed[i];

        bool previousEnterPressed{enterPressed};
        bool previousBackspacePressed{backspacePressed};
        bool previousRPressed{rPressed};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                running = false;
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    running = false;
                else if (event.key.code == sf::Keyboard::Up)
                    directionPressed[0] = true;
                else if (event.key.code == sf::Keyboard::Down)
                    directionPressed[1] = true;
                else if (event.key.code == sf::Keyboard::Left)
                    directionPressed[2] = true;
                else if (event.key.code == sf::Keyboard::Right)
                    directionPressed[3] = true;
                else if (event.key.code == sf::Keyboard::PageUp)
                    directionPressed[4] = true;
                else if (event.key.code == sf::Keyboard::PageDown)
                    directionPressed[5] = true;
                else if (event.key.code == sf::Keyboard::LControl)
                    modifierPressed[0][0] = true;
                else if (event.key.code == sf::Keyboard::LShift)
                    modifierPressed[0][1] = true;
                else if (event.key.code == sf::Keyboard::LAlt)
                    modifierPressed[0][2] = true;
                else if (event.key.code == sf::Keyboard::RControl)
                    modifierPressed[1][0] = true;
                else if (event.key.code == sf::Keyboard::RShift)
                    modifierPressed[1][1] = true;
                else if (event.key.code == sf::Keyboard::RAlt)
                    modifierPressed[1][2] = true;
                else if (event.key.code == sf::Keyboard::Backspace)
                    backspacePressed = true;
                else if (event.key.code == sf::Keyboard::Enter)
                    enterPressed = true;
                else if (event.key.code == sf::Keyboard::R)
                    rPressed = true;
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    running = false;
                else if (event.key.code == sf::Keyboard::Up)
                    directionPressed[0] = false;
                else if (event.key.code == sf::Keyboard::Down)
                    directionPressed[1] = false;
                else if (event.key.code == sf::Keyboard::Left)
                    directionPressed[2] = false;
                else if (event.key.code == sf::Keyboard::Right)
                    directionPressed[3] = false;
                else if (event.key.code == sf::Keyboard::PageUp)
                    directionPressed[4] = false;
                else if (event.key.code == sf::Keyboard::PageDown)
                    directionPressed[5] = false;
                else if (event.key.code == sf::Keyboard::LControl)
                    modifierPressed[0][0] = false;
                else if (event.key.code == sf::Keyboard::LShift)
                    modifierPressed[0][1] = false;
                else if (event.key.code == sf::Keyboard::LAlt)
                    modifierPressed[0][2] = false;
                else if (event.key.code == sf::Keyboard::RControl)
                    modifierPressed[1][0] = false;
                else if (event.key.code == sf::Keyboard::RShift)
                    modifierPressed[1][1] = false;
                else if (event.key.code == sf::Keyboard::RAlt)
                    modifierPressed[1][2] = false;
                else if (event.key.code == sf::Keyboard::Backspace)
                    backspacePressed = false;
                else if (event.key.code == sf::Keyboard::Enter)
                    enterPressed = false;
                else if (event.key.code == sf::Keyboard::R)
                    rPressed = false;

                keyEvent = event.key;
            }
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);

                glFixedCamera.perspective(Utility::Double::radians(70.0),
                                          static_cast<double>(event.size.width) / static_cast<double>(event.size.height),
                                          1.0, 1000.0);
            }
        }

        std::bitset<6> risingEdgeDirectionPressed{};

        for (size_t i{0}; i < 6; ++i)
            risingEdgeDirectionPressed[i] = directionPressed[i] && !previousDirectionPressed[i];

        bool const risingEdgeEnterPressed{enterPressed && !previousEnterPressed};
        bool const risingEdgeBackspacePressed{backspacePressed && !previousBackspacePressed};
        bool const risingEdgeRPressed{rPressed && !previousRPressed};

        auto move{
            [&l, player1Id] (Eigen::Vector3f const& u, Labyrinth3d::Direction d)
            {
                switch (d)
                {
                    case Labyrinth3d::Front:
                    case Labyrinth3d::Left:
                    case Labyrinth3d::Up:
                        if ((u - Eigen::Vector3f::UnitX()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Front);
                        else if ((u + Eigen::Vector3f::UnitX()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Back);
                        else if ((u - Eigen::Vector3f::UnitY()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Left);
                        else if ((u + Eigen::Vector3f::UnitY()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Right);
                        else if ((u - Eigen::Vector3f::UnitZ()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Up);
                        else if ((u + Eigen::Vector3f::UnitZ()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Down);
                        break;

                    case Labyrinth3d::Back:
                    case Labyrinth3d::Right:
                    case Labyrinth3d::Down:
                        if ((u - Eigen::Vector3f::UnitX()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Back);
                        else if ((u + Eigen::Vector3f::UnitX()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Front);
                        else if ((u - Eigen::Vector3f::UnitY()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Right);
                        else if ((u + Eigen::Vector3f::UnitY()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Left);
                        else if ((u - Eigen::Vector3f::UnitZ()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Down);
                        else if ((u + Eigen::Vector3f::UnitZ()).isZero())
                            l.player(player1Id).move(Labyrinth3d::Up);
                        break;
                }
            }
        };

        float const stepAngle{90.0 / 2.0};

        if (risingEdgeDirectionPressed[0])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(-stepAngle),
                                              Eigen::Vector3f::UnitY()}.toRotationMatrix();
            else
                move(rotation.col(0), Labyrinth3d::Front);
        }
        if (risingEdgeDirectionPressed[1])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(stepAngle),
                                              Eigen::Vector3f::UnitY()}.toRotationMatrix();
            else
                move(rotation.col(0), Labyrinth3d::Back);
        }
        if (risingEdgeDirectionPressed[2])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(stepAngle),
                                              Eigen::Vector3f::UnitZ()}.toRotationMatrix();
            else
                move(rotation.col(1), Labyrinth3d::Left);
        }
        if (risingEdgeDirectionPressed[3])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(-stepAngle),
                                              Eigen::Vector3f::UnitZ()}.toRotationMatrix();
            else
                move(rotation.col(1), Labyrinth3d::Right);
        }
        if (risingEdgeDirectionPressed[4])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(stepAngle),
                                              Eigen::Vector3f::UnitX()}.toRotationMatrix();
            else
                move(rotation.col(2), Labyrinth3d::Up);
        }
        if (risingEdgeDirectionPressed[5])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(-45.0),
                                              Eigen::Vector3f::UnitX()}.toRotationMatrix();
            else
                move(rotation.col(2), Labyrinth3d::Down);
        }
        if (risingEdgeBackspacePressed)
            l.player(player1Id).stepBack();
        if (risingEdgeEnterPressed)
            rotation = Eigen::Matrix3f::Identity();
        if (risingEdgeRPressed)
        {
            std::thread thSolvePlayer1(&Labyrinth3d::Player::solve<std::default_random_engine,
                                                                   Labyrinth3d::Solver::AStar>,
                                       &l.player(player1Id),
                                       std::ref(g), std::ref(ass), sleep, 0, 0,
                                       cycleOperationsSolving, cyclePauseSolving, nullptr);

            thSolvePlayer1.detach();
        }

        try
        {
            auto const i{l.player(firstPlayerId).i()};
            auto const j{l.player(firstPlayerId).j()};
            auto const k{l.player(firstPlayerId).k()};

            Eigen::Transform<float, 3, Eigen::Affine> modelView{Eigen::Transform<float, 3, Eigen::Affine>::Identity()};
            modelView.translation().x() = (j + 1) / 2 * wallsSize.x + j / 2 * waysSize.x + (j % 2 ? waysSize.x : wallsSize.x) / 2;
            modelView.translation().y() = (i + 1) / 2 * wallsSize.y + i / 2 * waysSize.y + (i % 2 ? waysSize.y : wallsSize.y) / 2;
            modelView.translation().z() = (k + 1) / 2 * wallsSize.z + k / 2 * waysSize.z + (k % 2 ? waysSize.z : wallsSize.z) / 2;
            modelView.linear() = rotation;

            glFixedCamera.changeModelView(glm::lookAt(glm::vec3{modelView.translation().x(), modelView.translation().y(), modelView.translation().z()},
                                                      glm::vec3{modelView.translation().x() + modelView.linear().col(0)[0],
                                                                modelView.translation().y() + modelView.linear().col(0)[1],
                                                                modelView.translation().z() + modelView.linear().col(0)[2]},
                                                      glm::vec3{modelView.linear().col(2)[0], modelView.linear().col(2)[1], modelView.linear().col(2)[2]}));

            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (size_t k{0}; k < l.grid().depth(); ++k)
            {
                for (size_t i{0}; i < l.grid().height(); ++i)
                {
                    for (size_t j{0}; j < l.grid().width(); ++j)
                    {
                        if (l.grid().at(i, j, k))
                        {
                            auto const& glBlock{glBlocks[((k % 2) * 2 + i % 2) * 2 + j % 2]};

                            auto mv{glFixedCamera.modelView()};
                            translateModelView(waysSize, wallsSize, glBlock.sides() / 2, mv, i, j, k);
                            //glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);
                            glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), textureShader, textures);
                        }
                    }
                }
            }

            for (size_t m{0}; m < l.playerIds().size(); ++m)
            {
                if (m != firstPlayerId)
                    continue;

                auto const playerId{l.playerIds()[m]};
                auto& glBlock{sfGlBlock};
                auto const& player{l.player(playerId)};
                glBlock.changeFacetColors(std::array<float, 6 * 4>{0.0, 1.0, 0.0, 0.25,
                                                                   0.0, 1.0, 0.0, 0.25,
                                                                   0.0, 1.0, 0.0, 0.25,
                                                                   0.0, 1.0, 0.0, 0.25,
                                                                   0.0, 1.0, 0.0, 0.25,
                                                                   0.0, 1.0, 0.0, 0.25});
                auto mv{glFixedCamera.modelView()};
                translateModelView(waysSize, wallsSize, glBlock.sides() / 2 / sfRatio, mv, player.startI(), player.startJ(), player.startK());
                glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);
                for (size_t n{0}; n < player.finishI().size(); ++n)
                {
                    mv = glFixedCamera.modelView();
                    glBlock.changeFacetColors(std::array<float, 6 * 4>{1.0, 0.0, 0.0, 0.25,
                                                                       1.0, 0.0, 0.0, 0.25,
                                                                       1.0, 0.0, 0.0, 0.25,
                                                                       1.0, 0.0, 0.0, 0.25,
                                                                       1.0, 0.0, 0.0, 0.25,
                                                                       1.0, 0.0, 0.0, 0.25});
                    translateModelView(waysSize, wallsSize, glBlock.sides() / 2 / sfRatio, mv, player.finishI()[n], player.finishJ()[n], player.finishK()[n]);
                    glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);
                }
            }

            for (auto it{playerDisplays.begin()}; it != playerDisplays.end(); ++it)
                displayPlayer(waysSize, wallsSize, l, it->first, glFixedCamera, colorShader, it->second.color,
                              it->first == firstPlayerId ? false : (l.player(it->first).i() != l.player(firstPlayerId).i()
                                                                    || l.player(it->first).j() != l.player(firstPlayerId).j()
                                                                    || l.player(it->first).k() != l.player(firstPlayerId).k()),
                              it->second.displayTrace, it->second.traceColor, pGlBlock, traceGlBlocks);
        }
        catch (GlException const& e)
        {
            switch (e.glError())
            {
                case GL_NO_ERROR:
                    std::cout << "GL_NO_ERROR" << std::endl;
                    break;
                case GL_INVALID_ENUM:
                    std::cout << "GL_INVALID_ENUM" << std::endl;
                    break;
                case GL_INVALID_VALUE:
                    std::cout << "GL_INVALID_VALUE" << std::endl;
                    break;
                case GL_INVALID_OPERATION:
                    std::cout << "GL_INVALID_OPERATION" << std::endl;
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                    break;
                case GL_OUT_OF_MEMORY:
                    std::cout << "GL_OUT_OF_MEMORY" << std::endl;
                    break;
                case GL_STACK_UNDERFLOW:
                    std::cout << "GL_STACK_UNDERFLOW" << std::endl;
                    break;
                case GL_STACK_OVERFLOW:
                    std::cout << "GL_STACK_OVERFLOW" << std::endl;
                    break;
                case GL_CONTEXT_LOST:
                    std::cout << "GL_CONTEXT_LOST" << std::endl;
                    break;
                case GL_TABLE_TOO_LARGE:
                    std::cout << "GL_TABLE_TOO_LARGE" << std::endl;
                    break;
                default:
                    std::cout << e.glError() << std::endl;
                    break;
            }
        }

        window.display();

        auto const finishClock{std::chrono::steady_clock::now()};
        auto const elapsedTime{std::chrono::duration_cast<std::chrono::microseconds>(finishClock - startClock).count()};
        auto constexpr frameRate{5};

        if (elapsedTime < frameRate)
            std::this_thread::sleep_for(std::chrono::milliseconds{frameRate - elapsedTime});
    }
/*
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

    return 0;
}
