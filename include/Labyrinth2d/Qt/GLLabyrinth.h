#ifndef GLLABYRINTH_H
#define GLLABYRINTH_H

#include <QGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <GL/gl.h>

#include "Labyrinth2d/Labyrinth.h"

class GLLabyrinth : public QGLWidget
{
    Q_OBJECT

    public:
        GLLabyrinth(QWidget *parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags f = 0);
        ~GLLabyrinth();
        void setLabyrinth(Labyrinth2d::Labyrinth *l);
        void tailleCaseChangee();
        void rechargerTextures();
        void arreterResolution();
        void resoudreLabyrinthe();
        void setCamera(GLdouble x, GLdouble y, GLdouble z, GLdouble angleRotationZ);
        GLdouble getXCamera() const;
        GLdouble getYCamera() const;
        GLdouble getZCamera() const;
        GLdouble getAngleRotationZCamera() const;

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
        void dessinerMurEntreeSortie(int x, int y);
        void libererTextures();
        bool routineDeplacement();

    private slots:
        void toucheAppuyee();
        void resolutionLabyrinthe();

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        void keyPressEvent(QKeyEvent* event);
        void keyReleaseEvent(QKeyEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);
};

#endif // GLLABYRINTH_H
