#include <cassert>
#include <stdexcept>

#include "Labyrinth2d/Grid.h"

Labyrinth2d::Grid::Grid(Labyrinth const& labyrinth,
                        size_t rows, size_t columns) : labyrinth_(&labyrinth),
                                                       height_(rows * 2 + 1), width_(columns * 2 + 1),
                                                       cells_(height_ * width_, false),
                                                       modificationCounter_(0)
{
    if (!rows || !columns)
        throw std::invalid_argument("Invalid size");

    for (size_t j(0); j < width_; ++j)
    {
        cells_[j] = true;
        cells_[(height_ - 1) * width_ + j] = true;
    }

    for (size_t i(1); i < height_ - 1; ++i)
    {
        cells_[width_ * i] = true;
        cells_[(i + 1) * width_ - 1] = true;
    }

    for (size_t i(2); i < height_ - 1; i += 2)
    {
        for (size_t j(2); j < width_ - 1; j += 2)
            cells_[i * width_ + j] = true;
    }
}

size_t Labyrinth2d::Grid::rows() const
{
    return (height_ - 1) / 2;
}

size_t Labyrinth2d::Grid::columns() const
{
    return (width_ - 1) / 2;
}

size_t Labyrinth2d::Grid::height() const
{
    return height_;
}

size_t Labyrinth2d::Grid::width() const
{
    return width_;
}

bool Labyrinth2d::Grid::at(size_t i, size_t j) const
{
    assert(i * width_ + j < cells_.size());

    return cells_[i * width_ + j];
}

bool Labyrinth2d::Grid::at(std::pair<size_t, size_t> const& index) const
{
    return at(index.first, index.second);
}

void Labyrinth2d::Grid::change(size_t i, size_t j, bool value)
{
    assert(i * width_ + j < cells_.size());

    if (cells_[i * width_ + j] != value)
    {
        cells_[i * width_ + j] = value;

        ++modificationCounter_;
    }
}

void Labyrinth2d::Grid::change(std::pair<size_t, size_t> const& index, bool value)
{
    change(index.first, index.second, value);
}

void Labyrinth2d::Grid::toggle(size_t i, size_t j)
{
    assert(i * width_ + j < cells_.size());

    cells_[i * width_ + j] = !cells_[i * width_ + j];

    ++modificationCounter_;
}

void Labyrinth2d::Grid::toggle(std::pair<size_t, size_t> const& index)
{
    toggle(index.first, index.second);
}

void Labyrinth2d::Grid::set(size_t i, size_t j)
{
    change(i, j, true);
}

void Labyrinth2d::Grid::set(std::pair<size_t, size_t> const& index)
{
    set(index.first, index.second);
}

void Labyrinth2d::Grid::reset(size_t i, size_t j)
{
    change(i, j, false);
}

void Labyrinth2d::Grid::reset(std::pair<size_t, size_t> const& index)
{
    reset(index.first, index.second);
}

bool Labyrinth2d::Grid::operator()(size_t i, size_t j) const
{
    return at(i, j);
}

bool Labyrinth2d::Grid::operator()(std::pair<size_t, size_t> const& index) const
{
    return at(index.first, index.second);
}

Labyrinth2d::Labyrinth const& Labyrinth2d::Grid::labyrinth() const
{
    assert(labyrinth_ != nullptr);

    return *labyrinth_;
}

size_t Labyrinth2d::Grid::modificationCounter() const
{
    return modificationCounter_;
}

Labyrinth2d::SubGrid::SubGrid(SubGrid const& subGrid, size_t rowShift, size_t columnShift,
                              size_t rows, size_t columns) : grid_(subGrid.grid_),
                                                             rowShift_(subGrid.rowShift_ + rowShift),
                                                             columnShift_(subGrid.columnShift_ + columnShift),
                                                             rows_(rows), columns_(columns),
                                                             operation_(subGrid.operation_)
{
    if (rowShift >= subGrid.rows_ || columnShift >= subGrid.columns_)
        throw std::invalid_argument("Invalid indices");

    if (rows_ < 2 || columns_ < 2 || rows_ > grid_.rows() - rowShift || columns_ > grid_.columns() - columnShift)
        throw std::invalid_argument("Invalid size");
}

Labyrinth2d::SubGrid::SubGrid(Grid &grid, Operation operation) : grid_(grid),
                                                                 rowShift_(0), columnShift_(0),
                                                                 rows_(grid_.rows()), columns_(grid_.columns()),
                                                                 operation_(operation)
{
}

Labyrinth2d::Grid const& Labyrinth2d::SubGrid::grid() const
{
    return grid_;
}

size_t Labyrinth2d::SubGrid::rowShift() const
{
    return rowShift_;
}

size_t Labyrinth2d::SubGrid::columnShift() const
{
    return columnShift_;
}

size_t Labyrinth2d::SubGrid::rows() const
{
    return rows_;
}

size_t Labyrinth2d::SubGrid::columns() const
{
    return columns_;
}

size_t Labyrinth2d::SubGrid::height() const
{
    return 2 * rows_ + 1;
}

size_t Labyrinth2d::SubGrid::width() const
{
    return 2 * columns_ + 1;
}

bool Labyrinth2d::SubGrid::at(size_t i, size_t j) const
{
    if (i >= height() || j >= width())
        throw std::invalid_argument("Invalid indices");

    return grid_(2 * rowShift_ + i, 2 * columnShift_ + j);
}

bool Labyrinth2d::SubGrid::at(std::pair<size_t, size_t> const& index) const
{
    return at(index.first, index.second);
}

void Labyrinth2d::SubGrid::change(size_t i, size_t j, bool value) const
{
    if (i >= height() - 1 || j >= width() - 1 || !i || !j || i % 2 == j % 2)
        throw std::invalid_argument("Invalid indices");

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.change(2 * rowShift_ + i, 2 * columnShift_ + j, value);
}

void Labyrinth2d::SubGrid::change(std::pair<size_t, size_t> const& index, bool value) const
{
    change(index.first, index.second, value);
}

void Labyrinth2d::SubGrid::toggle(size_t i, size_t j) const
{
    if (i >= height() - 1 || j >= width() - 1 || !i || !j || i % 2 == j % 2)
        throw std::invalid_argument("Invalid indices");

    bool const value(!at(i, j));

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.toggle(2 * rowShift_ + i, 2 * columnShift_ + j);
}

void Labyrinth2d::SubGrid::toggle(std::pair<size_t, size_t> const& index) const
{
    toggle(index.first, index.second);
}

void Labyrinth2d::SubGrid::set(size_t i, size_t j) const
{
    if (operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");

    change(i, j, true);
}

void Labyrinth2d::SubGrid::set(std::pair<size_t, size_t> const& index) const
{
    set(index.first, index.second);
}

void Labyrinth2d::SubGrid::reset(size_t i, size_t j) const
{
    if (operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    change(i, j, false);
}

void Labyrinth2d::SubGrid::reset(std::pair<size_t, size_t> const& index) const
{
    reset(index.first, index.second);
}

Labyrinth2d::SubGrid::Operation Labyrinth2d::SubGrid::operation() const
{
    return operation_;
}

bool Labyrinth2d::SubGrid::operator()(size_t i, size_t j) const
{
    return at(i, j);
}

bool Labyrinth2d::SubGrid::operator()(std::pair<size_t, size_t> const& index) const
{
    return at(index.first, index.second);
}
