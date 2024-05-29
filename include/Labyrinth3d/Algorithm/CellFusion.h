#ifndef LABYRINTH3D_ALGORITHM_CELLFUSION_H
#define LABYRINTH3D_ALGORITHM_CELLFUSION_H

/*!
 *  \file CellFusion.h
 *  \brief Generate a perfect labyrinth with cell fusion
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 24/05/2024
 */

#include <chrono>
#include <functional>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth3d
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
#include <QDebug>
template <class URNG>
void Labyrinth3d::Algorithm::CellFusion::operator()(URNG& g, SubGrid<bool> const& subGrid,
                                                    std::function<void(std::chrono::milliseconds)> const& sleep,
                                                    size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                    std::chrono::milliseconds const* timeout)
{
    Grid<std::size_t> grid{subGrid.grid().labyrinth(), subGrid.rows(), subGrid.columns(), subGrid.floors()};

    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
    size_t const depth(subGrid.depth());

    for (size_t k(1); k < depth - 1; ++k)
    {
        for (size_t i(1); i < height - 3; i += 2)
        {
            for (size_t j(2); j < width - 1; j += 2)
            {
                grid.change(i, j, k, 1);
                grid.change(i + 1, j - 1, k, 1);
                subGrid.set(i, j, k);
                subGrid.set(i + 1, j - 1, k);
            }
        }

        for (size_t i(2); i < height - 1; i += 2)
        {
            grid.change(i, width - 2, k, 1);
            subGrid.set(i, width - 2, k);
        }

        for (size_t j(2); j < width - 1; j += 2)
        {
            grid.change(height - 2, j, k, 1);
            subGrid.set(height - 2, j, k);
        }
    }

    for (size_t k(2); k < depth - 1; k += 2)
    {
        for (size_t i(1); i < height; i += 2)
        {
            for (size_t j(1); j < width; j += 2)
            {
                grid.change(i, j, k, 1);
                subGrid.set(i, j, k);
            }
        }
    }

    for (size_t k(1); k < depth - 1; k += 2)
    {
        for (size_t i(2); i < height - 1; i += 2)
        {
            for (size_t j(2); j < width - 1; j += 2)
            {
                grid.change(i, j, k, 1);
                subGrid.set(i, j, k);
            }
        }
    }

    for (size_t z = 1; z < depth ; z += 2)
    {
        for (size_t y = 1; y < height; y += 2)
        {
            for (size_t x = 1; x < width; x += 2)
                grid.change(y, x, z, ((z - 1) * (height - 1) / 2 + (y - 1)) * (width - 1) / 4 + (x - 1) / 2 + 2);
        }
    }

    std::function<void(int, int, int, int, int, int)> fill;

    fill = [&grid, &subGrid, width, height, depth, &fill](int x, int y, int z, int d, int e, int f) -> void
	{
        grid.change(y + e, x + d, z + f, grid.at(y, x, z));
        grid.change(y + e / 2, x + d / 2, z + f / 2, grid.at(y, x, z));
        subGrid.reset(y + e / 2, x + d / 2, z + f / 2);

        if (x + d + 2 < static_cast<int>(width)
            && grid.at(y + e, x + d + 1, z + f) != 1
            && grid.at(y + e, x + d + 2, z + f) != grid.at(y, x, z))
            fill(x + d, y + e, z + f, 2, 0, 0);
        if (x + d - 2 >= 0
            && grid.at(y + e, x + d - 1, z + f) != 1
            && grid.at(y + e, x + d - 2, z + f) != grid.at(y, x, z))
            fill(x + d, y + e, z + f, -2, 0, 0);
        if (y + e + 2 < static_cast<int>(height)
            && grid.at(y + e + 1, x + d, z + f) != 1
            && grid.at(y + e + 2, x + d, z + f) != grid.at(y, x, z))
            fill(x + d, y + e, z + f, 0, 2, 0);
        if (y + e - 2 >= 0
            && grid.at(y + e - 1, x + d, z + f) != 1
            && grid.at(y + e - 2, x + d, z + f) != grid.at(y, x, z))
            fill(x + d, y + e, z + f, 0, -2, 0);
        if (z + f + 2 < static_cast<int>(depth)
            && grid.at(x + e, x + d, z + f + 1) != 1
            && grid.at(y + e, x + d, z + f + 2) != grid.at(y, x, z))
            fill(x + d, y + e, z + f, 0, 0, 2);
        if (z + f - 2 >= 0
            && grid.at(y + e, x + d, z + f - 1) != 1
            && grid.at(y + e, x + d, z + f - 2) != grid.at(y, x, z))
            fill(x + d, y + e, z + f, 0, 0, -2);
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
        int z = (g() % (((depth - 2) + 1) / 2)) * 2 + 1;
        int d = 0, e = 0, f = 0;

        if (openedDoorNumber > 10 * ((depth - 1) * (width - 1) * (height - 1) / 8 - 1) / 100)
        {
            auto const v{g() % 3};
            if (!v)
            {
                d = -4 * (g() % 2) + 2;
                if (x + d >= 0 && x + d < static_cast<int>(width))
                {
                    if (grid.at(y, x, z) != grid.at(y, x + d, z))
                    {
                        fill(x, y, z, d, 0, 0);
                        ++openedDoorNumber;
                    }
                }
            }
            else if (v == 1)
            {
                e = -4 * (g() % 2) + 2;
                if (y + e >= 0 && y + e < static_cast<int>(height))
                {
                    if (grid.at(y, x, z) != grid.at(y + e, x, z))
                    {
                        fill(x, y, z, 0, e, 0);
                        ++openedDoorNumber;
                    }
                }
            }
            else
            {
                f = -4 * (g() % 2) + 2;
                if (z + f >= 0 && z + f < static_cast<int>(depth))
                {
                    if (grid.at(y, x, z) != grid.at(y, x, z + f))
                    {
                        fill(x, y, z, 0, 0, f);
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

                std::vector<std::tuple<int, int, int> > possibilities
                {
                    std::make_tuple(0, 2, 0), std::make_tuple(2, 0, 0),
                    std::make_tuple(0, -2, 0), std::make_tuple(-2, 0, 0),
                    std::make_tuple(0, 0, -2), std::make_tuple(0, 0, 2)
                };

                while (!b && !possibilities.empty())
                {
                    auto& p{possibilities[g() % possibilities.size()]};
                    d = std::get<0>(p);
                    e = std::get<1>(p);
                    f = std::get<2>(p);
                    p = possibilities.back();
                    possibilities.pop_back();

					if (grid.labyrinth().state() & Labyrinth::StopGenerating)
						return;

                    if (operationsCycle && cyclePause.count() && !(openedDoorNumber % operationsCycle))
                        sleep(cyclePause);

                    if (timeout != nullptr)
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                            throw TimeoutException();

                    if (x + d >= 0 && x + d < static_cast<int>(width)
                        && y + e >= 0 && y + e < static_cast<int>(height)
                        && z + f >= 0 && z + f < static_cast<int>(depth)
                        && grid.at(y, x, z) != grid.at(y + e, x + d, z + f))
                    {
                        fill(x, y, z, d, e, f);
                        ++openedDoorNumber;
                        b = true;
                        break;
                    }
                }

                x += 2;
                if (x >= static_cast<int>(width))
                {
                    x = 1;
                    y += 2;
                    if (y >= static_cast<int>(height))
                    {
                        y = 1;
                        z += 2;
                        if (z >= static_cast<int>(depth))
                            z = 1;
                    }
                }
            } while (!b);
        }
    } while (openedDoorNumber != (depth - 1) * (width - 1) * (height - 1) / 8 - 1);
}

#endif // LABYRINTH3D_ALGORITHM_CELLFUSION_H
