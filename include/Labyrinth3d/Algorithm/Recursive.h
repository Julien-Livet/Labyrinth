#ifndef LABYRINTH3D_ALGORITHM_RECURSIVE_H
#define LABYRINTH3D_ALGORITHM_RECURSIVE_H

/*!
 *  \file Recursive.h
 *  \brief Generate a labyrinth by sub-divinding the grid and applying the desired algorithm
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <vector>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "../Grid.h"
#include "../Labyrinth.h"

namespace Labyrinth3d
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
                 *  \param minimumFloors: minimum floors of a subgrid
                 */
                Recursive(Algorithm& algorithm, size_t divisionProbabilityPercentage,
                          size_t minimumRows = 2, size_t minimumColumns = 2, size_t minimumFloors = 2);

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
                 *  \brief Return the minimum floors of a subgrid
                 */
                size_t minimumFloors() const;
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

            private:
                Algorithm& algorithm_;
                size_t divisionProbabilityPercentage_;
                size_t minimumRows_;
                size_t minimumColumns_;
                size_t minimumFloors_;
        };
    }
}

template <class Algorithm>
Labyrinth3d::Algorithm::Recursive<Algorithm>::Recursive(Algorithm& algorithm,
                                                        size_t divisionProbabilityPercentage,
                                                        size_t minimumRows,
                                                        size_t minimumColumns,
                                                        size_t minimumFloors) : algorithm_(algorithm),
                                                                                divisionProbabilityPercentage_(divisionProbabilityPercentage),
                                                                                minimumRows_(minimumRows),
                                                                                minimumColumns_(minimumColumns),
                                                                                minimumFloors_(minimumFloors)
{
    if (divisionProbabilityPercentage > 100)
        throw std::invalid_argument("Invalid percentage");

    if (minimumRows < 2 || minimumColumns < 2 || minimumFloors < 2)
        throw std::invalid_argument("Invalid minimum size");
}

template <class Algorithm>
size_t Labyrinth3d::Algorithm::Recursive<Algorithm>::divisionProbabilityPercentage() const
{
    return divisionProbabilityPercentage_;
}

template <class Algorithm>
size_t Labyrinth3d::Algorithm::Recursive<Algorithm>::minimumRows() const
{
    return minimumRows_;
}

template <class Algorithm>
size_t Labyrinth3d::Algorithm::Recursive<Algorithm>::minimumColumns() const
{
    return minimumColumns_;
}

template <class Algorithm>
size_t Labyrinth3d::Algorithm::Recursive<Algorithm>::minimumFloors() const
{
    return minimumFloors_;
}

template <class Algorithm>
template <class URNG>
void Labyrinth3d::Algorithm::Recursive<Algorithm>::operator()(URNG& g, Grid::SubGrid const& subGrid,
                                                              size_t operationsCycle,
                                                              std::chrono::milliseconds const& cyclePause,
                                                              std::chrono::milliseconds const* timeout)
{
    if (subGrid.grid().labyrinth().state() & Labyrinth::StopGenerating)
        return;

    bool const rowDivision(subGrid.rows() / minimumRows_ > 1);
    bool const columnDivision(subGrid.columns() / minimumColumns_ > 1);
    bool const floorDivision(subGrid.floors() / minimumFloors_ > 1);

    if ((rowDivision || columnDivision || columnDivision) && g() % 100 + 1 < divisionProbabilityPercentage_)
    {
        size_t const rows(rowDivision ? g() % (subGrid.rows() - 2 * minimumRows_ + 1) + minimumRows_ : subGrid.rows());
        size_t const columns(columnDivision ? g() % (subGrid.columns() - 2 * minimumColumns_ + 1) + minimumColumns_ : subGrid.columns());
        size_t const floors(floorDivision ? g() % (subGrid.floors() - 2 * minimumFloors_ + 1) + minimumFloors_ : subGrid.floors());

        for (size_t i(0); i < (rowDivision ? 2 : 1); ++i)
            for (size_t j(0); j < (columnDivision ? 2 : 1); ++j)
                for (size_t k(0); k < (floorDivision ? 2 : 1); ++k)
                    operator()(g,
                               Grid::SubGrid(subGrid,
                                             i * rows,
                                             j * columns,
                                             k * floors,
                                             i * (subGrid.rows() - 2 * rows) + rows,
                                             j * (subGrid.columns() - 2 * columns) + columns,
                                             k * (subGrid.floors() - 2 * floors) + floors),
                               operationsCycle, cyclePause, timeout);

        std::vector<std::tuple<size_t, size_t, size_t> > positions;

        positions.reserve(12);

        if (rowDivision)
        {
            positions.push_back(std::make_tuple(2 * rows, 2 * (g() % columns) + 1, 2 * (g() % floors) + 1));

            if (columnDivision && subGrid.columns() - columns)
                positions.push_back(std::make_tuple(2 * rows, 2 * (g() % (subGrid.columns() - columns) + columns) + 1, 2 * (g() % floors) + 1));

            if (floorDivision && subGrid.floors() - floors)
                positions.push_back(std::make_tuple(2 * rows, 2 * (g() % columns) + 1, 2 * (g() % (subGrid.floors() - floors) + floors) + 1));

            if (columnDivision && subGrid.columns() - columns && floorDivision && subGrid.floors() - floors)
                positions.push_back(std::make_tuple(2 * rows, 2 * (g() % (subGrid.columns() - columns) + columns) + 1, 2 * (g() % (subGrid.floors() - floors) + floors) + 1));
        }

        if (columnDivision)
        {
            positions.push_back(std::make_tuple(2 * (g() % rows) + 1, 2 * columns, 2 * (g() % floors) + 1));

            if (rowDivision && subGrid.rows() - rows)
                positions.push_back(std::make_tuple(2 * (g() % (subGrid.rows() - rows) + rows) + 1, 2 * columns, 2 * (g() % floors) + 1));

            if (floorDivision && subGrid.floors() - floors)
                positions.push_back(std::make_tuple(2 * (g() % rows) + 1, 2 * columns, 2 * (g() % (subGrid.floors() - floors) + floors) + 1));

            if (rowDivision && subGrid.rows() - rows && floorDivision && subGrid.floors() - floors)
                positions.push_back(std::make_tuple(2 * (g() % (subGrid.rows() - rows) + rows) + 1, 2 * columns, 2 * (g() % (subGrid.floors() - floors) + floors) + 1));
        }

        if (floorDivision)
        {
            positions.push_back(std::make_tuple(2 * (g() % rows) + 1, 2 * (g() % columns) + 1, 2 * floors));

            if (rowDivision && subGrid.rows() - rows)
                positions.push_back(std::make_tuple(2 * (g() % (subGrid.rows() - rows) + rows) + 1, 2 * (g() % columns) + 1, 2 * columns));

            if (columnDivision && subGrid.columns() - columns)
                positions.push_back(std::make_tuple(2 * (g() % rows) + 1, 2 * (g() % (subGrid.columns() - columns) + columns) + 1, 2 * floors));

            if (rowDivision && subGrid.rows() - rows && columnDivision && subGrid.columns() - columns)
                positions.push_back(std::make_tuple(2 * (g() % (subGrid.rows() - rows) + rows) + 1, 2 * (g() % (subGrid.columns() - columns) + columns) + 1, 2 * floors));
        }

        if (!(positions.size() % 2))
        {
            auto& p(positions[g() % positions.size()]);

            p = positions.back();
            positions.pop_back();
        }

        while (positions.size() > 1)
        {
            auto& p(positions[g() % positions.size()]);

            subGrid.change(std::get<0>(p), std::get<1>(p), std::get<2>(p), false);

            p = positions.back();
            positions.pop_back();
        }
    }
    else
        algorithm_(g, subGrid, operationsCycle, cyclePause, timeout);
}

#endif // LABYRINTH3D_ALGORITHM_RECURSIVE_H
