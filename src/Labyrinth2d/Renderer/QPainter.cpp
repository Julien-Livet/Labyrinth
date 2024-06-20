#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

#include <QPainterPath>

#include "Labyrinth2d/Renderer/QPainter.h"

Labyrinth2d::Renderer::QPainter::Texture::Texture(QBrush const& brush,
                                                  qreal linePercentage,
                                                  Qt::PenStyle penStyle,
                                                  Qt::PenCapStyle penCapStyle,
                                                  Qt::PenJoinStyle penJoinStyle) : type_(Brush), brush_(brush),
                                                                                   linePercentage_(linePercentage),
                                                                                   penStyle_(penStyle),
                                                                                   penCapStyle_(penCapStyle),
                                                                                   penJoinStyle_(penJoinStyle),
                                                                                   imageArrangement_(Pattern),
                                                                                   aspectRatioMode_(Qt::IgnoreAspectRatio),
                                                                                   transformationMode_(Qt::FastTransformation)
{
}

Labyrinth2d::Renderer::QPainter::Texture::Texture(QPixmap const& pixmap,
                                                  ImageArrangement imageArrangement,
                                                  Qt::AspectRatioMode aspectRatioMode,
                                                  Qt::TransformationMode transformationMode) : type_(Image),
                                                                                               linePercentage_(0.0),
                                                                                               penStyle_(Qt::SolidLine),
                                                                                               penCapStyle_(Qt::SquareCap),
                                                                                               penJoinStyle_(Qt::MiterJoin),
                                                                                               pixmap_(pixmap),
                                                                                               imageArrangement_(imageArrangement),
                                                                                               aspectRatioMode_(aspectRatioMode),
                                                                                               transformationMode_(transformationMode)
{
}

Labyrinth2d::Renderer::QPainter::Texture::Type Labyrinth2d::Renderer::QPainter::Texture::type() const
{
    return type_;
}

QBrush const& Labyrinth2d::Renderer::QPainter::Texture::brush() const
{
    return brush_;
}

qreal Labyrinth2d::Renderer::QPainter::Texture::linePercentage() const
{
    return linePercentage_;
}

Qt::PenStyle Labyrinth2d::Renderer::QPainter::Texture::penStyle() const
{
    return penStyle_;
}

Qt::PenCapStyle Labyrinth2d::Renderer::QPainter::Texture::penCapStyle() const
{
    return penCapStyle_;
}

Qt::PenJoinStyle Labyrinth2d::Renderer::QPainter::Texture::penJoinStyle() const
{
    return penJoinStyle_;
}

QPixmap const& Labyrinth2d::Renderer::QPainter::Texture::pixmap() const
{
    return pixmap_;
}

Labyrinth2d::Renderer::QPainter::Texture::ImageArrangement Labyrinth2d::Renderer::QPainter::Texture::imageArrangement() const
{
    return imageArrangement_;
}

Qt::AspectRatioMode Labyrinth2d::Renderer::QPainter::Texture::aspectRatioMode() const
{
    return aspectRatioMode_;
}

Qt::TransformationMode Labyrinth2d::Renderer::QPainter::Texture::transformationMode() const
{
    return transformationMode_;
}

void Labyrinth2d::Renderer::QPainter::Texture::change(QBrush const& color, qreal linePercentage, Qt::PenStyle penStyle,
                                                      Qt::PenCapStyle penCapStyle, Qt::PenJoinStyle penJoinStyle)
{
    type_ = Brush;
    brush_ = color;
    linePercentage_ = linePercentage;
    penStyle_ = penStyle;
    penCapStyle_ = penCapStyle;
    penJoinStyle_ = penJoinStyle;
    pixmap_ = QPixmap();
    imageArrangement_ = Pattern;
    aspectRatioMode_ = Qt::IgnoreAspectRatio;
    transformationMode_ = Qt::FastTransformation;
}

void Labyrinth2d::Renderer::QPainter::Texture::change(QPixmap const& pixmap, ImageArrangement imageArrangement,
                                                      Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode)
{
    type_ = Image;
    brush_ = QBrush();
    linePercentage_ = 0.0;
    penStyle_ = Qt::SolidLine;
    penCapStyle_ = Qt::SquareCap;
    penJoinStyle_ = Qt::MiterJoin;
    pixmap_ = pixmap;
    imageArrangement_ = imageArrangement;
    aspectRatioMode_ = aspectRatioMode;
    transformationMode_ = transformationMode;
}

Labyrinth2d::Renderer::QPainter::PlayerRenderer::PlayerRenderer(bool displayPlayer_, bool displayTrace_,
                                                                bool displayStart_, bool displayFinish_) : playerTexture(Qt::darkGray, 75.0, Qt::SolidLine, Qt::RoundCap),
                                                                                                           traceTexture(Qt::gray, 25.0, Qt::SolidLine, Qt::RoundCap),
                                                                                                           startTexture(QColor(0, 255, 0, 128)),
                                                                                                           finishTexture(QColor(255, 0, 0, 128)),
                                                                                                           displayPlayer(displayPlayer_), displayTrace(displayTrace_),
                                                                                                           displayStart(displayStart_), displayFinish(displayFinish_)
{
}

Labyrinth2d::Renderer::QPainter::PlayerRenderer::PlayerRenderer(Texture const& playerTexture_,
                                                                Texture const& traceTexture_,
                                                                Texture const& startTexture_,
                                                                Texture const& exitTexture_,
                                                                bool displayPlayer_, bool displayTrace_,
                                                                bool displayStart_, bool displayFinish_) : playerTexture(playerTexture_), traceTexture(traceTexture_),
                                                                                                           startTexture(startTexture_), finishTexture(exitTexture_),
                                                                                                           displayPlayer(displayPlayer_), displayTrace(displayTrace_),
                                                                                                           displayStart(displayStart_), displayFinish(displayFinish_)
{
}

Labyrinth2d::Renderer::QPainter::QPainter(Labyrinth const& labyrinth_) : displayPlayers(true),
                                                                         labyrinth(labyrinth_),
                                                                         wallsTexture_(QBrush(Qt::black)),
                                                                         waysTexture_(QBrush(Qt::white)),
                                                                         backgroundTexture_(QBrush(Qt::white)),
                                                                         displayWalls_(true),
                                                                         displayWays_(true),
                                                                         displayBackground_(true),
                                                                         wallsSize_(QSize(20, 20)), waysSize_(QSize(20, 20)),
                                                                         gridModificationCounter_(labyrinth.grid().modificationCounter()),
                                                                         margins_(10.0, 10.0, 10.0, 10.0),
                                                                         paintingUpdateTypes_(UpdateWalls | UpdateWays | UpdateBackground)
{
}

void Labyrinth2d::Renderer::QPainter::cleanPlayerRenderers()
{
    for (auto it(playerRenderers.begin()); it != playerRenderers.end(); ++it)
        if (std::find(labyrinth.playerIds().begin(), labyrinth.playerIds().end(), it->first) == labyrinth.playerIds().end())
            playerRenderers.erase(it->first);
}

void Labyrinth2d::Renderer::QPainter::renderBackground(::QPainter* painter, QRegion const& region, QPoint const& targetOffset)
{
    assert(painter != 0);

    painter->save();

    painter->translate(targetOffset);

    QRegion const adjustedRegion(region.intersected(rect()));

    painter->setClipRegion(adjustedRegion, Qt::IntersectClip);

    if (displayBackground_)
    {
        auto const& texture(backgroundTexture_);

        if (texture.type() == Texture::Brush)
        {
            if (texture.brush().style() != Qt::LinearGradientPattern
                && texture.brush().style() != Qt::RadialGradientPattern
                && texture.brush().style() != Qt::ConicalGradientPattern
                && texture.brush().style() != Qt::TexturePattern
                && !texture.brush().color().alpha())
                return;

            QPainterPath painterPath;
            painterPath.addRegion(adjustedRegion);
            painter->fillPath(painterPath, texture.brush());
        }
        else if (texture.type() == Texture::Image)
        {
            if (texture.imageArrangement() == Texture::Mosaic)
                painter->fillRect(rect(), texture.pixmap()); // Or with method in displayCells?
            else if (texture.imageArrangement() == Texture::Pattern
                     || texture.imageArrangement() == Texture::Background)
            {
                QPixmap const pixmap(texture.pixmap().scaled(size(), texture.aspectRatioMode(), Qt::SmoothTransformation));

                std::vector<::QPainter::PixmapFragment> pixmapFragments;

                QPoint const pixmapCenter(QRect(QPoint(), pixmap.size()).center());

                for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
                    pixmapFragments.push_back(::QPainter::PixmapFragment::create(it->center() - pixmapCenter, *it));

                painter->drawPixmapFragments(pixmapFragments.data(),
                                             pixmapFragments.size(),
                                             pixmap,
                                             QFlags<::QPainter::PixmapFragmentHint>(pixmap.hasAlpha() ? ::QPainter::OpaqueHint : 0));
            }
            else
                assert(0);
        }
        else
            assert(0);
    }


    painter->restore();
}

void Labyrinth2d::Renderer::QPainter::renderBackground(::QPainter* painter, QPoint const& targetOffset)
{
    renderBackground(painter, QRegion(rect()), targetOffset);
}

void Labyrinth2d::Renderer::QPainter::renderWalls(::QPainter* painter,  QRegion const& region, QPoint const& targetOffset)
{
    assert(painter != 0);

    auto const& grid(labyrinth.grid());
    size_t const height(grid.height());
    size_t const width(grid.width());

    painter->save();

    painter->translate(targetOffset);

    QRegion const adjustedRegion(region.intersected(rect()));

    painter->setClipRegion(adjustedRegion, Qt::IntersectClip);

    if (displayWalls_)
    {
        auto const& texture(wallsTexture_);

        if (texture.type() == Texture::Brush)
        {
            if (texture.brush().style() != Qt::LinearGradientPattern
                && texture.brush().style() != Qt::RadialGradientPattern
                && texture.brush().style() != Qt::ConicalGradientPattern
                && texture.brush().style() != Qt::TexturePattern
                && !texture.brush().color().alpha())
                return;

            if (0.0 <= texture.linePercentage() && texture.linePercentage() <= 100.0)
            {
                std::vector<QLineF> lines;
                std::vector<QPointF> points;

                for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
                {
                    QRect indices(computeIndices(*it));

                    indices.setTop(indices.top() - indices.top() % 2);
                    indices.setBottom(indices.bottom() + (indices.bottom() + 1) % 2);
                    indices.setLeft(indices.left() - indices.left() % 2);
                    indices.setRight(indices.right() + (indices.right() + 1) % 2);

                    for (size_t i(static_cast<size_t>(indices.top())); i < static_cast<size_t>(indices.bottom()); i += 2)
                    {
                        size_t j1(static_cast<size_t>(indices.left()));

                        for (size_t j2(j1 + 1); j2 < static_cast<size_t>(indices.right()); j2 += 2)
                        {
                            if (!grid(i, j2))
                            {
                                addLine(lines, i, j1, i, j2 - 1);
                                j1 = j2 + 1;
                            }
                        }

                        addLine(lines, i, j1, i, static_cast<size_t>(indices.right()) - 1);
                    }

                    for (size_t j(static_cast<size_t>(indices.left())); j < static_cast<size_t>(indices.right()); j += 2)
                    {
                        size_t i1(static_cast<size_t>(indices.top()));

                        for (size_t i2(i1 + 1); i2 < static_cast<size_t>(indices.bottom()); i2 += 2)
                        {
                            if (!grid(i2, j))
                            {
                                addLine(lines, i1, j, i2 - 1, j);
                                i1 = i2 + 1;
                            }
                        }

                        addLine(lines, i1, j, static_cast<size_t>(indices.bottom()) - 1, j);
                    }

                    int const evenColumnWidth(wallsSize_.width());
                    int const oddColumnWidth(waysSize_.width());
                    int const evenRowHeight(wallsSize_.height());
                    int const oddRowHeight(waysSize_.height());

                    for (size_t i(static_cast<size_t>(indices.top())); i < static_cast<size_t>(indices.bottom()); i += 2)
                    {
                        for (size_t j(static_cast<size_t>(indices.left())); j < static_cast<size_t>(indices.right()); j += 2)
                        {
                            size_t count(0);

                            if (i && !grid(i - 1, j))
                                ++count;
                            if (i < height - 1 && !grid(i + 1, j))
                                ++count;
                            if (j && !grid(i, j - 1))
                                ++count;
                            if (j < width - 1 && !grid(i, j + 1))
                                ++count;

                            if (count == 4)
                                points.push_back(QPointF(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                                         + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                                         static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                                         + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0));
                        }
                    }
                }

                painter->setPen(QPen(texture.brush(), texture.linePercentage() / 100.0
                                                      * static_cast<qreal>(std::min(wallsSize_.width(), wallsSize_.height())),
                                     texture.penStyle(), texture.penCapStyle(), texture.penJoinStyle()));

                painter->drawLines(lines.data(), lines.size());
                painter->drawPoints(points.data(), points.size());
            }
            else
            {
                std::vector<std::pair<size_t, size_t> > cells;

                for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
                {
                    QRect const indices(computeIndices(*it));

                    for (size_t i(static_cast<size_t>(indices.top())); i < static_cast<size_t>(indices.bottom()); ++i)
                        for (size_t j(static_cast<size_t>(indices.left())); j < static_cast<size_t>(indices.right()); ++j)
                            if (grid(i, j))
                                cells.push_back(std::make_pair(i, j));
                }

                displayCells(painter, texture, cells);
            }
        }
        else if (texture.type() == Texture::Image)
        {
            std::vector<std::pair<size_t, size_t> > cells;

            for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
            {
                QRect const indices(computeIndices(*it));

                for (size_t i(static_cast<size_t>(indices.top())); i < static_cast<size_t>(indices.bottom()); ++i)
                    for (size_t j(static_cast<size_t>(indices.left())); j < static_cast<size_t>(indices.right()); ++j)
                        if (grid(i, j))
                            cells.push_back(std::make_pair(i, j));
            }

            displayCells(painter, texture, cells);
        }
        else
            assert(0);
    }

    painter->restore();
}

void Labyrinth2d::Renderer::QPainter::renderWalls(::QPainter* painter, QPoint const& targetOffset)
{
    renderWalls(painter, QRegion(rect()), targetOffset);
}

void Labyrinth2d::Renderer::QPainter::renderWays(::QPainter* painter,  QRegion const& region, QPoint const& targetOffset)
{
    assert(painter != 0);

    auto const& grid(labyrinth.grid());
    size_t const height(grid.height());
    size_t const width(grid.width());

    painter->save();

    painter->translate(targetOffset);

    QRegion const adjustedRegion(region.intersected(rect()));

    painter->setClipRegion(adjustedRegion, Qt::IntersectClip);

    if (displayWays_)
    {
        auto const& texture(waysTexture_);

        if (texture.type() == Texture::Brush)
        {
            if (texture.brush().style() != Qt::LinearGradientPattern
                && texture.brush().style() != Qt::RadialGradientPattern
                && texture.brush().style() != Qt::ConicalGradientPattern
                && texture.brush().style() != Qt::TexturePattern
                && !texture.brush().color().alpha())
                return;

            if (0.0 <= texture.linePercentage() && texture.linePercentage() <= 100.0)
            {
                std::vector<QLineF> lines;
                std::vector<QPointF> points;

                for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
                {
                    QRect indices(computeIndices(*it));

                    indices.setTop(indices.top() - indices.top() % 2);
                    indices.setBottom(indices.bottom() + (indices.bottom() + 1) % 2);
                    indices.setLeft(indices.left() - indices.left() % 2);
                    indices.setRight(indices.right() + (indices.right() + 1) % 2);

                    size_t const iMin(indices.top() ? indices.top() - 2 : 0);
                    size_t const iMax(static_cast<size_t>(indices.bottom()) < height ? indices.bottom() + 2 : height);

                    size_t const jMin(indices.left() ? indices.left() - 2 : 0);
                    size_t const jMax(static_cast<size_t>(indices.right()) < width ? indices.right() + 2 : width);

                    for (size_t i(static_cast<size_t>(indices.top()) + 1); i < static_cast<size_t>(indices.bottom()) - 1; i += 2)
                    {
                        size_t j1(jMin + 1);

                        for (size_t j2(j1 + 1); j2 < jMax; j2 += 2)
                        {
                            if (grid(i, j2))
                            {
                                addLine(lines, i, j1, i, j2 - 1);
                                j1 = j2 + 1;
                            }
                        }

                        if (!grid(i, jMax - 1))
                            addLine(lines, i, j1, i, jMax - 2);
                    }

                    for (size_t j(static_cast<size_t>(indices.left()) + 1); j < static_cast<size_t>(indices.right()) - 1; j += 2)
                    {
                        size_t i1(iMin + 1);

                        for (size_t i2(i1 + 1); i2 < iMax; i2 += 2)
                        {
                            if (grid(i2, j))
                            {
                                addLine(lines, i1, j, i2 - 1, j);
                                i1 = i2 + 1;
                            }
                        }

                        if (!grid(iMax - 1, j))
                            addLine(lines, i1, j, iMax - 2, j);
                    }

                    int const evenColumnWidth(wallsSize_.width());
                    int const oddColumnWidth(waysSize_.width());
                    int const evenRowHeight(wallsSize_.height());
                    int const oddRowHeight(waysSize_.height());

                    for (size_t i(static_cast<size_t>(indices.top()) + 1); i < static_cast<size_t>(indices.bottom()) - 1; i += 2)
                    {
                        for (size_t j(static_cast<size_t>(indices.left()) + 1); j < static_cast<size_t>(indices.right()) - 1; j += 2)
                        {
                            size_t count(0);

                            if (grid(i - 1, j))
                                ++count;
                            if (grid(i + 1, j))
                                ++count;
                            if (grid(i, j - 1))
                                ++count;
                            if (grid(i, j + 1))
                                ++count;

                            if (count == 4)
                                points.push_back(QPointF(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                                         + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                                         static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                                         + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0));
                        }
                    }
                }

                painter->setPen(QPen(texture.brush(), texture.linePercentage() / 100.0
                                                          * static_cast<qreal>(std::min(waysSize_.width(), waysSize_.height())),
                                     texture.penStyle(), texture.penCapStyle(), texture.penJoinStyle()));

                painter->drawLines(lines.data(), lines.size());
                painter->drawPoints(points.data(), points.size());
            }
            else
            {
                std::vector<std::pair<size_t, size_t> > cells;

                for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
                {
                    QRect indices(computeIndices(*it));

                    indices.setTop(std::max(1, indices.top()));
                    indices.setBottom(std::min(static_cast<int>(height - 1), indices.bottom()));
                    indices.setLeft(std::max(1, indices.left()));
                    indices.setRight(std::min(static_cast<int>(width - 1), indices.right()));

                    for (size_t i(static_cast<size_t>(indices.top())); i < static_cast<size_t>(indices.bottom()); ++i)
                        for (size_t j(static_cast<size_t>(indices.left())); j < static_cast<size_t>(indices.right()); ++j)
                            if (!grid(i, j))
                                cells.push_back(std::make_pair(i, j));
                }

                displayCells(painter, texture, cells);
            }
        }
        else if (texture.type() == Texture::Image)
        {
            std::vector<std::pair<size_t, size_t> > cells;

            for (auto it{adjustedRegion.begin()}; it != adjustedRegion.end(); ++it)
            {
                QRect indices(computeIndices(*it));

                indices.setTop(std::max(1, indices.top()));
                indices.setBottom(std::min(static_cast<int>(height - 1), indices.bottom()));
                indices.setLeft(std::max(1, indices.left()));
                indices.setRight(std::min(static_cast<int>(width - 1), indices.right()));

                for (size_t i(static_cast<size_t>(indices.top())); i < static_cast<size_t>(indices.bottom()); ++i)
                    for (size_t j(static_cast<size_t>(indices.left())); j < static_cast<size_t>(indices.right()); ++j)
                        if (!grid(i, j))
                            cells.push_back(std::make_pair(i, j));
            }

            displayCells(painter, texture, cells);
        }
        else
            assert(0);
    }

    painter->restore();
}

void Labyrinth2d::Renderer::QPainter::renderWays(::QPainter* painter, QPoint const& targetOffset)
{
    renderWays(painter, QRegion(rect()), targetOffset);
}

void Labyrinth2d::Renderer::QPainter::renderPlayer(size_t playerId, ::QPainter* painter,
                                                   QRegion const& region, QPoint const& targetOffset)
{
    assert(painter != 0);

    painter->save();

    painter->translate(targetOffset);

    QRegion const adjustedRegion(region.intersected(rect()));

    painter->setClipRegion(adjustedRegion, Qt::IntersectClip);

    int const evenColumnWidth(wallsSize_.width());
    int const oddColumnWidth(waysSize_.width());
    int const evenRowHeight(wallsSize_.height());
    int const oddRowHeight(waysSize_.height());

    Player const& player(labyrinth.player(playerId));
    auto const& playerRenderer(playerRenderers[playerId]);

    if (playerRenderer.displayPlayer)
    {
        if (playerRenderer.displayStart)
            displayCells(painter, playerRenderer.startTexture,
                         std::vector<std::pair<size_t, size_t> >{std::make_pair(player.startI(), player.startJ())});

        if (playerRenderer.displayFinish)
            for (size_t i{0}; i < player.finishI().size(); ++i)
                displayCells(painter, playerRenderer.finishTexture,
                             std::vector<std::pair<size_t, size_t> >{std::make_pair(player.finishI()[i], player.finishJ()[i])});

        if (playerRenderer.displayTrace)
        {
            auto const& traceTexture(playerRenderer.traceTexture);
            auto const& traceIntersections(player.traceIntersections());

            bool displayingCells(false);

            if (traceTexture.type() == Texture::Brush)
            {
                if (traceTexture.brush().style() != Qt::LinearGradientPattern
                    && traceTexture.brush().style() != Qt::RadialGradientPattern
                    && traceTexture.brush().style() != Qt::ConicalGradientPattern
                    && traceTexture.brush().style() != Qt::TexturePattern
                    && !traceTexture.brush().color().alpha())
                    return;

                if (0.0 <= traceTexture.linePercentage() && traceTexture.linePercentage() <= 100.0)
                {
                    QPainterPath painterPath;

                    if (!traceIntersections.empty())
                    {
                        {
                            size_t const& i(traceIntersections.front().first);
                            size_t const& j(traceIntersections.front().second);

                            painterPath.moveTo(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                               + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                               static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                               + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0);
                        }

                        for (size_t k(1); k < traceIntersections.size(); ++k)
                        {
                            size_t const& i(traceIntersections[k].first);
                            size_t const& j(traceIntersections[k].second);

                            painterPath.lineTo(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                               + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                               static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                               + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0);
                        }

                        {
                            size_t const i(player.i());
                            size_t const j(player.j());

                            painterPath.lineTo(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                               + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                               static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                               + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0);
                        }
                    }

                    painter->setPen(QPen(traceTexture.brush(), traceTexture.linePercentage() / 100.0
                                                               * static_cast<qreal>(std::min(waysSize_.width(), waysSize_.height())),
                                         traceTexture.penStyle(), traceTexture.penCapStyle(), traceTexture.penJoinStyle()));

                    painter->drawPath(painterPath);
                }
                else
                    displayingCells = true;
            }
            else if (traceTexture.type() == Texture::Image)
                displayingCells = true;
            else
                assert(0);

            if (displayingCells)
            {
                std::vector<std::pair<size_t, size_t> > cells;

                if (!traceIntersections.empty())
                {
                    cells.push_back(std::make_pair(traceIntersections.front().first,
                                                   traceIntersections.front().second));

                    for (size_t k(1); k < traceIntersections.size(); ++k)
                    {
                        if (traceIntersections[k].first == traceIntersections[k - 1].first)
                        {
                            if (traceIntersections[k].second < traceIntersections[k - 1].second)
                                for (size_t j(traceIntersections[k - 1].second - 1); j > traceIntersections[k].second; --j)
                                    cells.push_back(std::make_pair(traceIntersections[k].first, j));
                            else if (traceIntersections[k].second > traceIntersections[k - 1].second)
                                for (size_t j(traceIntersections[k - 1].second + 1); j < traceIntersections[k].second; ++j)
                                    cells.push_back(std::make_pair(traceIntersections[k].first, j));
                            else
                                assert(0);
                        }
                        else if (traceIntersections[k].second == traceIntersections[k - 1].second)
                        {
                            if (traceIntersections[k].first < traceIntersections[k - 1].first)
                                for (size_t i(traceIntersections[k - 1].first - 1); i > traceIntersections[k].first; --i)
                                    cells.push_back(std::make_pair(i, traceIntersections[k].second));
                            else if (traceIntersections[k].first > traceIntersections[k - 1].second)
                                for (size_t i(traceIntersections[k - 1].first + 1); i < traceIntersections[k].first; ++i)
                                    cells.push_back(std::make_pair(i, traceIntersections[k].second));
                            else
                                assert(0);
                        }
                        else
                            assert(0);
                    }
                }

                displayCells(painter, traceTexture, cells);
            }
        }

        displayCells(painter, playerRenderer.playerTexture,
                     std::vector<std::pair<size_t, size_t> >{std::make_pair(player.i(), player.j())});
    }

    painter->restore();
}

void Labyrinth2d::Renderer::QPainter::renderPlayer(size_t playerId, ::QPainter* painter, QPoint const& targetOffset)
{
    renderPlayer(playerId, painter, QRegion(rect()), targetOffset);
}

void Labyrinth2d::Renderer::QPainter::renderPlayers(::QPainter* painter, QRegion const& region, QPoint const& targetOffset)
{
    if (displayPlayers)
        for (auto id : labyrinth.playerIds())
            renderPlayer(id, painter, region, targetOffset);
}

void Labyrinth2d::Renderer::QPainter::renderPlayers(::QPainter* painter,  QPoint const& targetOffset)
{
    renderPlayers(painter, QRegion(rect()), targetOffset);
}

void Labyrinth2d::Renderer::QPainter::render(::QPainter* painter,  QRegion const& region, QPoint const& targetOffset)
{
    QRegion const adjustedRegion(region.intersected(rect()));

    bool const updateRegion{region_ != adjustedRegion};

    if (gridModificationCounter_ != labyrinth.grid().modificationCounter()
        || updateRegion)
    {
        paintingUpdateTypes_ |= UpdateWalls | UpdateWays | UpdateBackground;

        if (updateRegion)
        {
            region_ = adjustedRegion;
            corner_ = adjustedRegion.boundingRect().topLeft();
        }

        gridModificationCounter_ = labyrinth.grid().modificationCounter();
    }

    if (paintingUpdateTypes_ & UpdateBackground)
    {
        paintingUpdateTypes_ &= ~UpdateBackground;

        if (!region_.isEmpty())
        {
            if (displayBackground_)
            {
                backgroundPixmap_ = QPixmap(region_.boundingRect().size());
                backgroundPixmap_.fill(Qt::transparent);

                ::QPainter painter(&backgroundPixmap_);

                renderBackground(&painter,
                                 region_,
                                 -region_.boundingRect().topLeft());
            }
            else
                backgroundPixmap_ = QPixmap();
        }
        else
            backgroundPixmap_ = QPixmap();
    }

    if (paintingUpdateTypes_ & UpdateWalls)
    {
        paintingUpdateTypes_ &= ~UpdateWalls;

        if (!region_.isEmpty())
        {
            if (displayWalls_)
            {
                wallsPixmap_ = QPixmap(region_.boundingRect().size());
                wallsPixmap_.fill(Qt::transparent);

                ::QPainter painter(&wallsPixmap_);

                renderWalls(&painter,
                            region_,
                            -region_.boundingRect().topLeft());
            }
            else
                wallsPixmap_ = QPixmap();
        }
        else
            wallsPixmap_ = QPixmap();
    }

    if (paintingUpdateTypes_ & UpdateWays)
    {
        paintingUpdateTypes_ &= ~UpdateWays;

        if (!region_.isEmpty())
        {
            if (displayWays_)
            {
                waysPixmap_ = QPixmap(region_.boundingRect().size());
                waysPixmap_.fill(Qt::transparent);

                ::QPainter painter(&waysPixmap_);

                renderWays(&painter,
                           region_,
                           -region_.boundingRect().topLeft());
            }
            else
                waysPixmap_ = QPixmap();
        }
        else
            waysPixmap_ = QPixmap();
    }

    assert(painter != 0);

    painter->save();

    painter->translate(targetOffset + adjustedRegion.boundingRect().topLeft());

    QRegion const r(adjustedRegion.translated(-adjustedRegion.boundingRect().topLeft()));

    painter->setClipRegion(r, Qt::IntersectClip);

    if (displayBackground_ && !backgroundPixmap_.isNull())
        painter->drawPixmap(QPoint(), backgroundPixmap_,
                            r.translated(adjustedRegion.boundingRect().topLeft() - corner_).boundingRect());
    if (displayWalls_ && !wallsPixmap_.isNull())
        painter->drawPixmap(QPoint(), wallsPixmap_,
                            r.translated(adjustedRegion.boundingRect().topLeft() - corner_).boundingRect());
    if (displayWays_ && !waysPixmap_.isNull())
        painter->drawPixmap(QPoint(), waysPixmap_,
                            r.translated(adjustedRegion.boundingRect().topLeft() - corner_).boundingRect());

    painter->restore();

    // TODO: Improve players rendering like background, walls and ways...

    renderPlayers(painter, adjustedRegion, targetOffset);
}

void Labyrinth2d::Renderer::QPainter::render(::QPainter* painter,  QPoint const& targetOffset)
{
    render(painter, QRegion(rect()), targetOffset);
}

void Labyrinth2d::Renderer::QPainter::operator()(::QPainter* painter,  QRegion const& region, QPoint const& targetOffset)
{
    render(painter, region, targetOffset);
}

void Labyrinth2d::Renderer::QPainter::operator()(::QPainter* painter,  QPoint const& targetOffset)
{
    render(painter, QRegion(rect()), targetOffset);
}

QSize const& Labyrinth2d::Renderer::QPainter::wallsSize() const
{
    return wallsSize_;
}

QSize const& Labyrinth2d::Renderer::QPainter::waysSize() const
{
    return waysSize_;
}

void Labyrinth2d::Renderer::QPainter::changeWallsSize(QSize const& wallsSize)
{
    if (wallsSize.isEmpty())
        throw std::invalid_argument("Invalid walls size");

    if (wallsSize_ != wallsSize)
    {
        wallsSize_ = wallsSize;
        paintingUpdateTypes_ |= UpdateWalls | UpdateWays | UpdateBackground;
    }
}

void Labyrinth2d::Renderer::QPainter::changeWaysSize(QSize const& waysSize)
{
    if (waysSize.isEmpty())
        throw std::invalid_argument("Invalid ways size");

    if (waysSize_ != waysSize)
    {
        waysSize_ = waysSize;
        paintingUpdateTypes_ |= UpdateWalls | UpdateWays | UpdateBackground;
    }
}

QSize Labyrinth2d::Renderer::QPainter::size() const
{
    return QSize(wallsSize_.width() * static_cast<int>((labyrinth.grid().columns() + 1))
                 + waysSize_.width() * static_cast<int>(labyrinth.grid().columns()),
                 wallsSize_.height() * static_cast<int>((labyrinth.grid().rows() + 1))
                 + waysSize_.height() * static_cast<int>(labyrinth.grid().rows()));
}

QRect Labyrinth2d::Renderer::QPainter::rect() const
{
    return QRect(QPoint(), size());
}

void Labyrinth2d::Renderer::QPainter::addLine(std::vector<QLineF>& lines,
                                              size_t i1, size_t j1, size_t i2, size_t j2) const
{
    if (i1 == i2 && j1 == j2)
        return;

    int const evenColumnWidth(wallsSize_.width());
    int const oddColumnWidth(waysSize_.width());
    int const evenRowHeight(wallsSize_.height());
    int const oddRowHeight(waysSize_.height());

    lines.push_back(QLineF(QPointF(static_cast<qreal>((j1 + 1) / 2 * evenColumnWidth + j1 / 2 * oddColumnWidth)
                                   + static_cast<qreal>(j1 % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                   static_cast<qreal>((i1 + 1) / 2 * evenRowHeight + i1 / 2 * oddRowHeight)
                                   + static_cast<qreal>(i1 % 2 ? oddRowHeight : evenRowHeight) / 2.0),
                           QPointF(static_cast<qreal>((j2 + 1) / 2 * evenColumnWidth + j2 / 2 * oddColumnWidth)
                                   + static_cast<qreal>(j2 % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                   static_cast<qreal>((i2 + 1) / 2 * evenRowHeight + i2 / 2 * oddRowHeight)
                                   + static_cast<qreal>(i2 % 2 ? oddRowHeight : evenRowHeight) / 2.0)));
}

void Labyrinth2d::Renderer::QPainter::displayCells(::QPainter* painter, Texture const& texture,
                                                   std::vector<std::pair<size_t, size_t> > const& cells) const
{
    assert(painter != 0);

    int const evenColumnWidth(wallsSize_.width());
    int const oddColumnWidth(waysSize_.width());
    int const evenRowHeight(wallsSize_.height());
    int const oddRowHeight(waysSize_.height());

    if (texture.type() == Texture::Brush)
    {
        if (texture.brush().style() != Qt::LinearGradientPattern
            && texture.brush().style() != Qt::RadialGradientPattern
            && texture.brush().style() != Qt::ConicalGradientPattern
            && texture.brush().style() != Qt::TexturePattern
            && !texture.brush().color().alpha())
            return;

        if (0.0 <= texture.linePercentage() && texture.linePercentage() <= 100.0)
        {
            painter->setPen(QPen(texture.brush(), texture.linePercentage() / 100.0
                                                  * static_cast<qreal>(std::min(waysSize_.width(), waysSize_.height())),
                                 texture.penStyle(), texture.penCapStyle(), texture.penJoinStyle()));

            std::vector<QPointF> points;

            for (auto const& c : cells)
            {
                size_t const& i(c.first);
                size_t const& j(c.second);

                points.push_back(QPointF(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                         + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                         static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                         + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0));
            }

            painter->drawPoints(points.data(), points.size());
        }
        else
        {
            QRegion region;

            for (auto const& c : cells)
            {
                size_t const& i(c.first);
                size_t const& j(c.second);

                QSize size;

                if (i % 2)
                    size.rheight() = oddRowHeight;
                else
                    size.rheight() = evenRowHeight;

                if (j % 2)
                    size.rwidth() = oddColumnWidth;
                else
                    size.rwidth() = evenColumnWidth;

                region += QRect(QPoint(static_cast<int>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth),
                                       static_cast<int>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)),
                                size);
            }

            QPainterPath painterPath;
            painterPath.addRegion(region);
            painter->fillPath(painterPath, texture.brush());
        }
    }
    else if (texture.type() == Texture::Image)
    {
        if (texture.imageArrangement() == Texture::Pattern)
        {
            std::map<std::pair<bool, bool>, QPixmap> pixmaps;
            std::map<std::pair<bool, bool>, std::vector<::QPainter::PixmapFragment> > pixmapsFragments;

            for (int i(0); i < 2; ++i)
            {
                for (int j(0); j < 2; ++j)
                {
                    pixmaps[std::make_pair(static_cast<bool>(i % 2),
                                           static_cast<bool>(j % 2))] = texture.pixmap()
                                                                        .scaled(QSize(j * oddColumnWidth + (1 - j) * evenColumnWidth,
                                                                                      i * oddRowHeight + (1 - i) * evenRowHeight),
                                                                                texture.aspectRatioMode(), Qt::SmoothTransformation);
                    pixmapsFragments[std::make_pair(static_cast<bool>(i % 2),
                                                    static_cast<bool>(j % 2))] = std::vector<::QPainter::PixmapFragment>{};
                }
            }

            assert(pixmaps.size() == pixmapsFragments.size());

            for (auto const& c : cells)
            {
                size_t const& i(c.first);
                size_t const& j(c.second);
                QPointF const pos(static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth)
                                  + static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth) / 2.0,
                                  static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)
                                  + static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight) / 2.0);
                auto const p(std::make_pair(static_cast<bool>(i % 2), static_cast<bool>(j % 2)));

                pixmapsFragments[p].push_back(::QPainter::PixmapFragment::create(pos, QRectF(QPointF(), pixmaps[p].size())));
            }

            assert(pixmaps.size() == pixmapsFragments.size());

            auto it1(pixmaps.cbegin());
            auto it2(pixmapsFragments.cbegin());

            for (size_t i(0); i < pixmaps.size(); ++i)
            {
                painter->drawPixmapFragments(it2->second.data(),
                                             it2->second.size(),
                                             it1->second,
                                             QFlags<::QPainter::PixmapFragmentHint>(it1->second.hasAlpha() ? ::QPainter::OpaqueHint : 0));

                ++it1;
                ++it2;
            }
        }
        else if (texture.imageArrangement() == Texture::Mosaic)
        {
            QRegion region;

            for (auto const& c : cells)
            {
                size_t const& i(c.first);
                size_t const& j(c.second);

                QSize size;

                if (i % 2)
                    size.rheight() = oddRowHeight;
                else
                    size.rheight() = evenRowHeight;

                if (j % 2)
                    size.rwidth() = oddColumnWidth;
                else
                    size.rwidth() = evenColumnWidth;

                region += QRect(QPoint(static_cast<int>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth),
                                       static_cast<int>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)),
                                size);
            }

            QPainterPath painterPath;
            painterPath.addRegion(region);
            painter->fillPath(painterPath, texture.pixmap());
        }
        else if (texture.imageArrangement() == Texture::Background)
        {
            QPixmap pixmap(texture.pixmap().scaled(size(), texture.aspectRatioMode(), Qt::SmoothTransformation));

            QImage image{size(), QImage::Format_ARGB32};
            ::QPainter imagePainter{&image};

            for (int y{0}; y < size().height(); y += pixmap.height())
            {
                for (int x{0}; x < size().width(); x += pixmap.width())
                    imagePainter.drawPixmap(x, y, pixmap);
            }

            imagePainter.end();

            pixmap.convertFromImage(image);

            std::vector<::QPainter::PixmapFragment> pixmapFragments;

            for (auto const& c : cells)
            {
                auto const& i{c.first};
                auto const& j{c.second};
                QPointF const pos{static_cast<qreal>((j + 1) / 2 * evenColumnWidth + j / 2 * oddColumnWidth),
                                  static_cast<qreal>((i + 1) / 2 * evenRowHeight + i / 2 * oddRowHeight)};
                QSizeF const s{static_cast<qreal>(j % 2 ? oddColumnWidth : evenColumnWidth),
                               static_cast<qreal>(i % 2 ? oddRowHeight : evenRowHeight)};

                pixmapFragments.push_back(::QPainter::PixmapFragment::create(QPointF{pos.x() + s.width() / 2,
                                                                                     pos.y() + s.height() / 2},
                                                                             QRectF{QPointF{pos.x(),
                                                                                            pos.y()}, s}));
            }

            painter->drawPixmapFragments(pixmapFragments.data(),
                                         pixmapFragments.size(),
                                         pixmap,
                                         QFlags<::QPainter::PixmapFragmentHint>(pixmap.hasAlpha() ? ::QPainter::OpaqueHint : 0));
        }
        else
            assert(0);
    }
    else
        assert(0);
}

QRect Labyrinth2d::Renderer::QPainter::computeIndices(QRect const& rect) const
{
    assert(this->rect().contains(rect));

    int const evenColumnWidth(wallsSize_.width());
    int const oddColumnWidth(waysSize_.width());
    int const evenRowHeight(wallsSize_.height());
    int const oddRowHeight(waysSize_.height());

    QSize const size(evenColumnWidth + oddColumnWidth, evenRowHeight + oddRowHeight);

    QRect r;

    r.moveTop((rect.top() / size.height()) * 2);

    if (rect.top() - (r.top() / 2) * size.height() > evenRowHeight)
        r.moveTop(r.top() + 1);

    r.setBottom((rect.bottom() / size.height()) * 2 + 1);

    if (rect.bottom() - (r.bottom() / 2) * size.height() > evenRowHeight)
        r.setBottom(r.bottom() + 1);

    r.moveLeft((rect.left() / size.width()) * 2);

    if (rect.left() - (r.left() / 2) * size.width() > evenColumnWidth)
        r.moveLeft(r.left() + 1);

    r.setRight((rect.right() / size.width()) * 2 + 1);

    if (rect.right() - (r.right() / 2) * size.width() > evenColumnWidth)
        r.setRight(r.right() + 1);

    assert(r.top() >= 0 && r.bottom() <= static_cast<int>(labyrinth.grid().height())
           && r.left() >= 0 && r.right() <= static_cast<int>(labyrinth.grid().width()));

    return r;
}

QMarginsF const& Labyrinth2d::Renderer::QPainter::margins() const
{
    return margins_;
}

void Labyrinth2d::Renderer::QPainter::setMargins(QMarginsF const& margins)
{
    if (margins.top() < 0.0 || margins.bottom() < 0.0
        || margins.left() < 0.0 || margins.right() < 0.0)
        throw std::invalid_argument("Invalid margins");

    margins_ = margins;
    //_updatePainting |= UpdateWalls | UpdateWays | UpdateBackground;
}

Labyrinth2d::Renderer::QPainter::Texture const& Labyrinth2d::Renderer::QPainter::wallsTexture() const
{
    return wallsTexture_;
}

void Labyrinth2d::Renderer::QPainter::changeWallsTexture(Texture const& wallsTexture)
{
    wallsTexture_ = wallsTexture;
    paintingUpdateTypes_ |= UpdateWalls;
}

Labyrinth2d::Renderer::QPainter::Texture const& Labyrinth2d::Renderer::QPainter::waysTexture() const
{
    return waysTexture_;
}

void Labyrinth2d::Renderer::QPainter::changeWaysTexture(Texture const& waysTexture)
{
    waysTexture_ = waysTexture;
    paintingUpdateTypes_ |= UpdateWays;
}

Labyrinth2d::Renderer::QPainter::Texture const& Labyrinth2d::Renderer::QPainter::backgroundTexture() const
{
    return backgroundTexture_;
}

void Labyrinth2d::Renderer::QPainter::changeBackgroundTexture(Texture const& backgroundTexture)
{
    backgroundTexture_ = backgroundTexture;
    paintingUpdateTypes_ |= UpdateBackground;
}

bool Labyrinth2d::Renderer::QPainter::displayWalls() const
{
    return displayWalls_;
}

void Labyrinth2d::Renderer::QPainter::changeDisplayWalls(bool displayWalls)
{
    if (displayWalls_ != displayWalls)
    {
        displayWalls_ = displayWalls;

        if (displayWalls_ && wallsPixmap_.isNull())
            paintingUpdateTypes_ |= UpdateWalls;
    }
}

bool Labyrinth2d::Renderer::QPainter::displayWays() const
{
    return displayWays_;
}

void Labyrinth2d::Renderer::QPainter::changeDisplayWays(bool displayWays)
{
    if (displayWays_ != displayWays)
    {
        displayWays_ = displayWays;

        if (displayWays_ && waysPixmap_.isNull())
            paintingUpdateTypes_ |= UpdateWays;
    }
}

bool Labyrinth2d::Renderer::QPainter::displayBackground() const
{
    return displayBackground_;
}

void Labyrinth2d::Renderer::QPainter::changeDisplayBackground(bool displayBackground)
{
    if (displayBackground_ != displayBackground)
    {
        displayBackground_ = displayBackground;

        if (displayBackground_ && backgroundPixmap_.isNull())
            paintingUpdateTypes_ |= UpdateBackground;
    }
}
