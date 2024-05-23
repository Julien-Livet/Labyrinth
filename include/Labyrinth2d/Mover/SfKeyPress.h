#ifndef LABYRINTH2D_MOVER_QKEYPRESS_H
#define LABYRINTH2D_MOVER_QKEYPRESS_H

#include <array>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace Labyrinth2d
{
    class Labyrinth;

    namespace Mover
    {
        class SfKeyPress
        {
            public:
                enum Direction
                {
                    Right = 0,
                    Up = 1,
                    Left = 2,
                    Down = 3,
                    StepBack = 4
                };

                Labyrinth& labyrinth;
                size_t playerId;
                std::array<Sf::Keyboard::Key, 5> keys;

                QKeyPress(Labyrinth& labyrinth_, size_t playerId_,
                          std::array<Sf::Keyboard::Key, 5> const& keys_ = {Sf::Keyboard::Right, Sf::Keyboard::Up,
																		   Sf::Keyboard::Left, Sf::Keyboard::Down,
																		   Sf::Keyboard::Backspace});
                bool filter(sf::Event const& event);
        };
    }
}

#endif // LABYRINTH2D_MOVER_QKEYPRESS_H
