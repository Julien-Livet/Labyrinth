#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <array>

#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QVector3D>

#include "Labyrinth3d/Labyrinth.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    public:
        using QOpenGLWidget::QOpenGLWidget;
        GLWidget(Labyrinth3d::Labyrinth& labyrinth, QVector3D const& wallsSize = QVector3D(5, 5, 5),
                 QVector3D const& waysSize = QVector3D(20, 20, 20));
        Labyrinth3d::Labyrinth const& labyrinth() const;
        Labyrinth3d::Labyrinth& labyrinth();
        QVector3D const& wallsSize() const;
        void setWallsSize(QVector3D const& wallsSize);
        QVector3D const& waysSize() const;
        void setWaysSize(QVector3D const& waysSize);
        ~GLWidget();
        void setClearColor(const QColor& color);
        void setTexture(unsigned int i, unsigned int j, const QImage& image);
        const QImage& imageTexture(unsigned int i, unsigned int j) const;
        void makeBox(QVector3D const& bottom = QVector3D(-0.2, -0.2, -0.2),
                     QVector3D const& top = QVector3D(0.2, 0.2, 0.2),
                     std::array<QImage, 6> const& images = std::array<QImage, 6>{QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32),
                                                                                 QImage(128, 128, QImage::Format_ARGB32)});
        QMatrix4x4 const& cameraMatrix() const;
        void setCameraMatrix(QMatrix4x4 const& matrix);

    protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int width, int height) override;
        void keyPressEvent(QKeyEvent* event) override;

    private:
        Labyrinth3d::Labyrinth& labyrinth_;
        QColor clearColor_ = Qt::white;
        std::vector<std::array<QImage, 6> > images_;
        std::vector<std::array<QOpenGLTexture*, 6> > textures_;
        QOpenGLShaderProgram *program = nullptr;
        QOpenGLBuffer vbo;
        std::vector<std::array<std::array<std::array<float, 3>, 4>, 6> > coords_;
        float xCamera_ = 0;
        float yCamera_ = 0;
        float zCamera_ = 0;
        float rxCamera_ = 0;
        float ryCamera_ = 0;
        float rzCamera_ = 0;
        QMatrix4x4 cameraMatrix_;
        QVector3D wallsSize_ = QVector3D(5, 5, 5);
        QVector3D waysSize_ = QVector3D(20, 20, 20);
};

#endif
