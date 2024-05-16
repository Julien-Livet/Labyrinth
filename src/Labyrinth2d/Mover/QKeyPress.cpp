#include <cassert>

#include <QEvent>
#include <QKeyEvent>

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Mover/QKeyPress.h"

Labyrinth2d::Mover::QKeyPress::QKeyPress(Labyrinth& labyrinth_, size_t playerId_,
                                         std::array<Qt::Key, 5> const& keys_, QObject* parent) : QObject(parent),
                                                                                                 labyrinth(labyrinth_),
                                                                                                 playerId(playerId_),
                                                                                                 keys(keys_)
{
}

bool Labyrinth2d::Mover::QKeyPress::filter(QEvent* event)
{
    assert(event != 0);

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent(static_cast<QKeyEvent*>(event));

        for (size_t i(0); i < 4; ++i)
        {
            if (keyEvent->key() == keys[i])
            {
                labyrinth.player(playerId).move(static_cast<Labyrinth2d::Direction>(std::pow(2, i)));

                return true;
            }
        }

        if (keyEvent->key() == keys[StepBack])
        {
            labyrinth.player(playerId).stepBack();

            return true;
        }
    }

    return false;
}

bool Labyrinth2d::Mover::QKeyPress::eventFilter(QObject* object, QEvent* event)
{
    assert(object != 0);
    assert(event != 0);

    bool const result{filter(event)};

    if (result)
        return true;

    return QObject::eventFilter(object, event);
}
