#ifndef GLLABYRINTH_H
#define GLLABYRINTH_H

#include <QKeyEvent>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QWheelEvent>
#include <GL/gl.h>

#include "Labyrinth2d/Labyrinth.h"

class GLLabyrinth : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    public:
        using QOpenGLWidget::QOpenGLWidget;
        GLLabyrinth(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        ~GLLabyrinth();
        void setLabyrinth(Labyrinth2d::Labyrinth *l);
        void tailleCaseChangee();
        void rechargerTextures();
        void arreterResolution();
        void solveLabyrinth();
        void setTypeResolution(unsigned int type);
        unsigned getTypeResolution() const;

    signals:
        void deplacementJoueur(int dx, int dy);

    private:
        Labyrinth2d::Labyrinth* labyrinth;
        QPixmap pixmapMotifFond;
        QPixmap pixmapImageFond;
        QPixmap pixmapMotifMur;
        QPixmap pixmapImageMur;
        QPixmap pixmapMotifParcours;
        QPixmap pixmapImageParcours;
        QOpenGLShaderProgram *program = nullptr;
        QOpenGLBuffer vbo;
        QMatrix4x4 cameraMatrix_;
        QMatrix4x4 rotationMatrix_;
        std::vector<std::array<QImage, 6> > images_;
        std::vector<std::array<QOpenGLTexture*, 6> > textures_;
        std::vector<std::pair<QVector3D, QVector3D> > boxes_;
        size_t playerBoxId_{0};
        float const sfRatio_{0.95}; //start and finish
        size_t sfBoxId_{0}; // start and finish
        float const traceRatio_{0.05};
        QVector3D wallsSize_ = QVector3D(5, 5, 5);
        QVector3D waysSize_ = QVector3D(20, 20, 20);
        unsigned int typeResolution_{0};

        void libererTextures();
        void translateModelView(QVector3D const& offset, QMatrix4x4& mv, size_t i, size_t j) const;
        QVector3D boxSides(size_t i) const;
        void changeBox(size_t i, QVector3D const& bottom, QVector3D const& top);
        void changeBoxSides(size_t i, QVector3D const& sides);
        void bindBox(size_t i);
        void displayBox(size_t i, QMatrix4x4 const& mv);
        void setTexture(unsigned int i, unsigned int j, const QImage& image);
        const QImage& imageTexture(unsigned int i, unsigned int j) const;
        void makeBox(QVector3D const& bottom,
                     QVector3D const& top,
                     std::array<QImage, 6> const& images = std::array<QImage, 6>{QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32)});
        void makeBox(QVector3D const& sides,
                     std::array<QImage, 6> const& images = std::array<QImage, 6>{QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32)});
        void displayPlayer(size_t playerId, QColor const& color, bool displayPlayer,
                           bool displayTrace, QColor const& traceColor);

    private slots:
        void resolutionLabyrinthe();

    protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int width, int height) override;
        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
};

#endif // GLLABYRINTH_H
