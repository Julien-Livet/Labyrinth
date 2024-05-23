#include <cassert>

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Mover/SfKeyPress.h"

Labyrinth2d::Mover::SfKeyPress::SfKeyPress(Labyrinth& labyrinth_, size_t playerId_,
                                           std::array<sf::Keyboard::Key, 5> const& keys_) : labyrinth(labyrinth_),
																							playerId(playerId_),
																							keys(keys_)
{
}

bool Labyrinth2d::Mover::SfKeyPress::filter(sf::Event const& event)
{
    assert(event != 0);

    if (event.type == sf::Event::KeyPressed)
    {
        for (size_t i(0); i < 4; ++i)
        {
            if (event.key == keys[i])
            {
                labyrinth.player(playerId).move(static_cast<Labyrinth2d::Direction>(std::pow(2, i)));

                return true;
            }
        }

        if (event.key == keys[StepBack])
        {
            labyrinth.player(playerId).stepBack();

            return true;
        }
    }

    return false;
}
