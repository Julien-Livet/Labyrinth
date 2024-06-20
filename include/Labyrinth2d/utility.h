#ifndef LABYRINTH2D_UTILITY_H
#define LABYRINTH2D_UTILITY_H

/*!
 *  \file utility.h
 *  \brief Gather some information about labyrinth
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <cstring>
#include <queue>

namespace Labyrinth2d
{
    /*!
     *  \brief Movement direction
     */
    enum Direction
    {
        Right = 1,
        Up = 2,
        Left = 4,
        Down = 8
    };

    static std::chrono::milliseconds const dummyThreadSpleepingDuration(1);
}

template<typename POD>
std::queue<char>& read(std::queue<char>& data, POD& pod)
{
    // make sure we're not trying to read a complex object this way
    static_assert(std::is_pod<POD>::value, "must be plain old data");

    std::vector<char> buffer{};

    buffer.reserve(sizeof(POD));

    for (std::size_t i{0}; i < sizeof(POD); ++i)
    {
        buffer.emplace_back(data.front());
        data.pop();
    }

    std::memcpy(&pod, buffer.data(), sizeof(POD));

    return data;
}

template<typename POD>
std::queue<char>& write(std::queue<char>& data, POD const& pod)
{
    // make sure we're not trying to write a complex object this way
    static_assert(std::is_pod<POD>::value, "must be plain old data");

    std::vector<char> buffer(sizeof(POD));

    std::memcpy(buffer.data(), &pod, sizeof(POD));

    for (auto const& c : buffer)
        data.emplace(c);

    return data;
}

#endif // LABYRINTH2D_UTILITY_H
