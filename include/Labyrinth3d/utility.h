#ifndef LABYRINTH3D_UTILITY_H
#define LABYRINTH3D_UTILITY_H

/*!
 *  \file utility.h
 *  \brief Gather some information about labyrinth
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>

namespace Labyrinth3d
{
    /*!
     *  \brief Movement direction
     */
    enum Direction
    {
        Right = 1,
        Up = 2,
        Left = 4,
        Down = 8,
        Front = 16,
        Back = 32
    };

    static std::chrono::milliseconds const dummyThreadSpleepingDuration(1);
}

#endif // LABYRINTH3D_UTILITY_H
