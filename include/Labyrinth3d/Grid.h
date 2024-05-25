#ifndef LABYRINTH3D_GRID_H
#define LABYRINTH3D_GRID_H

/*!
 *  \file Grid.h
 *  \brief Represent a labyrinth grid with booleans
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 20/01/2016
 */

#include <vector>
#include <chrono>

#include "utility.h"

namespace Labyrinth3d
{
    class Labyrinth;

    /*!
     *  \brief Represent a labyrinth grid with booleans
     */
    template<typename CellType = bool>
    class Grid
    {
        public:
            /*!
             *  \brief Construct a grid with border and intersection walls of rows x columns x floors sized labyrinth
             *
             *  \param labyrinth: the labyrinth that this grid represents
             *  \param rows: number of rows
             *  \param columns: number of columns
             *  \param floors: number of floors
             */
            Grid(Labyrinth const& labyrinth, size_t rows, size_t columns, size_t floors);

            /*!
             *  \return The rows count
             */
            size_t rows() const;

            /*!
             *  \return The columns count
             */
            size_t columns() const;

            /*!
             *  \return The floors count
             */
            size_t floors() const;

            /*!
             *  \return The height
             */
            size_t height() const;

            /*!
             *  \return The width
             */
            size_t width() const;

            /*!
             *  \return The depth
             */
            size_t depth() const;

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             *
             *  \return A value which is one if the considered cell is wall
             */
            CellType at(size_t i, size_t j, size_t k) const;

            /*!
             *  \brief Change a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             *  \param value: which is true if the considered cell is wall
             */
            void change(size_t i, size_t j, size_t k, CellType value);

            /*!
             *  \brief Toggle a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             */
            void toggle(size_t i, size_t j, size_t k);

            /*!
             *  \brief Set a cell as a wall
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             */
            void set(size_t i, size_t j, size_t k);

            /*!
             *  \brief Set a cell as a way
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             */
            void reset(size_t i, size_t j, size_t k);

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             *
             *  \return A value which is true if the considered cell is wall
             */
            CellType operator()(size_t i, size_t j, size_t k) const;

            /*!
             * \brief Get a constant reference of the labyrinth
             */
            Labyrinth const& labyrinth() const;

            /*!
             * \brief Get the counter of modifications
             */
            size_t modificationCounter() const;

        private:
            Labyrinth const* labyrinth_;
            size_t height_;
            size_t width_;
            size_t depth_;
            std::vector<CellType> cells_;
            size_t modificationCounter_;
    };

    /*!
     *  \brief Class to manipulate a sub-grid
     */
    template <typename CellType = bool>
    class SubGrid
    {
        public:
            /*!
             *  \brief Authorized operations by manipulating sub-grid
             */
            enum Operation
            {
                Set,
                Reset,
                SetAndReset
            };

            /*!
             *  \brief Construct a subgrid from a sub-grid
             *
             *  \param subGrid: parent sub-grid of a labyrinth
             *  \param rowShift: row shift from the top left corner of parent sub-grid
             *  \param columnShift: column shift from the top left corner of parent sub-grid
             *  \param floorShift: floor shift from the top left corner of parent sub-grid
             *  \param rows: rows of this new sub-grid
             *  \param columns: columns of this new sub-grid
             *  \param floors: floors of this new sub-grid
             */
            SubGrid(SubGrid<CellType> const& subGrid, size_t rowShift, size_t columnShift,
                    size_t floorShift, size_t rows, size_t columns, size_t floors);

            /*!
             *  \brief Construct a subgrid from a grid
             *
             *  Row, column and floor shifts are null. Rows, columns and floors are equal to rows, columns and floors of labyrinth grid.
             *
             *  \param grid: grid of a labyrinth
             *  \param operation: authorized operation
             */
            SubGrid(Grid<CellType> &grid, Operation operation);

            /*!
             *  \return A constant reference to the top left of sub-grid
             */
            Grid<CellType> const& grid() const;

            /*!
             *  \return The row shift from the parent sub-grid
             */
            size_t rowShift() const;

            /*!
             *  \return The column shift from the parent sub-grid
             */
            size_t columnShift() const;

            /*!
             *  \return The floor shift from the parent sub-grid
             */
            size_t floorShift() const;

            /*!
             *  \return The rows count of the sub-grid
             */
            size_t rows() const;

            /*!
             *  \return The columns count of the sub-grid
             */
            size_t columns() const;

            /*!
             *  \return The floors count of the sub-grid
             */
            size_t floors() const;

            /*!
             *  \return The height of the sub-grid
             */
            size_t height() const;

            /*!
             *  \return The width of the sub-grid
             */
            size_t width() const;

            /*!
             *  \return The depth of the sub-grid
             */
            size_t depth() const;

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             *
             *  \return A value which is one if the considered cell is wall
             */
            CellType at(size_t i, size_t j, size_t k) const;

            /*!
             *  \brief Change a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             *  \param value: which is one if the considered cell is wall
             */
            void change(size_t i, size_t j, size_t k, CellType value) const;

            /*!
             *  \brief Toggle a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             */
            void toggle(size_t i, size_t j, size_t k) const;

            /*!
             *  \brief Set a cell as a wall
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             */
            void set(size_t i, size_t j, size_t k) const;

            /*!
             *  \brief Set a cell as a way
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             */
            void reset(size_t i, size_t j, size_t k) const;

            /*!
             *  \return The authorize operation for this sub-grid
             */
            Operation operation() const;

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param k: index in [0; depth() - 1]
             *
             *  \return A value which is one if the considered cell is wall
             */
            CellType operator()(size_t i, size_t j, size_t k) const;

        private:
            Grid<CellType>& grid_;
            size_t rowShift_;
            size_t columnShift_;
            size_t floorShift_;
            size_t rows_;
            size_t columns_;
            size_t floors_;
            Operation operation_;

            SubGrid(SubGrid const&) = delete;
            SubGrid& operator=(SubGrid const&) = delete;
    };
}

#include "Grid.tcc"

#endif // LABYRINTH3D_GRID_H
