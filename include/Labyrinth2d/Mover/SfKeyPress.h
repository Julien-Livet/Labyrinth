#ifndef LABYRINTH2D_MOVER_SFKEYPRESS_H
#define LABYRINTH2D_MOVER_SFKEYPRESS_H

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
                std::array<sf::Keyboard::Key, 5> keys;

                SfKeyPress(Labyrinth& labyrinth_, size_t playerId_,
                           std::array<sf::Keyboard::Key, 5> const& keys_ = {sf::Keyboard::Right, sf::Keyboard::Up,
																		    sf::Keyboard::Left, sf::Keyboard::Down,
																		    sf::Keyboard::Backspace});
                bool filter(sf::Event const& event);
        };
    }
}

#endif // LABYRINTH2D_MOVER_SFKEYPRESS_H
