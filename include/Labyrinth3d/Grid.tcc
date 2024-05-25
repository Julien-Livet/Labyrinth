#include <cassert>
#include <stdexcept>

template<typename CellType>
Labyrinth3d::Grid<CellType>::Grid(Labyrinth const& labyrinth,
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
        for (size_t j(2); j < width_ - 1; j += 2)
            for (size_t k(2); k < depth_ - 1; k += 2)
                cells_[(k * height_ + i) * width_ + j] = true;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::rows() const
{
    return (height_ - 1) / 2;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::columns() const
{
    return (width_ - 1) / 2;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::floors() const
{
    return (depth_ - 1) / 2;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::height() const
{
    return height_;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::width() const
{
    return width_;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::depth() const
{
    return depth_;
}

template<typename CellType>
CellType Labyrinth3d::Grid<CellType>::at(size_t i, size_t j, size_t k) const
{
    assert((k * height_ + i) * width_ + j < cells_.size());

    return cells_[(k * height_ + i) * width_ + j];
}

template<typename CellType>
void Labyrinth3d::Grid<CellType>::change(size_t i, size_t j, size_t k, CellType value)
{
    assert((k * height_ + i) * width_ + j < cells_.size());

    if (cells_[(k * height_ + i) * width_ + j] != value)
    {
        cells_[(k * height_ + i) * width_ + j] = value;

        ++modificationCounter_;
    }
}

template<typename CellType>
void Labyrinth3d::Grid<CellType>::toggle(size_t i, size_t j, size_t k)
{
    assert((k * height_ + i) * width_ + j < cells_.size());

    cells_[(k * height_ + i) * width_ + j] = !cells_[(k * height_ + i) * width_ + j];

    ++modificationCounter_;
}

template<typename CellType>
void Labyrinth3d::Grid<CellType>::set(size_t i, size_t j, size_t k)
{
    change(i, j, k, true);
}

template<typename CellType>
void Labyrinth3d::Grid<CellType>::reset(size_t i, size_t j, size_t k)
{
    change(i, j, k, false);
}

template<typename CellType>
CellType Labyrinth3d::Grid<CellType>::operator()(size_t i, size_t j, size_t k) const
{
    return at(i, j, k);
}

template<typename CellType>
Labyrinth3d::Labyrinth const& Labyrinth3d::Grid<CellType>::labyrinth() const
{
    assert(labyrinth_ != nullptr);

    return *labyrinth_;
}

template<typename CellType>
size_t Labyrinth3d::Grid<CellType>::modificationCounter() const
{
    return modificationCounter_;
}

template<typename CellType>
Labyrinth3d::SubGrid<CellType>::SubGrid(SubGrid const& subGrid, size_t rowShift, size_t columnShift, size_t floorShift,
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

template<typename CellType>
Labyrinth3d::SubGrid<CellType>::SubGrid(Grid<CellType> &grid, Operation operation) : grid_(grid),
    rowShift_(0), columnShift_(0), floorShift_(0),
    rows_(grid_.rows()), columns_(grid_.columns()),
    floors_(grid_.floors()),
    operation_(operation)
{
}

template<typename CellType>
Labyrinth3d::Grid<CellType> const& Labyrinth3d::SubGrid<CellType>::grid() const
{
    return grid_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::rowShift() const
{
    return rowShift_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::columnShift() const
{
    return columnShift_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::floorShift() const
{
    return floorShift_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::rows() const
{
    return rows_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::columns() const
{
    return columns_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::floors() const
{
    return floors_;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::height() const
{
    return 2 * rows_ + 1;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::width() const
{
    return 2 * columns_ + 1;
}

template<typename CellType>
size_t Labyrinth3d::SubGrid<CellType>::depth() const
{
    return 2 * floors_ + 1;
}

template<typename CellType>
CellType Labyrinth3d::SubGrid<CellType>::at(size_t i, size_t j, size_t k) const
{
    if (i >= height() || j >= width() || k >= depth())
        throw std::invalid_argument("Invalid indices");

    return grid_(2 * rowShift_ + i, 2 * columnShift_ + j, 2 * floorShift_ + k);
}

template<typename CellType>
void Labyrinth3d::SubGrid<CellType>::change(size_t i, size_t j, size_t k, CellType value) const
{
    if (i >= height() - 1 || j >= width() - 1 || k >= depth() - 1 || !i || !j || !k || i % 2 + j % 2 + k % 2 == 3)
        throw std::invalid_argument("Invalid indices");

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.change(2 * rowShift_ + i, 2 * columnShift_ + j, 2 * floorShift_ + k, value);
}

template<typename CellType>
void Labyrinth3d::SubGrid<CellType>::toggle(size_t i, size_t j, size_t k) const
{
    if (i >= height() - 1 || j >= width() - 1 || k >= depth() - 1 || !i || !j || !k || i % 2 + j % 2 + k % 2 == 3)
        throw std::invalid_argument("Invalid indices");

    CellType const value(!at(i, j, k));

    if (value && operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");
    else if (!value && operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    grid_.toggle(2 * rowShift_ + i, 2 * columnShift_ + j, 2 * floorShift_ + k);
}

template<typename CellType>
void Labyrinth3d::SubGrid<CellType>::set(size_t i, size_t j, size_t k) const
{
    if (operation_ == Reset)
        throw std::invalid_argument("Set operation not allowed");

    change(i, j, k, true);
}

template<typename CellType>
void Labyrinth3d::SubGrid<CellType>::reset(size_t i, size_t j, size_t k) const
{
    if (operation_ == Set)
        throw std::invalid_argument("Reset operation not allowed");

    change(i, j, k, false);
}

template<typename CellType>
typename Labyrinth3d::SubGrid<CellType>::Operation Labyrinth3d::SubGrid<CellType>::operation() const
{
    return operation_;
}

template<typename CellType>
CellType Labyrinth3d::SubGrid<CellType>::operator()(size_t i, size_t j, size_t k) const
{
    return at(i, j, k);
}
