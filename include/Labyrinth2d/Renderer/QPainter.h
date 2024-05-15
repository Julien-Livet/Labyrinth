#ifndef LABYRINTH2D_RENDERER_QPAINTER_H
#define LABYRINTH2D_RENDERER_QPAINTER_H

#include <map>

#include <QPainter>
#include <QMarginsF>

#include "../Labyrinth.h"

namespace Labyrinth2d
{
    namespace Renderer
    {
        class QPainter
        {
            public:
                class Texture
                {
                    public:
                        enum Type
                        {
                            Brush,
                            Image
                        };

                        enum ImageArrangement
                        {
                            Pattern,
                            Mosaic,
                            Background
                        };

                        Texture(QBrush const& brush, qreal linePercentage = -1.0, Qt::PenStyle penStyle = Qt::SolidLine,
                                Qt::PenCapStyle penCapStyle = Qt::SquareCap, Qt::PenJoinStyle penJoinStyle = Qt::MiterJoin);
                        Texture(QPixmap const& pixmap, ImageArrangement imageArrangement = Pattern,
                                Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio,
                                Qt::TransformationMode transformationMode = Qt::FastTransformation);
                        Type type() const;
                        QBrush const& brush() const;
                        qreal linePercentage() const;
                        Qt::PenStyle penStyle() const;
                        Qt::PenCapStyle penCapStyle() const;
                        Qt::PenJoinStyle penJoinStyle() const;
                        QPixmap const& pixmap() const;
                        ImageArrangement imageArrangement() const;
                        Qt::AspectRatioMode aspectRatioMode() const;
                        Qt::TransformationMode transformationMode() const;
                        void change(QBrush const& brush, qreal linePercentage = -1.0, Qt::PenStyle penStyle = Qt::SolidLine,
                                    Qt::PenCapStyle penCapStyle = Qt::SquareCap, Qt::PenJoinStyle penJoinStyle = Qt::MiterJoin);
                        void change(QPixmap const& pixmap, ImageArrangement imageArrangement = Pattern,
                                    Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio,
                                    Qt::TransformationMode transformationMode = Qt::FastTransformation);

                    private:
                        Type type_;
                        QBrush brush_;
                        qreal linePercentage_;
                        Qt::PenStyle penStyle_;
                        Qt::PenCapStyle penCapStyle_;
                        Qt::PenJoinStyle penJoinStyle_;
                        QPixmap pixmap_;
                        ImageArrangement imageArrangement_;
                        Qt::AspectRatioMode aspectRatioMode_;
                        Qt::TransformationMode transformationMode_;
                };

                struct PlayerRenderer
                {
                    public:
                        PlayerRenderer(bool displayPlayer_ = true, bool displayTrace_ = true,
                                       bool displayStart_ = true, bool displayFinish_ = true);

                        PlayerRenderer(Texture const& playerTexture_, Texture const& traceTexture_,
                                       Texture const& startTexture_, Texture const& exitTexture_,
                                       bool displayPlayer_ = true, bool displayTrace_ = true,
                                       bool displayStart_ = true, bool displayFinish_ = true);

                        Texture playerTexture;
                        Texture traceTexture;
                        Texture startTexture;
                        Texture finishTexture;
                        bool displayPlayer;
                        bool displayTrace;
                        bool displayStart;
                        bool displayFinish;
                };

                std::map<size_t, PlayerRenderer> playerRenderers;
                bool displayPlayers;

                Labyrinth const& labyrinth;

                QPainter(Labyrinth const& _labyrinth);
                void cleanPlayerRenderers();
                QSize const& wallsSize() const;
                QSize const& waysSize() const;
                void changeWallsSize(QSize const& wallsSize);
                void changeWaysSize(QSize const& waysSize);
                QSize size() const;
                QRect rect() const;
                void renderBackground(::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());
                void renderBackground(::QPainter* painter, QPoint const& targetOffset = QPoint());
                void renderWalls(::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());
                void renderWalls(::QPainter* painter, QPoint const& targetOffset = QPoint());
                void renderWays(::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());
                void renderWays(::QPainter* painter, QPoint const& targetOffset = QPoint());
                void renderPlayer(size_t playerId, ::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());
                void renderPlayer(size_t playerId, ::QPainter* painter, QPoint const& targetOffset = QPoint());
                void renderPlayers(::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());
                void renderPlayers(::QPainter* painter, QPoint const& targetOffset = QPoint());
                void render(::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());
                void render(::QPainter* painter, QPoint const& targetOffset = QPoint());
                Texture const& wallsTexture() const;
                void changeWallsTexture(Texture const& wallsTexture);
                Texture const& waysTexture() const;
                void changeWaysTexture(Texture const& waysTexture);
                Texture const& backgroundTexture() const;
                void changeBackgroundTexture(Texture const& backgroundTexture);
                bool displayWalls() const;
                void changeDisplayWalls(bool displayWalls);
                bool displayWays() const;
                void changeDisplayWays(bool displayWays);
                bool displayBackground() const;
                void changeDisplayBackground(bool displayBackground);

                /*!
                 *  \return Margins in percentage
                 */
                QMarginsF const& margins() const;
                void setMargins(QMarginsF const& margins);

                /*!
                 *  \brief Render the labyrinth in a QPaintDevice via a QPainter
                 */
                void operator()(::QPainter* painter, QRegion const& region, QPoint const& targetOffset = QPoint());

                /*!
                 *  \brief Render the labyrinth in a QPaintDevice via a QPainter
                 */
                void operator()(::QPainter* painter, QPoint const& targetOffset = QPoint());

            protected:
                void addLine(std::vector<QLineF>& lines, size_t i1, size_t j1, size_t i2, size_t j2) const;
                void displayCells(::QPainter* painter, Texture const& texture, std::vector<std::pair<size_t, size_t> > const& cells) const;
                QRect computeIndices(QRect const& rect) const;

            private:
                enum PaintingUpdateType_
                {
                    UpdateWalls = 0x1,
                    UpdateWays = 0x2,
                    UpdateBackground = 0x4
                };

                Texture wallsTexture_;
                Texture waysTexture_;
                Texture backgroundTexture_;
                bool displayWalls_;
                bool displayWays_;
                bool displayBackground_;
                QSize wallsSize_;
                QSize waysSize_;
                size_t gridModificationCounter_;
                QPixmap backgroundPixmap_;
                QPixmap wallsPixmap_;
                QPixmap waysPixmap_;
                QRegion region_;
                QMarginsF margins_;
                QPoint corner_;
                size_t paintingUpdateTypes_;
                //std::map<size_t, QPixmap> playersPixmap_;
        };
    }
}

#endif // LABYRINTH2D_RENDERER_QPAINTER_H
