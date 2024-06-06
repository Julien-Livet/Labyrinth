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

#include "include/Labyrinth3d/Labyrinth.h"
#include "include/Labyrinth3d/Algorithm/Algorithm.h"
#include "include/Labyrinth3d/Solver/Solver.h"

using namespace Labyrinth3d;

int main()
{
    size_t const cycleOperations(0 * 1);
    std::chrono::milliseconds const cyclePause(0 * 1 * 1);
    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(1 * 50);//1 * 50);

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

    Labyrinth l(2, 2, 2);
    //Labyrinth l(5, 5, 5);
    //Labyrinth l(9, 9, 9);

    Algorithm::CellFusion cfa;
    l.generate(g, cfa, sleep, cycleOperations, cyclePause);
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
/*
    //l.player(player1d).solve(g, ass, sleep, 0, 0, cycleOperationsSolving, cyclePauseSolving);
    std::thread thSolvePlayer1(&Player::solve<std::default_random_engine, Solver::AStar>, &l.player(player1Id),
                               std::ref(g), std::ref(ass), sleep, 0, 0,
                               cycleOperationsSolving, cyclePauseSolving, nullptr);
*//*
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
*/
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

    sf::Shader colorShader{};
    colorShader.loadFromFile("shaders/couleur3D.vert", "shaders/couleur3D.frag");
    sf::Shader textureShader{};
    textureShader.loadFromFile("shaders/texture.vert", "shaders/texture.frag");

    std::vector<GlBlock> glBlocks;
    glBlocks.reserve(2 * 2 * 2);

    std::array<float, 6 * 3> const colors{1.0, 0.0, 0.0,
                                          0.5, 0.0, 0.0,
                                          0.0, 1.0, 0.0,
                                          0.0, 0.5, 0.0,
                                          0.0, 0.0, 1.0,
                                          0.0, 0.0, 0.5};

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

    sf::Image image{};
    image.loadFromFile("resources/wall_pattern.png");
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
                                                 1.0, 100.0),
                                glm::lookAt(glm::vec3{5.0, 5.0, 5.0}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{0.0, 0.0, 1.0})};
    //glFixedCamera.ortho(-5.0, 5.0, -5.0, 5.0, 1.0, 100.0);

    bool running{true};
    bool backspacePressed{false};
    sf::Event::KeyEvent keyEvent{};

    std::bitset<6> previousDirectionPressed{};
    std::bitset<6> directionPressed{};
    std::bitset<6> risingEdgeDirectionPressed{};
    std::array<std::bitset<3>, 2> modifierPressed{};

    Eigen::Matrix3f rotation{Eigen::Matrix3f::Identity()};

    while (running)
    {
        auto const startClock{std::chrono::steady_clock::now()};

        sf::Event event{};

        for (size_t i{0}; i < 6; ++i)
            previousDirectionPressed[i] = directionPressed[i];

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

                keyEvent = event.key;
            }
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);

                glFixedCamera.perspective(Utility::Double::radians(70.0),
                                          static_cast<double>(event.size.width) / static_cast<double>(event.size.height),
                                          0.1, 100.0);
            }
        }

        for (size_t i{0}; i < 6; ++i)
            risingEdgeDirectionPressed[i] = directionPressed[i] && !previousDirectionPressed[i];

        if (risingEdgeDirectionPressed[0])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
            {
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(90.0), Eigen::Vector3f::UnitX()}.toRotationMatrix();
            }
            else
            {
                l.player(player1Id).move(Direction::Front);
            }
        }
        if (risingEdgeDirectionPressed[1])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
            {
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(-90.0), Eigen::Vector3f::UnitX()}.toRotationMatrix();
            }
            else
            {
                l.player(player1Id).move(Direction::Back);
            }
        }
        if (risingEdgeDirectionPressed[2])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
            {
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(90.0), Eigen::Vector3f::UnitY()}.toRotationMatrix();
            }
            else
            {
                l.player(player1Id).move(Direction::Left);
            }
        }
        if (risingEdgeDirectionPressed[3])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
            {
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(-90.0), Eigen::Vector3f::UnitY()}.toRotationMatrix();
            }
            else
            {
                l.player(player1Id).move(Direction::Right);
            }
        }
        if (risingEdgeDirectionPressed[4])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
            {
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(90.0), Eigen::Vector3f::UnitZ()}.toRotationMatrix();
            }
            else
            {
                l.player(player1Id).move(Direction::Up);
            }
        }
        if (risingEdgeDirectionPressed[5])
        {
            if (modifierPressed[0][1] || modifierPressed[1][1])
            {
                rotation *= Eigen::AngleAxisf{Utility::Float::radians(-90.0), Eigen::Vector3f::UnitZ()}.toRotationMatrix();
            }
            else
            {
                l.player(player1Id).move(Direction::Down);
            }
        }
        if (backspacePressed)
            l.player(player1Id).stepBack();

        auto const i{l.player(l.playerIds().front()).i()};
        auto const j{l.player(l.playerIds().front()).j()};
        auto const k{l.player(l.playerIds().front()).k()};

        Eigen::Transform<float, 3, Eigen::Affine> modelView{Eigen::Transform<float, 3, Eigen::Affine>::Identity()};
        modelView.translation().x() = (j + 1) / 2 * wallsSize.x + j / 2 * waysSize.x + (j % 2 ? waysSize.x : wallsSize.x) / 2;
        modelView.translation().y() = (i + 1) / 2 * wallsSize.y + i / 2 * waysSize.y + (i % 2 ? waysSize.y : wallsSize.y) / 2;
        modelView.translation().y() = (k + 1) / 2 * wallsSize.z + k / 2 * waysSize.z + (k % 2 ? waysSize.z : wallsSize.z) / 2;
        modelView.linear() = rotation;

        //glFixedCamera.changeModelView(modelView);
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
                        mv.translate((Eigen::Vector3f{} << (j / 2) * waysSize.x + ((j + 1) / 2) * wallsSize.x + glBlock.sides()[0] / 2,
                                                           (i / 2) * waysSize.y + ((i + 1) / 2) * wallsSize.y + glBlock.sides()[1] / 2,
                                                           (k / 2) * waysSize.z + ((k + 1) / 2) * wallsSize.z + glBlock.sides()[2] / 2).finished());
                        //glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), colorShader);
                        glBlock.display(glFixedCamera.glmProjection(), Utility::eigenToGlm(mv), textureShader, textures);
                        /*Eigen::IOFormat const heavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
                        std::cout << "ijk " << i << " " << j << " " << k << std::endl;
                        std::cout << modelView.matrix().format(heavyFmt) << std::endl;*//*
                        i = l.grid().height();
                        j = l.grid().width();
                        k = l.grid().depth();*/
                    }
                }
            }
        }

        window.display();

        auto const finishClock{std::chrono::steady_clock::now()};
        auto const elapsedTime{std::chrono::duration_cast<std::chrono::microseconds>(finishClock - startClock).count()};
        auto constexpr frameRate{5};

        if (elapsedTime < frameRate)
            std::this_thread::sleep_for(std::chrono::milliseconds{frameRate - elapsedTime});
    }

    return 0;
}
