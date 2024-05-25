#include <cassert>
#include <stdexcept>

template<typename CellType>
Labyrinth2d::Grid<CellType>::Grid(Labyrinth const& labyrinth,
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
        for (size_t j(2); j < width_ - 1; j += 2)
            cells_[i * width_ + j] = true;
}

template<typename CellType>
size_t Labyrinth2d::Grid<CellType>::rows() const
{
    return (height_ - 1) / 2;
}

template<typename CellType>
size_t Labyrinth2d::Grid<CellType>::columns() const
{
    return (width_ - 1) / 2;
}

template<typename CellType>
size_t Labyrinth2d::Grid<CellType>::height() const
{
    return height_;
}

template<typename CellType>
size_t Labyrinth2d::Grid<CellType>::width() const
{
    return width_;
}

template<typename CellType>
CellType Labyrinth2d::Grid<CellType>::at(size_t i, size_t j) const
{
    assert(i * width_ + j < cells_.size());

    return cells_[i * width_ + j];
}

template<typename CellType>
void Labyrinth2d::Grid<CellType>::change(size_t i, size_t j, CellType value)
{
    assert(i * width_ + j < cells_.size());

    if (cells_[i * width_ + j] != value)
    {
        cells_[i * width_ + j] = value;

        ++modificationCounter_;
    }
}

template<typename CellType>
void Labyrinth2d::Grid<CellType>::toggle(size_t i, size_t j)
{
    assert(i * width_ + j < cells_.size());

    cells_[i * width_ + j] = !cells_[i * width_ + j];

    ++modificationCounter_;
}

template<typename CellType>
void Labyrinth2d::Grid<CellType>::set(size_t i, size_t j)
{
    change(i, j, true);
}

template<typename CellType>
void Labyrinth2d::Grid<CellType>::reset(size_t i, size_t j)
{
    change(i, j, false);
}

template<typename CellType>
CellType Labyrinth2d::Grid<CellType>::operator()(size_t i, size_t j) const
{
    return at(i, j);
}

template<typename CellType>
Labyrinth2d::Labyrinth const& Labyrinth2d::Grid<CellType>::labyrinth() const
{
    assert(labyrinth_ != nullptr);

    return *labyrinth_;
}

template<typename CellType>
size_t Labyrinth2d::Grid<CellType>::modificationCounter() const
{
    return modificationCounter_;
}

template<typename CellType>
Labyrinth2d::SubGrid<CellType>::SubGrid(SubGrid<CellType> const& subGrid, size_t rowShift, size_t columnShift,
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

template<typename CellType>
Labyrinth2d::SubGrid<CellType>::SubGrid(Grid<CellType> &grid, Operation operation) : grid_(grid),
    rowShift_(0), columnShift_(0),
    rows_(grid_.rows()), columns_(grid_.columns()),
    operation_(operation)
{
}

template<typename CellType>
Labyrinth2d::Grid<CellType> const& Labyrinth2d::SubGrid<CellType>::grid() const
{
    return grid_;
}

template<typename CellType>
size_t Labyrinth2d::SubGrid<CellType>::rowShift() const
{
    return rowShift_;
}

template<typename CellType>
size_t Labyrinth2d::SubGrid<CellType>::columnShift() const
{
    return columnShift_;
}

template<typename CellType>
size_t Labyrinth2d::SubGrid<CellType>::rows() const
{
    return rows_;
}

template<typename CellType>
size_t Labyrinth2d::SubGrid<CellType>::columns() const
{
    return columns_;
}

template<typename CellType>
size_t Labyrinth2d::SubGrid<CellType>::height() const
{
    return 2 * rows_ + 1;
}

template<typename CellType>
size_t Labyrinth2d::SubGrid<CellType>::width() const
{
    return 2 * columns_ + 1;
}

template<typename CellType>
CellType Labyrinth2d::SubGrid<CellType>::at(size_t i, size_t j) const
{
    if (i >= height() || j >= width())
        throw std::invalid_argument("Invalid indices");

    return grid_(2 * rowShift_ + i, 2 * columnShift_ + j);
}

template<typename CellType>
void Labyrinth2d::SubGrid<CellType>::change(size_t i, size_t j, CellType value) const
{
    if (i >= height() - 1 || j >= width() - 1 || !i || !j || i % 2 == j % 2)
        throw std::invalid_argument("Invalid indices");

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.change(2 * rowShift_ + i, 2 * columnShift_ + j, value);
}

template<typename CellType>
void Labyrinth2d::SubGrid<CellType>::toggle(size_t i, size_t j) const
{
    if (i >= height() - 1 || j >= width() - 1 || !i || !j || i % 2 == j % 2)
        throw std::invalid_argument("Invalid indices");

    CellType const value(!at(i, j));

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.toggle(2 * rowShift_ + i, 2 * columnShift_ + j);
}

template<typename CellType>
void Labyrinth2d::SubGrid<CellType>::set(size_t i, size_t j) const
{
    if (operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");

    change(i, j, true);
}

template<typename CellType>
void Labyrinth2d::SubGrid<CellType>::reset(size_t i, size_t j) const
{
    if (operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    change(i, j, false);
}

template<typename CellType>
typename Labyrinth2d::SubGrid<CellType>::Operation Labyrinth2d::SubGrid<CellType>::operation() const
{
    return operation_;
}

template<typename CellType>
CellType Labyrinth2d::SubGrid<CellType>::operator()(size_t i, size_t j) const
{
    return at(i, j);
}
