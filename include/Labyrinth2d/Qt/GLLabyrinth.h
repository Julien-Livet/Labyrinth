#ifndef GLLABYRINTH_H
#define GLLABYRINTH_H

#include <QKeyEvent>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QWheelEvent>
#include <GL/gl.h>

#include "Labyrinth2d/Labyrinth.h"
#include "Labyrinth2d/Qt/GeometryEngine.h"

class GLLabyrinth : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    public:
        GLLabyrinth(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        ~GLLabyrinth();
        void setLabyrinth(Labyrinth2d::Labyrinth *l);
        void tailleCaseChangee();
        void rechargerTextures();
        void arreterResolution();
        void solveLabyrinth();
        void setCamera(GLdouble x, GLdouble y, GLdouble z, GLdouble angleRotationZ);
        GLdouble getXCamera() const;
        GLdouble getYCamera() const;
        GLdouble getZCamera() const;
        GLdouble getAngleRotationZCamera() const;
        void setTypeResolution(unsigned int type);
        unsigned getTypeResolution() const;

    signals:
        void deplacementJoueur(int dx, int dy);

    private:
        Labyrinth2d::Labyrinth* labyrinth;
        GLdouble xCamera;
        GLdouble yCamera;
        GLdouble zCamera;
        GLdouble angleRotationZCamera;
        bool toucheDroiteAppuyee;
        bool toucheGaucheAppuyee;
        bool toucheBasAppuyee;
        bool toucheHautAppuyee;
        QTimer* timer;
        QPixmap pixmapMotifFond;
        QPixmap pixmapImageFond;
        QPixmap pixmapMotifMur;
        QPixmap pixmapImageMur;
        QPixmap pixmapMotifParcours;
        QPixmap pixmapImageParcours;
        GLuint glIDMotifFond;
        GLuint glIDImageFond;
        GLuint glIDMotifMur;
        GLuint glIDImageMur;
        GLuint glIDMotifParcours;
        GLuint glIDImageParcours;
        int direction;//0 pour y--, 1 pour x--, 2 pour y++, 3 pour x++
        bool directionAtteinte;
        QTimer* timerResolution;
        double pasAngle;
        double pasDistance;
        unsigned int typeResolution_;
        QOpenGLShaderProgram program;
        QList<QOpenGLTexture*> textures_;
        QMatrix4x4 projection;
        QList<GeometryEngine*> geometries;
        void dessinerMurEntreeSortie(int x, int y);
        void libererTextures();
        bool routineDeplacement();
        void loadTexture(QImage const& image, GLuint& id);
        QOpenGLTexture* loadTexture(QImage const& image);
        void initShaders();
        void initTextures();

    private slots:
        void toucheAppuyee();
        void resolutionLabyrinthe();

    protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int width, int height) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
};

#endif // GLLABYRINTH_H
