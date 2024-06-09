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
        struct PlayerDisplay
        {
            QColor color{255, 0, 0};
            bool displayTrace{true};
            QColor traceColor{255, 0, 0, 128};
        };

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
        QMatrix4x4 const& cameraMatrix() const;
        PlayerDisplay const& playerDisplay(size_t i) const;
        PlayerDisplay& playerDisplay(size_t i);
        void setPlayerDisplay(size_t i, PlayerDisplay const& playerDisplay);

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
        QMatrix4x4 cameraMatrix_;
        QMatrix4x4 rotationMatrix_;
        QVector3D wallsSize_ = QVector3D(5, 5, 5);
        QVector3D waysSize_ = QVector3D(20, 20, 20);
        std::vector<std::pair<QVector3D, QVector3D> > boxes_;
        size_t playerBoxId_{0};
        float const sfRatio_{0.95}; //start and finish
        size_t sfBoxId_{0}; // start and finish
        float const traceRatio_{0.05};
        std::map<size_t, PlayerDisplay> playerDisplays_;
        size_t firstPlayerId_{0};

        void translateModelView(QVector3D const& offset, QMatrix4x4& mv, size_t i, size_t j, size_t k) const;
        QVector3D boxSides(size_t i) const;
        void changeBox(size_t i, QVector3D const& bottom, QVector3D const& top);
        void changeBoxSides(size_t i, QVector3D const& sides);
        void bindBox(size_t i);
        void displayPlayer(size_t playerId, QColor const& color, bool displayPlayer,
                           bool displayTrace, QColor const& traceColor);
        void displayBox(size_t i, QMatrix4x4 const& mv);

};

#endif
