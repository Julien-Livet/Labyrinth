#ifndef LABYRINTH2D_ALGORITHM_CELLFUSION_H
#define LABYRINTH2D_ALGORITHM_CELLFUSION_H

/*!
 *  \file CellFusion.h
 *  \brief Generate a perfect labyrinth with cell fusion
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 14/05/2024
 */

#include <chrono>
#include <functional>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth2d
{
    namespace Algorithm
    {
        /*!
         *  \brief Generate a perfect labyrinth by randomly searching a way
         *
         *  Each cell is marked with a different number.
         *  A random cell with a random direction is chosen: if neighbour number cell
         *  is different, cells are merged (one of both numbers is kept) and the wall
         *  between them is opened.
         */
        struct CellFusion
        {
            /*!
             *  \brief Constructor
             */
            CellFusion();

            /*!
             *  \brief Generate the labyrinth
             *
             *  \param g: uniform random number generator
             *  \param subGrid: sub-grid of labyrinth grid
             *  \param sleep: sleep function
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort generation
             */
            template <class URNG>
            void operator()(URNG& g, SubGrid<bool> const& subGrid,
                            std::function<void(std::chrono::milliseconds)> const& sleep = [] (std::chrono::milliseconds const&) -> void {},
                            size_t operationsCycle = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::CellFusion::operator()(URNG& g, SubGrid<bool> const& subGrid,
                                                    std::function<void(std::chrono::milliseconds)> const& sleep,
                                                    size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                    std::chrono::milliseconds const* timeout)
{
    Grid<std::size_t> grid{subGrid.grid().labyrinth(), subGrid.rows(), subGrid.columns()};

    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());

    for (size_t i(1); i < height - 3; i += 2)
    {
        for (size_t j(2); j < width - 1; j += 2)
        {
            grid.change(i, j, 1);
            grid.change(i + 1, j - 1, 1);
            subGrid.set(i, j);
            subGrid.set(i + 1, j - 1);
        }
    }

    for (size_t i(2); i < height - 1; i += 2)
    {
        grid.change(i, width - 2, 1);
        subGrid.set(i, width - 2);
    }

    for (size_t j(2); j < width - 1; j += 2)
    {
        grid.change(height - 2, j, 1);
        subGrid.set(height - 2, j);
    }

    for (size_t y = 1; y < height; y += 2)
    {
        for (size_t x = 1; x < width; x += 2)
            grid.change(y, x, (y - 1) * (width - 1) / 4 + (x - 1) / 2 + 2);
    }

    std::function<void(int, int, int, int)> fill;

    fill = [&grid, &subGrid, width, height, &fill](int x, int y, int d, int e) -> void
	{
		grid.change(y + e, x + d, grid.at(y, x));
        grid.change(y + e / 2, x + d / 2, grid.at(y, x));
        subGrid.reset(y + e / 2, x + d / 2);

        if (x + d + 2 < static_cast<int>(width) && grid.at(y + e, x + d + 1) != 1 &&
            grid.at(y + e, x + d + 2) != grid.at(y, x))
			fill(x + d, y + e, 2, 0);
		if (x + d - 2 >= 0 && grid.at(y + e, x + d - 1) != 1 && grid.at(y + e, x + d - 2) != grid.at(y, x))
			fill(x + d, y + e, -2, 0);
        if (y + e + 2 < static_cast<int>(height) && grid.at(y + e + 1, x + d) != 1 &&
            grid.at(y + e + 2, x + d) != grid.at(y, x))
			fill(x + d, y + e, 0, 2);
		if (y + e - 2 >= 0 && grid.at(y + e - 1, x + d) != 1 && grid.at(y + e - 2, x + d) != grid.at(y, x))
			fill(x + d, y + e, 0, -2);
    };

    size_t openedDoorNumber = 0;

    do
    {
        if (grid.labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (operationsCycle && cyclePause.count() && !(openedDoorNumber % operationsCycle))
            sleep(cyclePause);

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        int x = (g() % (((width - 2) + 1) / 2)) * 2 + 1;
        int y = (g() % (((height - 2) + 1) / 2)) * 2 + 1;
        int d = 0, e = 0;

        if (openedDoorNumber > 10 * ((width - 1) * (height - 1) / 4 - 1) / 100)
        {
            if (g() % 2)
            {
                d = -4 * (g() % 2) + 2;
                if (x + d >= 0 && x + d < static_cast<int>(width))
                {
                    if (grid.at(y, x) != grid.at(y, x + d))
                    {
                        fill(x, y, d, 0);
                        ++openedDoorNumber;
                    }
                }
            }
            else
            {
                e = -4 * (g() % 2) + 2;
                if (y + e >= 0 && y + e < static_cast<int>(height))
                {
                    if (grid.at(y, x) != grid.at(y + e, x))
                    {
                        fill(x, y, 0, e);
                        ++openedDoorNumber;
                    }
                }
            }
        }
        else
        {
            bool b = false;

            do
            {
				if (grid.labyrinth().state() & Labyrinth::StopGenerating)
					return;

                if (operationsCycle && cyclePause.count() && !(openedDoorNumber % operationsCycle))
                    sleep(cyclePause);

                if (timeout != nullptr)
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                        throw TimeoutException();

                x += 2;
                if (x >= static_cast<int>(width))
                {
                    x = 1;
                    y += 2;
                    if (y >= static_cast<int>(height))
                        y = 1;
                }
                int c = 0;
                d = 0;
                e = 0;
                if (g() % 2)
                    d = -4 * (g() % 2) + 2;
                else
                    e = -4 * (g() % 2) + 2;
                do
                {
					if (grid.labyrinth().state() & Labyrinth::StopGenerating)
						return;

                    if (operationsCycle && cyclePause.count() && !(openedDoorNumber % operationsCycle))
                        sleep(cyclePause);

                    if (timeout != nullptr)
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                            throw TimeoutException();

                    if (x + d >= 0 && x + d < static_cast<int>(width) &&
                        y + e >= 0 && y + e < static_cast<int>(height) &&
                        grid.at(y, x) != grid.at(y + e, x + d))
                    {
                        fill(x, y, d, e);
                        ++openedDoorNumber;
                        b = true;
                        break;
                    }
                    else
                    {
                        if (c == 1)
                        {
                            int f = d;
                            d = e;
                            e = f;
                        }
                        else
                        {
                            d *= -1;
                            e *= -1;
                        }
                        c++;
                    }
                } while (c != 4 && !b);/*
                if (x + 2 < static_cast<int>(width) && grid.at(y, x) != grid.at(y, x + 2))
                {
                    fill(x, y, 2, 0);
                    ++openedDoorNumber;
                    b = true;
                    break;
                }*/
            } while (!b);
        }
    } while (openedDoorNumber != (width - 1) * (height - 1) / 4 - 1);
}

#endif // LABYRINTH2D_ALGORITHM_CELLFUSION_H
