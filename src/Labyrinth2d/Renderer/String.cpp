#include <cassert>
#include <utility>
#include <algorithm>

#include "Labyrinth2d/Renderer/String.h"

Labyrinth2d::Renderer::String::Direction::Direction(char character) : none(character),
                                                                     rightUpLeftDown(character),
                                                                     rightUpLeft(character),
                                                                     upLeftDown(character),
                                                                     leftDownRight(character),
                                                                     downRightUp(character),
                                                                     rightLeft(character),
                                                                     upDown(character),
                                                                     rightUp(character),
                                                                     upLeft(character),
                                                                     leftDown(character),
                                                                     downRight(character),
                                                                     right(character),
                                                                     up(character),
                                                                     left(character),
                                                                     down(character)
{
}

Labyrinth2d::Renderer::String::Direction::Direction(char none_, char rightUpLeftDown_,
                                                    char rightUpLeft_, char upLeftDown_, char leftDownRight_, char downRightUp_,
                                                    char rightLeft_, char upDown_, char rightUp_, char upLeft_, char leftDown_, char downRight_,
                                                    char right_, char up_, char left_, char down_) : none(none_), rightUpLeftDown(rightUpLeftDown_),
                                                                                                     rightUpLeft(rightUpLeft_),
                                                                                                     upLeftDown(upLeftDown_),
                                                                                                     leftDownRight(leftDownRight_),
                                                                                                     downRightUp(downRightUp_),
                                                                                                     rightLeft(rightLeft_),
                                                                                                     upDown(upDown_),
                                                                                                     rightUp(rightUp_),
                                                                                                     upLeft(upLeft_),
                                                                                                     leftDown(leftDown_),
                                                                                                     downRight(downRight_),
                                                                                                     right(right_),
                                                                                                     up(up_),
                                                                                                     left(left_),
                                                                                                     down(down_)
{
}

Labyrinth2d::Renderer::String::PlayerRenderer::PlayerRenderer(bool displayPlayer_, bool displayTrace_) : player('X'),
                                                                                                         tracesDirection('.', static_cast<char>(197),
                                                                                                                        static_cast<char>(193), static_cast<char>(180), static_cast<char>(194), static_cast<char>(195),
                                                                                                                        static_cast<char>(196), static_cast<char>(179), static_cast<char>(192), static_cast<char>(217), static_cast<char>(191), static_cast<char>(218),
                                                                                                                        static_cast<char>(196), static_cast<char>(179), static_cast<char>(196), static_cast<char>(179)),
                                                                                                         displayPlayer(displayPlayer_),
                                                                                                         displayTrace(displayTrace_)
{
}

Labyrinth2d::Renderer::String::PlayerRenderer::PlayerRenderer(char player_, char tracesDirection_,
                                                             bool displayPlayer_, bool displayTrace_) : player(player_),
                                                                                                        tracesDirection(tracesDirection_),
                                                                                                        displayPlayer(displayPlayer_),
                                                                                                        displayTrace(displayTrace_)
{
}

Labyrinth2d::Renderer::String::String(Labyrinth const& labyrinth_): labyrinth(labyrinth_),
                                                                    waysDirection(' '),
                                                                    wallsDirection('.', static_cast<char>(197),
                                                                                   static_cast<char>(193), static_cast<char>(180), static_cast<char>(194), static_cast<char>(195),
                                                                                   static_cast<char>(196), static_cast<char>(179), static_cast<char>(192), static_cast<char>(217), static_cast<char>(191), static_cast<char>(218),
                                                                                   static_cast<char>(196), static_cast<char>(179), static_cast<char>(196), static_cast<char>(179)),
                                                                    backgroundCharacter(' '),
                                                                    displayWalls(true),
                                                                    displayWays(true),
                                                                    displayPlayers(true)
{
}

Labyrinth2d::Renderer::String::String(Labyrinth const& labyrinth_, char way, char wall) : labyrinth(labyrinth_),
                                                                                          waysDirection(way),
                                                                                          wallsDirection(wall),
                                                                                          backgroundCharacter(' '),
                                                                                          displayWalls(true),
                                                                                          displayWays(true),
                                                                                          displayPlayers(true)
{
}

Labyrinth2d::Renderer::String::String(Labyrinth const& labyrinth_,
                                      Direction const& waysDirection_,
                                      Direction const& wallsDirection_) : labyrinth(labyrinth_),
                                                                          waysDirection(waysDirection_),
                                                                          wallsDirection(wallsDirection_),
                                                                          backgroundCharacter(' '),
                                                                          displayWalls(true),
                                                                          displayWays(true),
                                                                          displayPlayers(true)
{
}

std::string Labyrinth2d::Renderer::String::operator()()
{
    auto const& grid(labyrinth.grid());
    size_t const height(grid.height());
    size_t const width(grid.width());

    std::vector<char> str(height * (width + 1) + 1, '#');
    str.back() = 0;

    for (size_t i(0); i < height; ++i)
    {
        for (size_t j(0); j < width; ++j)
        {
            char& c(str[i * (width + 1) + j]);

            if (!grid(i, j))
            {
                if (!displayPlayers)
                    continue;

                bool ok(false);
                size_t playerId(0);

                for (auto id : labyrinth.playerIds())
                {
                    if (labyrinth.player(id).i() == i && labyrinth.player(id).j() == j && playerRenderers[id].displayPlayer)
                    {
                        playerId = id;
                        ok = true;
                        break;
                    }
                }

                if (ok)
                    c = playerRenderers[playerId].player;
                else
                {
                    size_t traces(0);
                    size_t traceDirections(0);

                    auto findDirections([this, i, j, &traces, &traceDirections, &ok, &playerId]
                    (size_t id, Player const& player,
                     size_t previousTraceIndex, std::pair<size_t, size_t> const& position)
                    {
                         if (i == player.traceIntersections()[previousTraceIndex].first && i == position.first
                             && ((player.traceIntersections()[previousTraceIndex].second <= j && j < position.second)
                                 || (position.second < j && j <= player.traceIntersections()[previousTraceIndex].second)))
                         {
                             if (playerRenderers[id].displayTrace)
                             {
                                 if (j > position.second)
                                 {
                                     ++traces;
                                     traceDirections |= Left;
                                 }
                                 else if (j < position.second)
                                 {
                                     ++traces;
                                     traceDirections |= Right;
                                 }
                                 else
                                     assert(0);

                                 if (player.traceIntersections()[previousTraceIndex].second == j)
                                 {
                                     if (previousTraceIndex - 1 < player.traceIntersections().size())
                                     {
                                         ++traces;

                                         if (i < player.traceIntersections()[previousTraceIndex - 1].first)
                                             traceDirections |= Down;
                                         else if (i > player.traceIntersections()[previousTraceIndex - 1].first)
                                             traceDirections |= Up;
                                         else
                                             assert(0);
                                     }
                                 }
                                 else
                                 {
                                     ++traces;
                                     traceDirections |= Left | Right;
                                 }

                                 playerId = id;
                                 ok = true;
                             }
                         }
                         else if (j == player.traceIntersections()[previousTraceIndex].second && j == position.second
                                  && ((player.traceIntersections()[previousTraceIndex].first <= i && i < position.first)
                                      || (position.first < i && i <= player.traceIntersections()[previousTraceIndex].first)))
                         {
                             if (playerRenderers[id].displayTrace)
                             {
                                 if (i > position.first)
                                 {
                                     ++traces;
                                     traceDirections |= Up;
                                 }
                                 else if (i < position.first)
                                 {
                                     ++traces;
                                     traceDirections |= Down;
                                 }
                                 else
                                     assert(0);

                                 if (player.traceIntersections()[previousTraceIndex].first == i)
                                 {
                                     if (previousTraceIndex - 1 < player.traceIntersections().size())
                                     {
                                         ++traces;

                                         if (j < player.traceIntersections()[previousTraceIndex - 1].second)
                                             traceDirections |= Right;
                                         else if (j > player.traceIntersections()[previousTraceIndex - 1].second)
                                             traceDirections |= Left;
                                         else
                                             assert(0);
                                     }
                                 }
                                 else
                                 {
                                     ++traces;
                                     traceDirections |= Up | Down;
                                 }

                                 playerId = id;
                                 ok = true;
                             }
                         }
                    });

                    for (auto id : labyrinth.playerIds())
                    {
                        auto const& player(labyrinth.player(id));

                        if (player.enabledTrace() && !player.traceIntersections().empty())
                        {
                            findDirections(id, player, player.traceIntersections().size() - 1, std::make_pair(player.i(), player.j()));

                            if (ok)
                                break;

                            for (size_t k(1); k < player.traceIntersections().size(); ++k)
                            {
                                findDirections(id, player, k - 1, player.traceIntersections()[k]);

                                if (ok)
                                    break;
                            }

                            if (ok)
                                break;
                        }
                    }

                    if (ok)
                    {
                        if (i % 2 && j % 2)
                            c = directionCharacter_(traceDirections, traces, playerRenderers[playerId].tracesDirection);
                        else if (!(i % 2) && j % 2)
                            c = playerRenderers[playerId].tracesDirection.upDown;
                        else if (i % 2 && !(j % 2))
                            c = playerRenderers[playerId].tracesDirection.rightLeft;
                        else
                            assert(0);
                    }
                    else
                    {
                        if (!displayWays)
                            continue;

                        if (i % 2 && j % 2)
                        {
                            size_t ways(0);
                            size_t wayDirections(0);

                            if (i - 1 < grid.height() && !grid(i - 1, j))
                            {
                                ++ways;
                                wayDirections |= Up;
                            }

                            if (j + 1 < grid.width() && !grid(i, j + 1))
                            {
                                ++ways;
                                wayDirections |= Right;
                            }

                            if (i + 1 < grid.height() && !grid(i + 1, j))
                            {
                                ++ways;
                                wayDirections |= Down;
                            }

                            if (j - 1 < grid.width() && !grid(i, j - 1))
                            {
                                ++ways;
                                wayDirections |= Left;
                            }

                            c = directionCharacter_(wayDirections, ways, waysDirection);
                        }
                        else if (!(i % 2) && j % 2)
                            c = waysDirection.upDown;
                        else if (i % 2 && !(j % 2))
                            c = waysDirection.rightLeft;
                        else
                            assert(0);
                    }
                }
            }
            else
            {
                if (!displayWalls)
                    continue;

                if (!(i % 2) && !(j % 2))
                {
                    size_t walls(0);
                    size_t wallDirections(0);

                    if (i - 1 < grid.height() && grid(i - 1, j))
                    {
                        ++walls;
                        wallDirections |= Up;
                    }

                    if (j + 1 < grid.width() && grid(i, j + 1))
                    {
                        ++walls;
                        wallDirections |= Right;
                    }

                    if (i + 1 < grid.height() && grid(i + 1, j))
                    {
                        ++walls;
                        wallDirections |= Down;
                    }

                    if (j - 1 < grid.width() && grid(i, j - 1))
                    {
                        ++walls;
                        wallDirections |= Left;
                    }

                    c = directionCharacter_(wallDirections, walls, wallsDirection);
                }
                else if (i % 2 && !(j % 2))
                    c = wallsDirection.upDown;
                else if (!(i % 2) && j % 2)
                    c = wallsDirection.rightLeft;
                else
                    assert(0);
            }
        }

        str[i * (width + 1) + width] = '\n';
    }

    return std::string(str.data());
}

char Labyrinth2d::Renderer::String::directionCharacter_(size_t directions, size_t directionCount, Direction const& direction)
{
    char c(0);

    switch (directionCount)
    {
        case 0:
            c = direction.none;
            break;

        case 4:
            c = direction.rightUpLeftDown;
            break;

        case 1:
            switch (directions)
            {

                case Right:
                    c = direction.right;
                    break;

                case Up:
                    c = direction.up;
                    break;

                case Left:
                    c = direction.left;
                    break;

                case Down:
                    c = direction.down;
                    break;

                default:
                    assert(0);
                    break;
            }
            break;

        case 2:
            if ((directions & Up) && (directions & Down))
                c = direction.upDown;
            else if ((directions & Left) && (directions & Right))
                c = direction.rightLeft;
            else if ((directions & Up) && (directions & Right))
                c = direction.rightUp;
            else if ((directions & Up) && (directions & Left))
                c = direction.upLeft;
            else if ((directions & Down) && (directions & Right))
                c = direction.downRight;
            else if ((directions & Down) && (directions & Left))
                c = direction.leftDown;
            else
                assert(0);
            break;

        case 3:
            if (!(directions & Up))
                c = direction.leftDownRight;
            else if (!(directions & Right))
                c = direction.upLeftDown;
            else if (!(directions & Down))
                c = direction.rightUpLeft;
            else if (!(directions & Left))
                c = direction.downRightUp;
            else
                assert(0);
            break;
    }

    return c;
}

void Labyrinth2d::Renderer::String::swapWallsAndWaysDirection()
{
    std::swap(wallsDirection, waysDirection);
}

void Labyrinth2d::Renderer::String::cleanPlayerRenderers()
{
    for (auto it(playerRenderers.begin()); it != playerRenderers.end(); ++it)
        if (std::find(labyrinth.playerIds().begin(), labyrinth.playerIds().end(), it->first) == labyrinth.playerIds().end())
            playerRenderers.erase(it->first);
}
