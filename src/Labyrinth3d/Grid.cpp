#include <cassert>
#include <stdexcept>

#include "Labyrinth3d/Grid.h"

Labyrinth3d::Grid::Grid(Labyrinth const& labyrinth,
                        size_t rows, size_t columns, size_t floors) : labyrinth_(&labyrinth),
                                                                      height_(rows * 2 + 1),
                                                                      width_(columns * 2 + 1),
                                                                      depth_(floors * 2 + 1),
                                                                      cells_(depth_ * height_ * width_, false),
                                                                          modificationCounter_(0)
{
    if (!rows || !columns || !floors)
        throw std::invalid_argument("Invalid size");

    for (size_t i(0); i < height_; ++i)
    {
        for (size_t j(0); j < width_; ++j)
        {
            cells_[i * width_ + j] = true;
            cells_[((depth_ - 1) * height_ + i) * width_ + j] = true;
        }
    }

    for (size_t i(1); i < height_ - 1; ++i)
    {
        for (size_t k(0); k < depth_; ++k)
        {
            cells_[(k * height_ + i) * width_] = true;
            cells_[(k * height_ + i) * width_ + width_ - 1] = true;
        }
    }

    for (size_t j(0); j < width_; ++j)
    {
        for (size_t k(1); k < depth_ - 1; ++k)
        {
            cells_[(k * height_) * width_ + j] = true;
            cells_[(k * height_ + height_ - 1) * width_ + j] = true;
        }
    }

    for (size_t i(2); i < height_ - 1; i += 2)
    {
        for (size_t j(2); j < width_ - 1; j += 2)
        {
            for (size_t k(2); k < depth_ - 1; k += 2)
                cells_[(k * height_ + i) * width_ + j] = true;
        }
    }
}

size_t Labyrinth3d::Grid::rows() const
{
    return (height_ - 1) / 2;
}

size_t Labyrinth3d::Grid::columns() const
{
    return (width_ - 1) / 2;
}

size_t Labyrinth3d::Grid::floors() const
{
    return (depth_ - 1) / 2;
}

size_t Labyrinth3d::Grid::height() const
{
    return height_;
}

size_t Labyrinth3d::Grid::width() const
{
    return width_;
}

size_t Labyrinth3d::Grid::depth() const
{
    return depth_;
}

bool Labyrinth3d::Grid::at(size_t i, size_t j, size_t k) const
{
    assert((k * height_ + i) * width_ + j < cells_.size());

    return cells_[(k * height_ + i) * width_ + j];
}

void Labyrinth3d::Grid::change(size_t i, size_t j, size_t k, bool value)
{
    assert((k * height_ + i) * width_ + j < cells_.size());

    if (cells_[(k * height_ + i) * width_ + j] != value)
    {
        cells_[(k * height_ + i) * width_ + j] = value;

        ++modificationCounter_;
    }
}

void Labyrinth3d::Grid::toggle(size_t i, size_t j, size_t k)
{
    assert((k * height_ + i) * width_ + j < cells_.size());

    cells_[(k * height_ + i) * width_ + j] = !cells_[(k * height_ + i) * width_ + j];

    ++modificationCounter_;
}

void Labyrinth3d::Grid::set(size_t i, size_t j, size_t k)
{
    change(i, j, k, true);
}

void Labyrinth3d::Grid::reset(size_t i, size_t j, size_t k)
{
    change(i, j, k, false);
}

bool Labyrinth3d::Grid::operator()(size_t i, size_t j, size_t k) const
{
    return at(i, j, k);
}

Labyrinth3d::Labyrinth const& Labyrinth3d::Grid::labyrinth() const
{
    assert(labyrinth_ != nullptr);

    return *labyrinth_;
}

size_t Labyrinth3d::Grid::modificationCounter() const
{
    return modificationCounter_;
}

Labyrinth3d::SubGrid::SubGrid(SubGrid const& subGrid, size_t rowShift, size_t columnShift, size_t floorShift,
                              size_t rows, size_t columns, size_t floors) : grid_(subGrid.grid_),
                                                                            rowShift_(subGrid.rowShift_ + rowShift),
                                                                            columnShift_(subGrid.columnShift_ + columnShift),
                                                                            floorShift_(subGrid.floorShift_ + floorShift),
                                                                            rows_(rows), columns_(columns), floors_(floors),
                                                                            operation_(subGrid.operation_)
{
    if (rowShift >= subGrid.rows_ || columnShift >= subGrid.columns_ || floorShift >= subGrid.floors_)
        throw std::invalid_argument("Invalid indices");

    if (rows < 2 || columns < 2 || floors < 2
        || rows > grid_.rows() - rowShift || columns > grid_.columns() - columnShift || floors > grid_.floors() - floorShift)
        throw std::invalid_argument("Invalid size");
}

Labyrinth3d::SubGrid::SubGrid(Grid &grid, Operation operation) : grid_(grid),
                                                                 rowShift_(0), columnShift_(0), floorShift_(0),
                                                                 rows_(grid_.rows()), columns_(grid_.columns()),
                                                                 floors_(grid_.floors()),
                                                                 operation_(operation)
{
}

Labyrinth3d::Grid const& Labyrinth3d::SubGrid::grid() const
{
    return grid_;
}

size_t Labyrinth3d::SubGrid::rowShift() const
{
    return rowShift_;
}

size_t Labyrinth3d::SubGrid::columnShift() const
{
    return columnShift_;
}

size_t Labyrinth3d::SubGrid::floorShift() const
{
    return floorShift_;
}

size_t Labyrinth3d::SubGrid::rows() const
{
    return rows_;
}

size_t Labyrinth3d::SubGrid::columns() const
{
    return columns_;
}

size_t Labyrinth3d::SubGrid::floors() const
{
    return floors_;
}

size_t Labyrinth3d::SubGrid::height() const
{
    return 2 * rows_ + 1;
}

size_t Labyrinth3d::SubGrid::width() const
{
    return 2 * columns_ + 1;
}

size_t Labyrinth3d::SubGrid::depth() const
{
    return 2 * floors_ + 1;
}

bool Labyrinth3d::SubGrid::at(size_t i, size_t j, size_t k) const
{
    if (i >= height() || j >= width() || k >= depth())
        throw std::invalid_argument("Invalid indices");

    return grid_(2 * rowShift_ + i, 2 * columnShift_ + j, 2 * floorShift_ + k);
}

void Labyrinth3d::SubGrid::change(size_t i, size_t j, size_t k, bool value) const
{
    if (i >= height() - 1 || j >= width() - 1 || k >= depth() - 1 || !i || !j || !k || i % 2 + j % 2 + k % 2 == 3)
        throw std::invalid_argument("Invalid indices");

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.change(2 * rowShift_ + i, 2 * columnShift_ + j, 2 * floorShift_ + k, value);
}

void Labyrinth3d::SubGrid::toggle(size_t i, size_t j, size_t k) const
{
    if (i >= height() - 1 || j >= width() - 1 || k >= depth() - 1 || !i || !j || !k || i % 2 + j % 2 + k % 2 == 3)
        throw std::invalid_argument("Invalid indices");

    bool const value(!at(i, j, k));

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.toggle(2 * rowShift_ + i, 2 * columnShift_ + j, 2 * floorShift_ + k);
}

void Labyrinth3d::SubGrid::set(size_t i, size_t j, size_t k) const
{
    if (operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");

    change(i, j, k, true);
}

void Labyrinth3d::SubGrid::reset(size_t i, size_t j, size_t k) const
{
    if (operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    change(i, j, k, false);
}

Labyrinth3d::SubGrid::Operation Labyrinth3d::SubGrid::operation() const
{
    return operation_;
}

bool Labyrinth3d::SubGrid::operator()(size_t i, size_t j, size_t k) const
{
    return at(i, j, k);
}
