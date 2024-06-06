#ifndef LABYRINTH3D_MOVER_QKEYPRESS_H
#define LABYRINTH3D_MOVER_QKEYPRESS_H

#include <array>

#include <QObject>

namespace Labyrinth3d
{
    class Labyrinth;

    namespace Mover
    {
        class QKeyPress : public QObject
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
                std::array<Qt::Key, 7> keys;

                QKeyPress(Labyrinth& labyrinth_, size_t playerId_,
                          std::array<Qt::Key, 7> const& keys_ = {Qt::Key_Right, Qt::Key_Up,
                                                                 Qt::Key_Left, Qt::Key_Down,
                                                                 Qt::Key_PageUp, Qt::Key_PageDown,
                                                                 Qt::Key_Backspace},
                          QObject* parent = 0);
                bool filter(QEvent* event);

            protected:
                virtual bool eventFilter(QObject* object, QEvent* event);
        };
    }
}

#endif // LABYRINTH3D_MOVER_QKEYPRESS_H
