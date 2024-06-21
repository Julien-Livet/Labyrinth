#ifndef LABYRINTH2D_GRID_H
#define LABYRINTH2D_GRID_H

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

namespace Labyrinth2d
{
    class Labyrinth;

    /*!
     *  \brief Represent a labyrinth grid with booleans
     */
    class Grid
    {
        public:
            /*!
             *  \brief Construct a grid with border and intersection walls of rows x columns sized labyrinth
             *
             *  \param labyrinth: the labyrinth that this grid represents
             *  \param rows: number of rows
             *  \param columns: number of columns
             */
            Grid(Labyrinth const& labyrinth, size_t rows, size_t columns);

            /*!
             *  \return The rows count
             */
            size_t rows() const;

            /*!
             *  \return The columns count
             */
            size_t columns() const;

            /*!
             *  \return The height
             */
            size_t height() const;

            /*!
             *  \return The width
             */
            size_t width() const;

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *
             *  \return A boolean value which is true if the considered cell is wall
             */
            bool at(size_t i, size_t j) const;

            /*!
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             *
             *  \return A boolean value which is true if the considered cell is wall
             */
            bool at(std::pair<size_t, size_t> const& index) const;

            /*!
             *  \brief Change a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param value: boolean value which is true if the considered cell is wall
             */
            void change(size_t i, size_t j, bool value);

            /*!
             *  \brief Change a cell state
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             *  \param value: boolean value which is true if the considered cell is wall
             */
            void change(std::pair<size_t, size_t> const& index, bool value);

            /*!
             *  \brief Toggle a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             */
            void toggle(size_t i, size_t j);

            /*!
             *  \brief Toggle a cell state
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             */
            void toggle(std::pair<size_t, size_t> const& index);

            /*!
             *  \brief Set a cell as a wall
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             */
            void set(size_t i, size_t j);

            /*!
             *  \brief Set a cell as a wall
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             */
            void set(std::pair<size_t, size_t> const& index);
			
            /*!
             *  \brief Set a cell as a way
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             */
            void reset(size_t i, size_t j);

            /*!
             *  \brief Set a cell as a way
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             */
            void reset(std::pair<size_t, size_t> const& index);

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *
             *  \return A boolean which is true if the considered cell is wall
             */
            bool operator()(size_t i, size_t j) const;

            /*!
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             *
             *  \return A boolean which is true if the considered cell is wall
             */
            bool operator()(std::pair<size_t, size_t> const& index) const;

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
            std::vector<bool> cells_;
            size_t modificationCounter_;
    };

    /*!
     *  \brief Class to manipulate a sub-grid
     */
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
             *  \param rows: rows of this new sub-grid
             *  \param columns: columns of this new sub-grid
             */
            SubGrid(SubGrid const& subGrid, size_t rowShift, size_t columnShift, size_t rows, size_t columns);

            /*!
             *  \brief Construct a subgrid from a grid
             *
             *  Row and column shifts are null. Rows and columns are equal to rows and columns of labyrinth grid.
             *
             *  \param grid: grid of a labyrinth
             *  \param operation: authorized operation
             */
            SubGrid(Grid &grid, Operation operation);

            /*!
             *  \return A constant reference to the top left of sub-grid
             */
            Grid const& grid() const;

            /*!
             *  \return The row shift from the parent sub-grid
             */
            size_t rowShift() const;

            /*!
             *  \return The column shift from the parent sub-grid
             */
            size_t columnShift() const;

            /*!
             *  \return The rows count of the sub-grid
             */
            size_t rows() const;

            /*!
             *  \return The columns count of the sub-grid
             */
            size_t columns() const;

            /*!
             *  \return The height of the sub-grid
             */
            size_t height() const;

            /*!
             *  \return The width of the sub-grid
             */
            size_t width() const;

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *
             *  \return A boolean value which is true if the considered cell is wall
             */
            bool at(size_t i, size_t j) const;

            /*!
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             *
             *  \return A boolean value which is true if the considered cell is wall
             */
            bool at(std::pair<size_t, size_t> const& index) const;

            /*!
             *  \brief Change a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *  \param value: boolean value which is true if the considered cell is wall
             */
            void change(size_t i, size_t j, bool value) const;

            /*!
             *  \brief Change a cell state
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             *  \param value: boolean value which is true if the considered cell is wall
             */
            void change(std::pair<size_t, size_t> const& index, bool value) const;

            /*!
             *  \brief Toggle a cell state
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             */
            void toggle(size_t i, size_t j) const;

            /*!
             *  \brief Toggle a cell state
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             */
            void toggle(std::pair<size_t, size_t> const& index) const;

            /*!
             *  \brief Set a cell as a wall
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             */
            void set(size_t i, size_t j) const;

            /*!
             *  \brief Set a cell as a wall
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             */
            void set(std::pair<size_t, size_t> const& index) const;

            /*!
             *  \brief Set a cell as a way
             *
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             */
            void reset(size_t i, size_t j) const;

            /*!
             *  \brief Set a cell as a way
             *
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             */
            void reset(std::pair<size_t, size_t> const& index) const;

            /*!
             *  \return The authorize operation for this sub-grid
             */
            Operation operation() const;

            /*!
             *  \param i: index in [0; height() - 1]
             *  \param j: index in [0; width() - 1]
             *
             *  \return A boolean value which is true if the considered cell is wall
             */
            bool operator()(size_t i, size_t j) const;

            /*!
             *  \param index: index in [0; height() - 1]x[0; width() - 1]
             *
             *  \return A boolean value which is true if the considered cell is wall
             */
            bool operator()(std::pair<size_t, size_t> const& index) const;

        private:
            Grid& grid_;
            size_t rowShift_;
            size_t columnShift_;
            size_t rows_;
            size_t columns_;
            Operation operation_;

            SubGrid(SubGrid const&) = delete;
            SubGrid& operator=(SubGrid const&) = delete;
    };
}

#endif // LABYRINTH2D_GRID_H
