#include <QTest>

class TestRendererQPainter : public QObject
{
    Q_OBJECT

    private slots:
        void renderSameColor();
        void renderSameImageMosaic();
        void renderSameImageBackground();
};

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Algorithm/CellFusion.h"
#include "Labyrinth2d/Renderer/QPainter.h"

#include <random>

#include <QImage>
#include <QPainter>

void TestRendererQPainter::renderSameColor()
{
    Labyrinth2d::Labyrinth l{1, 1};
    Labyrinth2d::Renderer::QPainter qpr{l};

    qpr.changeWallsSize(QSize(1, 1));
    qpr.changeWaysSize(QSize(10, 10));

    QColor const color{Qt::blue};

    qpr.changeWallsTexture(Labyrinth2d::Renderer::QPainter::Texture(color, 100.0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
    qpr.changeWaysTexture(Labyrinth2d::Renderer::QPainter::Texture(color, 100.0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
    qpr.changeBackgroundTexture(Labyrinth2d::Renderer::QPainter::Texture(color, 100.0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

    QImage image{qpr.size(), QImage::Format_ARGB32};
    QPainter painter{&image};

    qpr(&painter);

    QImage targetImage{qpr.size(), QImage::Format_ARGB32};
    targetImage.fill(color);

    QCOMPARE(image, targetImage);
}

void TestRendererQPainter::renderSameImageMosaic()
{
    Labyrinth2d::Labyrinth l{10, 10};
    Labyrinth2d::Renderer::QPainter qpr{l};

    qpr.changeWallsSize(QSize(1, 1));
    qpr.changeWaysSize(QSize(10, 10));

    QPixmap const pixmap{"../../../resources/wall_pattern.png"};

    qpr.changeWallsTexture(Labyrinth2d::Renderer::QPainter::Texture(pixmap, Labyrinth2d::Renderer::QPainter::Texture::Mosaic, Qt::KeepAspectRatioByExpanding));
    qpr.changeWaysTexture(Labyrinth2d::Renderer::QPainter::Texture(pixmap, Labyrinth2d::Renderer::QPainter::Texture::Mosaic, Qt::KeepAspectRatioByExpanding));
    qpr.changeBackgroundTexture(Labyrinth2d::Renderer::QPainter::Texture(pixmap, Labyrinth2d::Renderer::QPainter::Texture::Mosaic, Qt::KeepAspectRatioByExpanding));

    QImage image{qpr.size(), QImage::Format_ARGB32};
    QPainter painter{&image};

    qpr(&painter);

    QImage targetImage{qpr.size(), QImage::Format_ARGB32};
    QPainter targetPainter{&targetImage};

    for (int y{0}; y < qpr.size().height(); y += pixmap.height())
    {
        for (int x{0}; x < qpr.size().width(); x += pixmap.width())
            targetPainter.drawPixmap(x, y, pixmap);
    }

    QCOMPARE(image, targetImage);
}

void TestRendererQPainter::renderSameImageBackground()
{
    Labyrinth2d::Labyrinth l{10, 10};
    Labyrinth2d::Renderer::QPainter qpr{l};

    qpr.changeWallsSize(QSize(1, 1));
    qpr.changeWaysSize(QSize(10, 10));

    auto constexpr aspectRatio{Qt::KeepAspectRatioByExpanding};

    auto const pixmap{QPixmap{"../../../resources/background_image.png"}.scaled(qpr.size(),
                                                                                aspectRatio,
                                                                                Qt::SmoothTransformation)};

    qpr.changeWallsTexture(Labyrinth2d::Renderer::QPainter::Texture(pixmap, Labyrinth2d::Renderer::QPainter::Texture::Background, aspectRatio));
    qpr.changeWaysTexture(Labyrinth2d::Renderer::QPainter::Texture(pixmap, Labyrinth2d::Renderer::QPainter::Texture::Background, aspectRatio));
    qpr.changeBackgroundTexture(Labyrinth2d::Renderer::QPainter::Texture(pixmap, Labyrinth2d::Renderer::QPainter::Texture::Background, aspectRatio));

    QImage image{qpr.size(), QImage::Format_ARGB32};
    QPainter painter{&image};

    qpr(&painter);

    QImage targetImage{qpr.size(), QImage::Format_ARGB32};
    QPainter targetPainter{&targetImage};

    for (int y{0}; y < qpr.size().height(); y += pixmap.height())
    {
        for (int x{0}; x < qpr.size().width(); x += pixmap.width())
            targetPainter.drawPixmap(x, y, pixmap);
    }

    QCOMPARE(image, targetImage);
}

QTEST_MAIN(TestRendererQPainter)
#include "testrendererqpainter.moc"
