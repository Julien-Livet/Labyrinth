#ifndef LABYRINTH2D_ALGORITHM_CELLFUSION_H
#define LABYRINTH2D_ALGORITHM_CELLFUSION_H

/*!
 *  \file CellFusion.h
 *  \brief Generate a perfect labyrinth with cell fusion
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 14/05/2024
 */

#include <vector>
#include <thread>
#include <chrono>

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
             *  \param operationsCycle: number of operations in each cycle
             *  \param cyclePause: pause time between each cycle
             *  \param timeout: time before to abort generation
             */
            template <class URNG>
            void operator()(URNG& g, Grid::SubGrid const& subGrid, size_t operationsCycle = 0,
                            std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                            std::chrono::milliseconds const* timeout = nullptr);
        };
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::CellFusion::operator()(URNG& g, Grid::SubGrid const& subGrid,
                                                    size_t operationsCycle, std::chrono::milliseconds const& cyclePause,
                                                    std::chrono::milliseconds const* timeout)
{
	Grid<std::size_t> grid{subGrid.grid().labyrinth, subGrid.rows(), subGrid.columns()};
	
    auto const t(std::chrono::steady_clock::now());

    size_t const height(subGrid.height());
    size_t const width(subGrid.width());
	
    for (size_t i(0); i < height; ++i)
		for (size_t j(0); j < width; ++j)
			grid.reset(i, j);

    for (int y = 0; y < heigth; y += 2)
        for (int x = 0; x < width; x++)
            grid.change(y, x, 1);

    for (int x = 0; x < width; x += 2)
        for (int y = 0; y < heigth; y++)
            grid.change(i, j, 1);

    for (int y = 1; y < heigth; y += 2)
        for (int x = 1; x < width; x += 2)
            grid.change(i, j, (y - 1) * (width - 1) / 4 + (x - 1) / 2 + 2);


	auto const fill{[&grid](int x, int y, int d, int e) -> void
	{
		grid.change(y + e, x + d, grid.at(y, x));
		grid.change(y + e / 2, x + d / 2, grid.at(y, x);

		if (x + d + 2 < width && grid.at(y + e, x + d + 1) != 1 && grid.at(y + e, x + d + 2) != grid.at(y, x))
			fill(x + d, y + e, 2, 0);
		if (x + d - 2 >= 0 && grid.at(y + e, x + d - 1) != 1 && grid.at(y + e, x + d - 2) != grid.at(y, x))
			fill(x + d, y + e, -2, 0);
		if (y + e + 2 < height && grid.at(y + e + 1, x + d) != 1 && grid.at(y + e + 2, x + d) != grid.at(y, x))
			fill(x + d, y + e, 0, 2);
		if (y + e - 2 >= 0 && grid.at(y + e - 1, x + d) != 1 && grid.at(y + e - 2, x + d) != grid.at(y, x))
			fill(x + d, y + e, 0, -2);
	}};

    int openedDoorNumber = 0;

    do
    {
        if (grid.labyrinth().state() & Labyrinth::StopGenerating)
            return;

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
        if (operationsCycle && cyclePause.count() && !(openedWalls % operationsCycle))
            std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

        int x = (g() % (((width - 2) + 1) / 2)) * 2 + 1;
        int y = (g() % (((height - 2) + 1) / 2)) * 2 + 1;
        int d = 0, e = 0;

        if (openedDoorNumber > 10 * ((width - 1) * (height - 1) / 4 - 1) / 100)
        {
            if (g() % 2)
            {
                d = -4 * (g() % 2) + 2;
                if (x + d >= 0 && x + d < width)
                {
                    if (subGrid.at(y, x) != subGrid.at(y, x + d))
                    {
                        fill(x, y, d, 0);
                        ++openedDoorNumber;
                    }
                }
            }
            else
            {
                e = -4 * (g() % 2) + 2;
                if (y + e >= 0 && y + e < height)
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

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
				if (operationsCycle && cyclePause.count() && !(openedWalls % operationsCycle))
					std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

                x += 2;
                if (x >= width)
                {
                    x = 1;
                    y += 2;
                    if (y >= height)
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

#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)
					if (operationsCycle && cyclePause.count() && !(openedWalls % operationsCycle))
						std::this_thread::sleep_for(cyclePause);
#endif // _GLIBCXX_HAS_GTHREADS && _GLIBCXX_USE_C99_STDINT_TR1

                    if (x + d >= 0 && x + d < width && y + e >= 0 && y + e < height && grid.at(y, x) != grid.at(y + e, x + d))
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
                if (x + 2 < width && grid.at(y, x) != grid.at(y, x + 2))
                {
                    fill(x, y, 2, 0);
                    ++openedDoorNumber;
                    b = true;
                    break;
                }*/
            } while (!b);
        }
    } while (openedDoorNumber != (longueur - 1) * (largeur - 1) / 4 - 1);
	
	for (size_t i(0); i < height; ++i)
	{
		for (size_t j(0); j < width; ++j)
		{
			if (grid.at(i, j))
				subGrid.set(i, j);
			else
				subGrid.reset(i, j);
		}
	}
}

#endif // LABYRINTH2D_ALGORITHM_CELLFUSION_H
