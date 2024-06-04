#ifndef LABYRINTH2D_ALGOTIHM_RECURSIVE_H
#define LABYRINTH2D_ALGOTIHM_RECURSIVE_H

/*!
 *  \file Recursive.h
 *  \brief Generate a labyrinth by sub-divinding the grid and applying the desired algorithm
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <chrono>
#include <stdexcept>
#include <vector>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth2d
{
    namespace Algorithm
    {
        /*!
         *  \brief Generate a labyrinth by sub-divinding the grid and applying the desired algorithm
         *
         *  The grid is divided randomly and the sub-grids can not be smaller than (minimumRos, minimumColumns)
         */
        template <class Algorithm>
        class Recursive
        {
            public:
                /*!
                 *  \brief Constructor
                 *
                 *  \param algorithm: algorithm used to generate the labyrinth
                 *  \param divisionProbabilityPercentage: percentage of division probability
                 *  \param minimumRows: minimum rows of a subgrid
                 *  \param minimumColumns: minimum columns of a subgrid
                 */
                Recursive(Algorithm& algorithm, size_t divisionProbabilityPercentage,
                          size_t minimumRows = 2, size_t minimumColumns = 2);

                /*!
                 *  \brief Return the percentage of division probability
                 */
                size_t divisionProbabilityPercentage() const;

                /*!
                 *  \brief Return the minimum rows of a subgrid
                 */
                size_t minimumRows() const;

                /*!
                 *  \brief Return the minimum columns of a subgrid
                 */
                size_t minimumColumns() const;

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
                void operator()(URNG& g, SubGrid const& subGrid, size_t operationsCycle = 0,
                                std::chrono::milliseconds const& cyclePause = std::chrono::milliseconds(0),
                                std::chrono::milliseconds const* timeout = nullptr);

            private:
                Algorithm& algorithm_;
                size_t divisionProbabilityPercentage_;
                size_t minimumRows_;
                size_t minimumColumns_;
        };
    }
}

template <class Algorithm>
Labyrinth2d::Algorithm::Recursive<Algorithm>::Recursive(Algorithm& algorithm,
                                                        size_t divisionProbabilityPercentage,
                                                        size_t minimumRows,
                                                        size_t minimumColumns) : algorithm_(algorithm),
                                                                                 divisionProbabilityPercentage_(divisionProbabilityPercentage),
                                                                                 minimumRows_(minimumRows),
                                                                                 minimumColumns_(minimumColumns)
{
    if (divisionProbabilityPercentage > 100)
        throw std::invalid_argument("Invalid percentage");

    if (minimumRows < 2 || minimumColumns < 2)
        throw std::invalid_argument("Invalid minimum size");
}

template <class Algorithm>
size_t Labyrinth2d::Algorithm::Recursive<Algorithm>::divisionProbabilityPercentage() const
{
    return divisionProbabilityPercentage_;
}

template <class Algorithm>
size_t Labyrinth2d::Algorithm::Recursive<Algorithm>::minimumRows() const
{
    return minimumRows_;
}

template <class Algorithm>
size_t Labyrinth2d::Algorithm::Recursive<Algorithm>::minimumColumns() const
{
    return minimumColumns_;
}

template <class Algorithm>
template <class URNG>
void Labyrinth2d::Algorithm::Recursive<Algorithm>::operator()(URNG& g, SubGrid const& subGrid,
                                                              size_t operationsCycle,
                                                              std::chrono::milliseconds const& cyclePause,
                                                              std::chrono::milliseconds const* timeout)
{
    if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
        return;

    bool const rowDivision(subGrid.rows() / minimumRows_ > 1);
    bool const columnDivision(subGrid.columns() / minimumColumns_ > 1);

    if ((rowDivision || columnDivision) && g() % 100 + 1 < divisionProbabilityPercentage_)
    {
        size_t const rows(rowDivision ? g() % (subGrid.rows() - 2 * minimumRows_ + 1) + minimumRows_ : subGrid.rows());
        size_t const columns(columnDivision ? g() % (subGrid.columns() - 2 * minimumColumns_ + 1) + minimumColumns_ : subGrid.columns());

        if (rowDivision)
            for (size_t j(0); j < subGrid.columns(); ++j)
                subGrid.change(2 * rows, 2 * j + 1, true);

        if (columnDivision)
            for (size_t i(0); i < subGrid.rows(); ++i)
                subGrid.change(2 * i + 1, 2 * columns, true);

        for (size_t i(0); i < (rowDivision ? 2 : 1); ++i)
            for (size_t j(0); j < (columnDivision ? 2 : 1); ++j)
                operator()(g,
                           SubGrid(subGrid,
                                         i * rows,
                                         j * columns,
                                         i * (subGrid.rows() - 2 * rows) + rows,
                                         j * (subGrid.columns() - 2 * columns) + columns),
                           operationsCycle, cyclePause, timeout);

        std::vector<std::pair<size_t, size_t> > positions;

        positions.reserve(4);

        if (rowDivision)
        {
            positions.push_back(std::make_pair(2 * rows, 2 * (g() % columns) + 1));

            if (columnDivision && subGrid.columns() - columns)
                positions.push_back(std::make_pair(2 * rows, 2 * (g() % (subGrid.columns() - columns) + columns) + 1));
        }

        if (columnDivision)
        {
            positions.push_back(std::make_pair(2 * (g() % rows) + 1, 2 * columns));

            if (rowDivision && subGrid.rows() - rows)
                positions.push_back(std::make_pair(2 * (g() % (subGrid.rows() - rows) + rows) + 1, 2 * columns));
        }

        if (!(positions.size() % 2))
        {
            auto& p(positions[g() % positions.size()]);

            p = positions.back();
            positions.pop_back();
        }

        while (!positions.empty())
        {
            auto& p(positions[g() % positions.size()]);

            subGrid.change(p.first, p.second, false);

            p = positions.back();
            positions.pop_back();
        }
    }
    else
        algorithm_(g, subGrid, operationsCycle, cyclePause, timeout);
}

#endif // LABYRINTH2D_ALGOTIHM_RECURSIVE_H
