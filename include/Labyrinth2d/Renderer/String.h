#ifndef LABYRINTH2D_RENDERER_STRING_H
#define LABYRINTH2D_RENDERER_STRING_H

/*!
 *  \file String.h
 *  \brief Render a labyrinth with characters and display it in a console, a file...
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <string>
#include <map>

#include "../Labyrinth.h"

namespace Labyrinth2d
{
    namespace Renderer
    {
        /*!
         *  \brief Render a labyrinth with characters and display it in a console, a file...
         *
         *  Each wall configuration is managed.
         */
        struct String
        {
            public:
                struct Direction
                {
                    public:
                        char none;
                        char rightUpLeftDown;
                        char rightUpLeft;
                        char upLeftDown;
                        char leftDownRight;
                        char downRightUp;
                        char rightLeft;
                        char upDown;
                        char rightUp;
                        char upLeft;
                        char leftDown;
                        char downRight;
                        char right;
                        char up;
                        char left;
                        char down;

                        /*!
                         *  \brief Constructor with an unique character for all characters)
                         */
                        Direction(char character);

                        /*!
                         *  \brief Constructor with characters for ways and walls
                         *
                         *  \param none_: character to render no directions
                         *  \param rightUpLeftDown_: character to render a wall with 4 directions
                         *  \param rightLeft_: character to render a right left direction
                         *  \param upDown_: character to render a horizontal direction
                         *  \param rightUp_: character to render a right up direction
                         *  \param upLeft_: character to render an up left direction
                         *  \param leftDown_: character to render a left down direction
                         *  \param downRight_: character to render a down right direction
                         *  \param right_: character to render a right direction
                         *  \param up_: character to render an up direction
                         *  \param left_: character to render a left direction
                         *  \param down_: character to render a down direction
                         */
                        Direction(char none_, char rightUpLeftDown_,
                                  char rightUpLeft_, char upLeftDown_, char leftDownRight_, char downRightUp_,
                                  char rightLeft_, char upDown_, char rightUp_, char upLeft_, char leftDown_, char downRight_,
                                  char right_, char up_, char left_, char down_);
                };

                struct PlayerRenderer
                {
                    public:
                        /*!
                         *  \brief Constructor with default characters that are X for player and cross and other ascii characters for trace
                         *
                         *  \param displayPlayer_: display player
                         *  \param displayTrace_: display trace
                         */
                        PlayerRenderer(bool displayPlayer_ = true, bool displayTrace_ = true);

                        PlayerRenderer(char player_, char tracesDirection_ = '.', bool displayPlayer_ = true, bool displayTrace_ = true);

                        char player;
                        Direction tracesDirection;
                        bool displayPlayer;
                        bool displayTrace;
                };

                Labyrinth const& labyrinth;
                Direction waysDirection;
                Direction wallsDirection;
                char backgroundCharacter;
                std::map<size_t, PlayerRenderer> playerRenderers;
                bool displayWalls;
                bool displayWays;
                bool displayPlayers;

                /*!
                 *  \brief Constructor with default characters that are cross and other ascii characters
                 *
                 *  \param labyrinth_: labyrinth to render
                 */
                String(Labyrinth const& labyrinth_);

                /*!
                 *  \brief Constructor with an unique character for ways and walls
                 *
                 *  \param labyrinth_: labyrinth to render
                 *  \param ways: character to render ways (with a space for example)
                 *  \param walls: character to render walls (with a filled rectangle by default)
                 */
                String(Labyrinth const& labyrinth_, char ways, char walls = static_cast<char>(219));

                /*!
                 *  \brief Constructor with characters for the way and walls
                 *
                 *  \param labyrinth_: labyrinth to render
                 *  \param waysDirection_: character to render ways
                 *  \param wallsDirection_: character to render walls
                 */
                String(Labyrinth const& labyrinth_, Direction const& waysDirection_, Direction const& wallDirection_);

                /*!
                 *  \brief Render the labyrinth in a string
                 */
                std::string operator()();

                /*!
                 *  \brief Swap walls and ways direction
                 */
                void swapWallsAndWaysDirection();

                /*!
                 *  \brief Clean unused player renderers
                 */
                void cleanPlayerRenderers();

            private:
                static char directionCharacter_(size_t directions, size_t directionCount, Direction const& direction);
        };
    }
}

#endif // LABYRINTH2D_RENDERER_STRING_H
