#ifndef LABYRINTH2D_ALGORITHM_FRACTAL_H
#define LABYRINTH2D_ALGORITHM_FRACTAL_H

/*!
 *  \file Fractal.h
 *  \brief Generate a perfect labyrinth with fractals
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <array>
#include <chrono>
#include <deque>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth2d
{
    namespace Algorithm
    {
        /*!
         *  \brief Generate a perfect labyrinth with fractals
         *
         *  A row or a column is randomly chosen while size is not reached.
         *  This line is expanded in orthogonal direction with 5 x 5 (w x h) sized labyrinths.
         */
        class Fractal
        {
            public:
                /*!
                 *  \brief Generate the labyrinth
                 *
                 *  \param g: uniform random number generator
                 *  \param subGrid: sub-grid of labyrinth grid
                 *  \param sleep: sleep function
                 *  \param cycleOperations: number of operations in each cycle
                 *  \param cyclePause: pause time between each cycle
                 *  \param timeout: time before to abort generation
                 */
                template <class URNG>
                void operator()(URNG& g, SubGrid const& subGrid,
                                std::function<void(std::chrono::milliseconds)> const& /*sleep*/ = [] (std::chrono::milliseconds const&) -> void {},
                                size_t /*cycleOperations*/ = 0,
                                std::chrono::milliseconds const& /*cyclePause*/ = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                template <class URNG>
                inline std::array<std::array<bool, 2>, 3> expandPattern_(URNG& g, bool verticalDirection,
                                                                         bool startWall, bool centerWall);
        };
    }
}

template <class URNG>
void Labyrinth2d::Algorithm::Fractal::operator()(URNG& g, SubGrid const& subGrid,
                                                 std::function<void(std::chrono::milliseconds)> const& /*sleep*/,
                                                 size_t /*cycleOperations*/,
                                                 std::chrono::milliseconds const& /*cyclePause*/,
                                                 std::chrono::milliseconds const* timeout)
{
    if (subGrid.rows() < 2 || subGrid.columns() < 2)
        return;

    auto const t(std::chrono::steady_clock::now());

    std::deque<std::deque<bool> > rows(2, {true, false, true});
    std::deque<std::deque<bool> > columns(2, {true, false, true});

    if (g() % 2)
        rows[g() % 2][1] = true; // up or down
    else
        columns[g() % 2][1] = true; // left or right

    bool reachedRows(rows.size() == subGrid.rows());
    bool reachedColumns(columns.size() == subGrid.columns());

    while (!reachedRows || !reachedColumns)
    {
        if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
            return;

        if (timeout != nullptr)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t) > *timeout)
                throw TimeoutException();

        if (!reachedRows && (reachedColumns || g() % 2))
        {
            size_t const i(g() % rows.size());

            std::deque<bool> insertedRow;

            for (size_t k(0); k < columns.size(); k += 2)
            {
                auto const a(expandPattern_(g, true, rows[i][k], rows[i][k + 1]));

                rows[i][k] = a[0][0];
                rows[i][k + 1] = a[1][0];
                insertedRow.push_back(a[0][1]);
                insertedRow.push_back(a[1][1]);
                columns[k].insert(columns[k].begin() + i + 1, a[2][0]);

                if (k + 1 < columns.size())
                    columns[k + 1].insert(columns[k + 1].begin() + i + 1, a[2][1]);
            }

            if (!(columns.size() % 2))
                insertedRow.push_back(true);

            rows.insert(rows.begin() + i + 1, insertedRow);
        }
        else// if (!reachedColumns)
        {
            size_t const j(g() % columns.size());

            std::deque<bool> insertedColumn;

            for (size_t k(0); k < rows.size(); k += 2)
            {
                auto const a(expandPattern_(g, false, columns[j][k], columns[j][k + 1]));

                columns[j][k] = a[0][0];
                columns[j][k + 1] = a[2][0];
                insertedColumn.push_back(a[0][1]);
                insertedColumn.push_back(a[2][1]);
                rows[k].insert(rows[k].begin() + j + 1, a[1][0]);

                if (k + 1 < rows.size())
                    rows[k + 1].insert(rows[k + 1].begin() + j + 1, a[1][1]);
            }

            if (!(rows.size() % 2))
                insertedColumn.push_back(true);

            columns.insert(columns.begin() + j + 1, insertedColumn);
        }

        reachedRows = (rows.size() == subGrid.rows());
        reachedColumns = (columns.size() == subGrid.columns());
    }

    for (size_t i(0); i < rows.size(); ++i)
    {
        for (size_t j(1); j < rows[i].size() - 1; ++j)
            subGrid.change(2 * i + 1, 2 * j, rows[i][j]);
    }

    for (size_t j(0); j < columns.size(); ++j)
    {
        for (size_t i(1); i < columns[j].size() - 1; ++i)
            subGrid.change(2 * i, 2 * j + 1, columns[j][i]);
    }
}

template <class URNG>
inline std::array<std::array<bool, 2>, 3> Labyrinth2d::Algorithm::Fractal::expandPattern_(URNG& g, bool verticalDirection,
                                                                                          bool startWall, bool centerWall)
{
    std::array<std::array<bool, 2>, 3> a{true, true, false, false, false, false};

    if (!startWall)
        a[0][g() % 2] = false; // left up/down if verticalDirection or up left/right if !verticalDirection

    if (centerWall)
    {
        if (verticalDirection)
        {
            a[1][0] = true; // up
            a[1][1] = true; // down
        }
        else
        {
            a[2][0] = true; // left
            a[2][1] = true; // right
        }
    }
    else
        a[1 + g() % 2][g() % 2] = true;

    return a;
}

#endif // LABYRINTH2D_ALGORITHM_FRACTAL_H
