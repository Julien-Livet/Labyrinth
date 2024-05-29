#include <math.h>
#include <GL/glu.h>

#include <QTimer>

#include "Labyrinth2d/Qt/GLLabyrinth.h"
#include "Labyrinth2d/Qt/QLabyrinth.h"
#include "Labyrinth2d/Qt/constants.h"

GLLabyrinth::GLLabyrinth(QWidget *parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f), QOpenGLFunctions()
{
    labyrinth = 0;
    xCamera = 0;
    yCamera = 0;
    zCamera = 0;
    angleRotationZCamera = 0;
    toucheDroiteAppuyee = false;
    toucheGaucheAppuyee = false;
    toucheBasAppuyee = false;
    toucheHautAppuyee = false;
    glIDMotifFond = 0;
    glIDImageFond = 0;
    glIDMotifMur = 0;
    glIDImageMur = 0;
    glIDMotifParcours = 0;
    glIDImageParcours = 0;
    directionAtteinte = false;
    pasAngle = PASANGLEJEU;
    pasDistance = PASDISTANCEJEU;
    typeResolution_ = 0;

    timer = new QTimer(this);
    timer->setInterval(25);
    timer->setSingleShot(false);

    connect(timer, SIGNAL(timeout()), this, SLOT(toucheAppuyee()));

    timerResolution = new QTimer(this);
    timerResolution->setInterval(100);
    timerResolution->setSingleShot(false);

    connect(timerResolution, SIGNAL(timeout()), this, SLOT(resolutionLabyrinthe()));

    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
    setMouseTracking(true);
}

GLLabyrinth::~GLLabyrinth()
{
    makeCurrent();
    libererTextures();
    for (auto& g : geometries)
        delete g;
    geometries.clear();
    doneCurrent();
}

void GLLabyrinth::setLabyrinth(Labyrinth2d::Labyrinth *l)
{
    labyrinth = l;

    QLabyrinth* lab = qobject_cast<QLabyrinth*>(parentWidget());

    if (!lab)
        return;

    QSize const tailleCase = lab->getTailleCase();
    int const hauteurMur = 2 * qMax(tailleCase.width(), tailleCase.height());

    xCamera = (GLdouble)(lab->getEmplacementXJoueur() + 1.0 / 2.0) * tailleCase.width();
    yCamera = (GLdouble)(lab->getEmplacementYJoueur() + 1.0 / 2.0) * tailleCase.height();
    zCamera = (GLdouble)(hauteurMur / 2);//3
    if (!lab->getEmplacementXJoueur())
        angleRotationZCamera = 0;
    else if (lab->getEmplacementXJoueur() == lab->getLongueur() - 1)
        angleRotationZCamera = 2.0 * M_PI;
    else if (!lab->getEmplacementYJoueur())
        angleRotationZCamera = M_PI_2;
    else if (lab->getEmplacementYJoueur() == lab->getLargeur() - 1)
        angleRotationZCamera = -M_PI_2;
    pasAngle = PASANGLEJEU;
    pasDistance = PASDISTANCEJEU;
    timerResolution->setInterval(100);

    if (lab->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {/*
        //glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);*/
        ///Convert to GL2: glEnable(GL_FOG);
    }
    else
    {/*
        //glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);*/
        ///Convert to GL2: glDisable(GL_FOG);
    }
}

void GLLabyrinth::tailleCaseChangee()
{
    QLabyrinth* lab = qobject_cast<QLabyrinth*>(parentWidget());

    if (!lab)
        return;

    QSize const tailleCase = lab->getTailleCase();
    int const hauteurMur = 2 * qMax(tailleCase.width(), tailleCase.height());

    xCamera = (GLdouble)(lab->getEmplacementXJoueur() + 1.0 / 2.0) * tailleCase.width();
    yCamera = (GLdouble)(lab->getEmplacementYJoueur() + 1.0 / 2.0) * tailleCase.height();
    zCamera = (GLdouble)(hauteurMur / 2);//3
    angleRotationZCamera = 0;
}

void GLLabyrinth::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initShaders();
    initTextures();

    QLabyrinth* lab = qobject_cast<QLabyrinth*>(parentWidget());

    if (!lab)
        return;

    if (lab->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {/*
        //glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);*/
        glEnable(GL_FOG);
    }
    else
    {/*
        //glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);*/
        glDisable(GL_FOG);
    }

    for (size_t i(0); i < labyrinth->grid().height(); ++i)
    {
        for (size_t j(0); j < labyrinth->grid().width(); ++j)
        {
            if (labyrinth->grid().at(i, j))
            {
                QVector3D const bottom((j + 1) / 2 * lab->wallsSize().width() + j / 2 * lab->waysSize().width(),
                                       (i + 1) / 2 * lab->wallsSize().height() + i / 2 * lab->waysSize().height(),
                                       0);
                geometries << new GeometryEngine(bottom,
                                                 bottom +
                                                 QVector3D(lab->wallsSize().width(),
                                                           lab->wallsSize().height(),
                                                           2 * qMax(lab->wallsSize().width(), lab->wallsSize().height())));
            }
        }
    }
}

void GLLabyrinth::paintGL()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    QSize const tailleCase = l->getTailleCase();
    int const hauteurMur = 2 * qMax(tailleCase.width(), tailleCase.height());
    QLabyrinth::Texture textures[3] = {l->getTextureFond(), l->getTextureMur(), l->getTextureParcours()};

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Enable back face culling
    glEnable(GL_CULL_FACE);
/*
    for (auto const& t : textures_)
        t->bind();*/
    textures_[2]->bind();
    program.bind();

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(70, (double)width() / height(), 0.1,
                           qMax((l->getLongueur() + 1) * tailleCase.width(),
                                (l->getLargeur() + 1) * tailleCase.height()));

    if (l->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {
        QColor c = l->getModeLabyrinthe().couleurObscurite;
        glClearColor(c.redF(), c.greenF(), c.blueF(), c.alphaF());
    }
    else
        glClearColor(textures[0].couleur.redF(), textures[0].couleur.greenF(), textures[0].couleur.blueF(), textures[0].couleur.alphaF());

    QMatrix4x4 matrix;

    matrix.scale(-1, 1, 1);

    if ((l->getEmplacementXJoueur() == l->getXSortie() && l->getEmplacementYJoueur() == l->getYSortie()) || (!l->getResolutionProgressive() && l->getEnResolution()))
        matrix.lookAt(QVector3D(l->getLongueur() * tailleCase.width() / 2,
                                l->getLargeur() * tailleCase.height() / 2,
                                qMax(l->getLongueur() * tailleCase.width(),
                                     l->getLargeur() * tailleCase.height()) * 2 / 3),
                      QVector3D(l->getLongueur() * tailleCase.width() / 2,
                                l->getLargeur() * tailleCase.height() / 2,
                                0),
                      QVector3D(0, -1, 0));
    else
        matrix.lookAt(QVector3D(xCamera, yCamera, zCamera),
                      QVector3D(xCamera + 10.0 * cos(angleRotationZCamera),
                                yCamera + 10.0 * sin(angleRotationZCamera),
                                zCamera),
                      QVector3D(0, 0, 1));

    matrix.scale(tailleCase.width(), tailleCase.height(), hauteurMur);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 2 which contains pattern of walls
    program.setUniformValue("texture", 2);

    for (auto&g : geometries)
        g->drawCubeGeometry(&program);

    return;

    if (l->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {/*
        GLint couleurAmbient[] = {255, 0, 0, 255};
        glLightiv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, couleurAmbient);
        GLfloat light0_position[] = {xCamera, yCamera, zCamera, 1};//{0.0f, -10.0f, 0.0f, 0.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);*//*
        //glColor4i(255, 0, 0, 255);
        glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
        glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);
        glColor4i(255, 0, 0, 255);*/
/*
        //différents paramètres
        GLfloat ambient[] = {0.15f,0.15f,0.15f,1.0f};
        GLfloat diffuse[] = {0.5f,0.5f,0.5f,1.0f};
        GLfloat specular_reflexion[] = {0.8f,0.8f,0.8f,1.0f};
        GLubyte shiny_obj = 128;

        //positionnement de la lumière avec les différents paramètres
        glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
        glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
        glEnable(GL_LIGHT0);

        //spécification de la réflexion sur les matériaux
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
        glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient);
        glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular_reflexion);
        glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shiny_obj);*//*
        GLint couleurDiffusion[4] = {255, 255, 255, 255};
        glMaterialiv(GL_FRONT_AND_BACK, GL_SPECULAR, couleurDiffusion);
        glMaterialiv(GL_FRONT_LEFT, GL_SPECULAR, couleurDiffusion);
        glMaterialiv(GL_FRONT_RIGHT, GL_SPECULAR, couleurDiffusion);
        glMaterialiv(GL_FRONT_FACE, GL_SPECULAR, couleurDiffusion);
        glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);
        glMateriali(GL_FRONT_LEFT, GL_SHININESS, 100);
        glMateriali(GL_FRONT_RIGHT, GL_SHININESS, 100);
        glMateriali(GL_FRONT_FACE, GL_SHININESS, 100);

        QColor c = l->getModeLabyrinthe().couleurObscurite;
        if (c == QColor(Qt::black))
            c = QColor(Qt::white);
        GLint couleur[4] = {c.red(), c.green(), c.blue(), 255};
        GLfloat position[4] = {xCamera, yCamera, zCamera, 1};
        glLightiv(GL_LIGHT0, GL_SPECULAR, couleur);
        glLightfv(GL_LIGHT0, GL_POSITION, position);*//*
        glLighti(GL_LINEAR_ATTENUATION, GL_FRONT_AND_BACK, 10000);//qMax(tailleCase.width(), tailleCase.height())*l->getModeLabyrinthe().rayonObscurite);
        glLighti(GL_LINEAR_ATTENUATION, GL_FRONT_LEFT, 10000);//qMax(tailleCase.width(), tailleCase.height())*l->getModeLabyrinthe().rayonObscurite);
        glLighti(GL_LINEAR_ATTENUATION, GL_FRONT_RIGHT, 10000);//qMax(tailleCase.width(), tailleCase.height())*l->getModeLabyrinthe().rayonObscurite);
        glLighti(GL_LINEAR_ATTENUATION, GL_FRONT_FACE, 10000);//qMax(tailleCase.width(), tailleCase.height())*l->getModeLabyrinthe().rayonObscurite);*/
        //GLint couleur[4] = {0, 0, 0, 0};
        //glLightiv(GL_LIGHT0, GL_AMBIENT, couleur);
        //glNormal3f(-cos(angleRotationZCamera), -sin(angleRotationZCamera), 0);
/*
        //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 100.0f);
        int MatSpec[4] = {1, 1, 1, 1};
        //glMaterialiv(GL_FRONT_AND_BACK, GL_AMBIENT, MatSpec);
        glMaterialiv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatSpec);
        glMaterialiv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpec);
        glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);
        GLfloat position[4] = {xCamera, yCamera, zCamera, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        //QColor c = l->getModeLabyrinthe().couleurObscurite;
        //if (c == QColor(Qt::black))
            //c = QColor(Qt::white);
        //GLfloat couleur[4] = {c.redF(), c.greenF(), c.blueF(), 1.0f};
        GLfloat couleurBlanche[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        //glLightfv(GL_LIGHT0, GL_AMBIENT, couleur);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, couleurBlanche);
        glLightfv(GL_LIGHT0, GL_SPECULAR, couleurBlanche);*/
/**Convert into GL2:
        GLfloat rayon = qMax(tailleCase.width(), tailleCase.height()) * l->getModeLabyrinthe().rayonObscurite;
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 0);
        glFogf(GL_FOG_END, rayon);
        QColor c = l->getModeLabyrinthe().couleurObscurite;
        GLfloat couleur[4] = {c.redF(), c.greenF(), c.blueF(), 1.0f};
        glFogfv(GL_FOG_COLOR, couleur);**/
    }

    if (!glIDMotifFond && !glIDImageFond && !glIDMotifMur && !glIDImageMur && !glIDMotifParcours && !glIDImageParcours)
        rechargerTextures();

    //Dessin du fond
    if (textures[0].typeTexture == QLabyrinth::TextureMotif && !pixmapMotifFond.isNull())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glIDMotifFond);/**Convert into GL2:
        glBegin(GL_QUADS);

        //Face inférieure du cube
        glTexCoord2i(0, 0);
        glVertex3d(0, 0, 0);
        glTexCoord2i(0, pixmapMotifFond.height() / tailleCase.height());
        glVertex3d(l->getLongueur(), 0, 0);
        glTexCoord2i(pixmapMotifFond.width() / tailleCase.width(), pixmapMotifFond.height() / tailleCase.height());
        glVertex3d(l->getLongueur(), l->getLargeur(), 0);
        glTexCoord2i(pixmapMotifFond.width() / tailleCase.width(), 0);
        glVertex3d(0, l->getLargeur(), 0);

        glEnd();**/
        glDisable(GL_TEXTURE_2D);
    }
    else if (textures[0].typeTexture == QLabyrinth::TextureImage && !pixmapImageFond.isNull())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glIDImageFond);/**Convert into GL2:
        glBegin(GL_QUADS);

        //Face inférieure du cube
        glTexCoord2i(0, 0);
        glVertex3d(0, 0, 0);
        glTexCoord2i(0, pixmapImageFond.height() / tailleCase.height());
        glVertex3d(l->getLongueur(), 0, 0);
        glTexCoord2i(pixmapImageFond.width() / tailleCase.width(), pixmapImageFond.height() / tailleCase.height());
        glVertex3d(l->getLongueur(), l->getLargeur(), 0);
        glTexCoord2i(pixmapImageFond.width() / tailleCase.width(), 0);
        glVertex3d(0, l->getLargeur(), 0);

        glEnd();**/
        glDisable(GL_TEXTURE_2D);
    }
    else// if (textures[0].typeTexture == QLabyrinth::TextureCouleur)
    {/**Convert into GL2:
        glBegin(GL_QUADS);

        glColor3ub(textures[0].couleur.red(), textures[0].couleur.green(), textures[0].couleur.blue());
        glVertex3d(0, 0, 0);
        glVertex3d(l->getLongueur(), 0, 0);
        glVertex3d(l->getLongueur(), l->getLargeur(), 0);
        glVertex3d(0, l->getLargeur(), 0);

        glEnd();**/
    }

	auto const& grid{labyrinth->grid()};

    //Dessin du mur
    if (textures[1].typeTexture == QLabyrinth::TextureMotif && !pixmapMotifMur.isNull())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glIDMotifMur);/**Convert into GL2:
        glBegin(GL_QUADS);
        for (int i = 0; i < l->getLargeur(); i++)
        {
            for (int j = 0; j < l->getLongueur(); j++)
            {
                if (grid.at(i, j) == 1)
                {
                    //Face avant du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j, i, 1);
                    glTexCoord2i(0, pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j, i, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), 0);
                    glVertex3d(j + 1, i, 1);

                    //Face latérale gauche du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j, i + 1, 1);
                    glTexCoord2i(0, pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j, i + 1, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j, i, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), 0);
                    glVertex3d(j, i, 1);

                    //Face arrière du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j, i + 1, 1);
                    glTexCoord2i(0, pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j, i + 1, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i + 1, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), 0);
                    glVertex3d(j + 1, i + 1, 1);

                    //Face latérale droite du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j + 1, i + 1, 1);
                    glTexCoord2i(0, pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i + 1, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), pixmapMotifMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i, 0);
                    glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), 0);
                    glVertex3d(j + 1, i, 1);

                    if (l->getPartieTerminee())
                    {
                        //Face supérieure du cube
                        glTexCoord2i(0, 0);
                        glVertex3d(j, i, 0);
                        glTexCoord2i(0, pixmapMotifMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i, 0);
                        glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), pixmapMotifMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i + 1, 0);
                        glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), 0);
                        glVertex3d(j, i + 1, 0);

                        //Face inférieure du cube
                        glTexCoord2i(0, 0);
                        glVertex3d(j, i, 1);
                        glTexCoord2i(0, pixmapMotifMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i, 1);
                        glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), pixmapMotifMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i + 1, 1);
                        glTexCoord2i(pixmapMotifMur.width() / tailleCase.width(), 0);
                        glVertex3d(j, i + 1, 1);
                    }
                }
            }
        }
        glEnd();**/
        glDisable(GL_TEXTURE_2D);
    }
    else if (textures[1].typeTexture == QLabyrinth::TextureImage && !pixmapImageMur.isNull())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glIDImageMur);/**Convert into GL2:
        glBegin(GL_QUADS);
        for (int i = 0; i < l->getLargeur(); i++)
        {
            for (int j = 0; j < l->getLongueur(); j++)
            {
                if (grid.at(i, j) == 1)
                {
                    //Face avant du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j, i, 1);
                    glTexCoord2i(0, pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j, i, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), 0);
                    glVertex3d(j + 1, i, 1);

                    //Face latérale gauche du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j, i + 1, 1);
                    glTexCoord2i(0, pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j, i + 1, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j, i, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), 0);
                    glVertex3d(j, i, 1);

                    //Face arrière du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j, i + 1, 1);
                    glTexCoord2i(0, pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j, i+1, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i + 1, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), 0);
                    glVertex3d(j + 1, i + 1, 1);

                    //Face latérale droite du cube
                    glTexCoord2i(0, 0);
                    glVertex3d(j + 1, i + 1, 1);
                    glTexCoord2i(0, pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i + 1, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), pixmapImageMur.height() / tailleCase.height());
                    glVertex3d(j + 1, i, 0);
                    glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), 0);
                    glVertex3d(j + 1, i, 1);

                    if (l->getPartieTerminee())
                    {
                        //Face supérieure du cube
                        glTexCoord2i(0, 0);
                        glVertex3d(j, i, 0);
                        glTexCoord2i(0, pixmapImageMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i, 0);
                        glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), pixmapImageMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i + 1, 0);
                        glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), 0);
                        glVertex3d(j, i + 1, 0);

                        //Face inférieure du cube
                        glTexCoord2i(0, 0);
                        glVertex3d(j, i, 1);
                        glTexCoord2i(0, pixmapImageMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i, 1);
                        glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), pixmapImageMur.height() / tailleCase.height());
                        glVertex3d(j + 1, i + 1, 1);
                        glTexCoord2i(pixmapImageMur.width() / tailleCase.width(), 0);
                        glVertex3d(j, i + 1, 1);
                    }
                }
            }
        }
        glEnd();**/
        glDisable(GL_TEXTURE_2D);
    }
    else// if (textures[1].typeTexture == QLabyrinth::TextureCouleur)
    {/**Convert into GL2:
        glBegin(GL_QUADS);
        glColor3ub(textures[1].couleur.red(), textures[1].couleur.green(), textures[1].couleur.blue());
        for (int i = 0; i < l->getLargeur(); i++)
        {
            for (int j = 0; j < l->getLongueur(); j++)
            {
                if (grid.at(i, j) == 1)
                {
                    //Face avant du cube
                    glVertex3d(j, i, 1);
                    glVertex3d(j, i, 0);
                    glVertex3d(j + 1, i, 0);
                    glVertex3d(j + 1, i, 1);

                    //Face latérale gauche du cube
                    glVertex3d(j, i + 1, 1);
                    glVertex3d(j, i + 1, 0);
                    glVertex3d(j, i, 0);
                    glVertex3d(j, i, 1);

                    //Face arrière du cube
                    glVertex3d(j, i + 1, 1);
                    glVertex3d(j, i + 1, 0);
                    glVertex3d(j + 1, i + 1, 0);
                    glVertex3d(j + 1, i + 1, 1);

                    //Face latérale droite du cube
                    glVertex3d(j + 1, i + 1, 1);
                    glVertex3d(j + 1, i + 1, 0);
                    glVertex3d(j + 1, i, 0);
                    glVertex3d(j + 1, i, 1);

                    if (l->getPartieTerminee())
                    {
                        //Face supérieure du cube
                        glVertex3d(j, i, 0);
                        glVertex3d(j + 1, i, 0);
                        glVertex3d(j + 1, i + 1, 0);
                        glVertex3d(j, i + 1, 0);

                        //Face inférieure du cube
                        glVertex3d(j, i, 1);
                        glVertex3d(j + 1, i, 1);
                        glVertex3d(j + 1, i + 1, 1);
                        glVertex3d(j, i + 1, 1);
                    }
                }
            }
        }
        glEnd();**/
    }

    //Dessin du parcours
    if (textures[2].typeTexture == QLabyrinth::TextureMotif && !pixmapMotifParcours.isNull())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glIDMotifParcours);/**Convert into GL2:
        glBegin(GL_QUADS);
        if (l->getAfficherTrace())
        {
			auto const& player{labyrinth->player(labyrinth->playerIds().front())};
			auto traceIntersections{player.traceIntersections()};
			traceIntersections.emplace_back(std::make_pair<size_t, size_t>(player.i(), player.j()));
			auto i{traceIntersections.front().first};
			auto j{traceIntersections.front().second};
			
			for (size_t k{0}; k < traceIntersections.size() - 1; ++k)
			{
				while (i != traceIntersections[k + 1].first && j != traceIntersections[k + 1].second) 
				{
					//Face inférieure du cube
                    if (static_cast<int>(i) != l->getEmplacementYJoueur() || static_cast<int>(j) != l->getEmplacementXJoueur())
						glColor4ub(255, 255, 255, textures[2].couleur.alpha() / 2);
					else
						glColor4ub(255, 255, 255, textures[2].couleur.alpha());
					glTexCoord2i(0, 0);
					glVertex3d(j, i, 0.001);
					glTexCoord2i(0, pixmapMotifParcours.height() / tailleCase.height());
					glVertex3d(j + 1, i, 0.001);
                    glTexCoord2i(pixmapMotifParcours.width() / tailleCase.width(), pixmapMotifParcours.height() / tailleCase.height());
					glVertex3d(j + 1, i + 1, 0.001);
					glTexCoord2i(pixmapMotifParcours.width() / tailleCase.width(), 0);
					glVertex3d(j, i + 1, 0.001);
					
					if (i < traceIntersections[k + 1].first)
						++i;
					else if (traceIntersections[k + 1].first > i)
						--i;
					else if (j < traceIntersections[k + 1].second)
						++j;
					else if (traceIntersections[k + 1].second > j)
						--j;
				}
			}
        }
        else
        {
            //Face inférieure du cube
            glColor4ub(255, 255, 255, textures[2].couleur.alpha());
            glTexCoord2i(0, 0);
            glVertex3d(l->getEmplacementXJoueur(), l->getEmplacementYJoueur(), 0.001);
            glTexCoord2i(0, pixmapMotifParcours.height() / tailleCase.height());
            glVertex3d(l->getEmplacementXJoueur() + 1, l->getEmplacementYJoueur(), 0.001);
            glTexCoord2i(pixmapMotifParcours.width() / tailleCase.width(), pixmapMotifParcours.height() / tailleCase.height());
            glVertex3d(l->getEmplacementXJoueur() + 1, l->getEmplacementYJoueur() + 1, 0.001);
            glTexCoord2i(pixmapMotifParcours.width() / tailleCase.width(), 0);
            glVertex3d(l->getEmplacementXJoueur(), l->getEmplacementYJoueur() + 1, 0.001);
        }
        glEnd();**/
        glDisable(GL_TEXTURE_2D);
    }
    else if (textures[2].typeTexture == QLabyrinth::TextureImage && !pixmapImageParcours.isNull())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glIDImageParcours);
        ///Convert into GL2: glBegin(GL_QUADS);
        if (l->getAfficherTrace())
        {
			auto const& player{labyrinth->player(labyrinth->playerIds().front())};
			auto traceIntersections{player.traceIntersections()};
			traceIntersections.emplace_back(std::make_pair<size_t, size_t>(player.i(), player.j()));
			auto i{traceIntersections.front().first};
			auto j{traceIntersections.front().second};
			
			for (size_t k{0}; k < traceIntersections.size() - 1; ++k)
			{
				while (i != traceIntersections[k + 1].first && j != traceIntersections[k + 1].second) 
                {/**Convert into GL2:
					//Face inférieure du cube
                    if (static_cast<int>(i) != l->getEmplacementYJoueur() || static_cast<int>(j) != l->getEmplacementXJoueur())
						glColor4ub(255, 255, 255, textures[2].couleur.alpha() / 2);
					else
						glColor4ub(255, 255, 255, textures[2].couleur.alpha());
					glTexCoord2i(0, 0);
					glVertex3d(j, i, 0.001);
					glTexCoord2i(0, pixmapImageParcours.height() / tailleCase.height());
					glVertex3d(j + 1, i, 0.001);
					glTexCoord2i(pixmapImageParcours.width() / tailleCase.width(), pixmapImageParcours.height() / tailleCase.height());
					glVertex3d(j + 1, i + 1, 0.001);
					glTexCoord2i(pixmapImageParcours.width() / tailleCase.width(), 0);
                    glVertex3d(j, i + 1, 0.001);**/
					
					if (i < traceIntersections[k + 1].first)
						++i;
					else if (traceIntersections[k + 1].first > i)
						--i;
					else if (j < traceIntersections[k + 1].second)
						++j;
					else if (traceIntersections[k + 1].second > j)
						--j;
				}
			}
        }
        else
        {/**Convert into GL2:
            //Face inférieure du cube
            glColor4ub(255, 255, 255, textures[2].couleur.alpha());
            glTexCoord2i(0, 0);
            glVertex3d(l->getEmplacementXJoueur(), l->getEmplacementYJoueur(), 0.001);
            glTexCoord2i(0, pixmapImageParcours.height() / tailleCase.height());
            glVertex3d(l->getEmplacementXJoueur() + 1, l->getEmplacementYJoueur(), 0.001);
            glTexCoord2i(pixmapImageParcours.width() / tailleCase.width(), pixmapImageParcours.height() / tailleCase.height());
            glVertex3d(l->getEmplacementXJoueur() + 1, l->getEmplacementYJoueur() + 1, 0.001);
            glTexCoord2i(pixmapImageParcours.width() / tailleCase.width(), 0);
            glVertex3d(l->getEmplacementXJoueur(), l->getEmplacementYJoueur() + 1, 0.001);**/
        }
        ///Convert into GL2: glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    else// if (textures[2].typeTexture == QLabyrinth::TextureCouleur)
    {
        ///Convert into GL2: glBegin(GL_QUADS);
        if (l->getAfficherTrace())
        {
			auto const& player{labyrinth->player(labyrinth->playerIds().front())};
			auto traceIntersections{player.traceIntersections()};
			traceIntersections.emplace_back(std::make_pair<size_t, size_t>(player.i(), player.j()));
			auto i{traceIntersections.front().first};
			auto j{traceIntersections.front().second};
			
			for (size_t k{0}; k < traceIntersections.size() - 1; ++k)
			{
				while (i != traceIntersections[k + 1].first && j != traceIntersections[k + 1].second) 
                {/**Convert into GL2:
					//Face inférieure du cube
                    if (static_cast<int>(i) != l->getEmplacementYJoueur() || static_cast<int>(j) != l->getEmplacementXJoueur())
						glColor4ub(textures[2].couleur.red(), textures[2].couleur.green(), textures[2].couleur.blue(), textures[2].couleur.alpha() / 2);
					else
						glColor4ub(textures[2].couleur.red(), textures[2].couleur.green(), textures[2].couleur.blue(), textures[2].couleur.alpha());
					glVertex3d(j, i, 0.001);
					glVertex3d(j + 1, i, 0.001);
					glVertex3d(j + 1, i + 1, 0.001);
                    glVertex3d(j, i + 1, 0.001);**/
					
					if (i < traceIntersections[k + 1].first)
						++i;
					else if (traceIntersections[k + 1].first > i)
						--i;
					else if (j < traceIntersections[k + 1].second)
						++j;
					else if (traceIntersections[k + 1].second > j)
						--j;
				}
			}
        }
        else
        {/**Convert into GL2:
            //Face inférieure du cube
            glColor4ub(textures[2].couleur.red(), textures[2].couleur.green(), textures[2].couleur.blue(), textures[2].couleur.alpha());
            glVertex3d(l->getEmplacementXJoueur(), l->getEmplacementYJoueur(), 0.001);
            glVertex3d(l->getEmplacementXJoueur() + 1, l->getEmplacementYJoueur(), 0.001);
            glVertex3d(l->getEmplacementXJoueur() + 1, l->getEmplacementYJoueur() + 1, 0.001);
            glVertex3d(l->getEmplacementXJoueur(), l->getEmplacementYJoueur() + 1, 0.001);**/
        }
        ///Convert into GL2: glEnd();
    }
/**Convert into GL2:
    glBegin(GL_QUADS);

    //Face inférieure du cube
    glColor4ub(0, 255, 0, 128);
    glVertex3d(l->getXEntree(), l->getYEntree(), 0.002);
    glVertex3d(l->getXEntree() + 1, l->getYEntree(), 0.002);
    glVertex3d(l->getXEntree() + 1, l->getYEntree() + 1, 0.002);
    glVertex3d(l->getXEntree(), l->getYEntree() + 1, 0.002);

    glColor4ub(0, 255, 0, 255);**/
    dessinerMurEntreeSortie(l->getXEntree(), l->getYEntree());
/**Convert into GL2:
    //Face inférieure du cube
    glColor4ub(255, 0, 0, 128);
    glVertex3d(l->getXSortie(), l->getYSortie(), 0.002);
    glVertex3d(l->getXSortie() + 1, l->getYSortie(), 0.002);
    glVertex3d(l->getXSortie() + 1, l->getYSortie() + 1, 0.002);
    glVertex3d(l->getXSortie(), l->getYSortie() + 1, 0.002);

    glColor4ub(255, 0, 0, 255);**/
    dessinerMurEntreeSortie(l->getXSortie(), l->getYSortie());

    ///Convert into GL2: glEnd();
/**
    if (l->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {
        glPushMatrix();
        glTranslated(xCamera / tailleCase.width(), yCamera / tailleCase.height(), zCamera / hauteurMur);
        QColor c = l->getModeLabyrinthe().couleurObscurite;
        GLdouble rayon = qMax(tailleCase.width(), tailleCase.height()) * l->getModeLabyrinthe().rayonObscurite;
        GLUquadric *params = gluNewQuadric();
        for (int i = 255; i >= 0; i -= 1)
        {
            glColor4ub(c.red(), c.green(), c.blue(), i);
            gluSphere(params, (double)rayon * i / 255, 50, 50);
        }
        gluDeleteQuadric(params);

        glPopMatrix();
    }
**/
    //On efface la couleur
    ///Convert into GL2: glColor4ub(255, 255, 255, 255);
}

void GLLabyrinth::dessinerMurEntreeSortie(int x, int y)
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());
/**Convert into GL2:
    if (!x)
    {
        glVertex3d(0, y + 1, 1);
        glVertex3d(0, y + 1, 0);
        glVertex3d(0, y, 0);
        glVertex3d(0, y, 1);
    }
    else if (x == l->getLongueur() - 1)
    {
        glVertex3d(x + 1, y + 1, 1);
        glVertex3d(x + 1, y + 1, 0);
        glVertex3d(x + 1, y, 0);
        glVertex3d(x + 1, y, 1);
    }
    else if (!y)
    {
        glVertex3d(x, 0, 1);
        glVertex3d(x, 0, 0);
        glVertex3d(x + 1, 0, 0);
        glVertex3d(x + 1, 0, 1);
    }
    else if (y == l->getLargeur() - 1)
    {
        glVertex3d(x, y + 1, 1);
        glVertex3d(x, y + 1, 0);
        glVertex3d(x + 1, y + 1, 0);
        glVertex3d(x + 1, y + 1, 1);
    }**/
}

void GLLabyrinth::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    update();
}

void GLLabyrinth::keyPressEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyPressEvent(event);

    if (event->isAutoRepeat())
        return;

    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    if (l->enModeEcranDeVeille())
        l->quitterModeEcranDeVeille();

    if (l->getPartieEnPause() || (l->getEmplacementXJoueur() == l->getXSortie() && l->getEmplacementYJoueur() == l->getYSortie()) || l->getEnResolution())
        return;

    switch (event->key())
    {
        case Qt::Key_Right:
            toucheDroiteAppuyee = true;
            if (!timer->isActive())
                toucheAppuyee();
            break;
        case Qt::Key_Left:
            toucheGaucheAppuyee = true;
            if (!timer->isActive())
                toucheAppuyee();
            break;
        case Qt::Key_Down:
            toucheBasAppuyee = true;
            if (!timer->isActive())
                toucheAppuyee();
            break;
        case Qt::Key_Up:
            toucheHautAppuyee = true;
            if (!timer->isActive())
                toucheAppuyee();
            break;
        default:
            break;
    }
}

void GLLabyrinth::keyReleaseEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyReleaseEvent(event);

    if (event->isAutoRepeat())
        return;

    switch (event->key())
    {
        case Qt::Key_Right:
            toucheDroiteAppuyee = false;
            break;
        case Qt::Key_Left:
            toucheGaucheAppuyee = false;
            break;
        case Qt::Key_Down:
            toucheBasAppuyee = false;
            break;
        case Qt::Key_Up:
            toucheHautAppuyee = false;
            break;
        default:
            break;
    }
}

void GLLabyrinth::toucheAppuyee()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    if (l->getPartieTerminee() || l->getPartieEnPause())
    {
        timer->stop();
        toucheDroiteAppuyee = false;
        toucheGaucheAppuyee = false;
        toucheBasAppuyee = false;
        toucheHautAppuyee = false;
        return;
    }

    routineDeplacement();
}

bool GLLabyrinth::routineDeplacement()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return false;

    QSize tailleCase = l->getTailleCase();
    GLdouble x = xCamera;
    GLdouble y = yCamera;
    int deplacementX = 0;
    int deplacementY = 0;

    bool lancerTimer = false;

    if (toucheDroiteAppuyee)
    {
        angleRotationZCamera += pasAngle * M_PI / 180.0;
        lancerTimer = true;
        if (angleRotationZCamera)
            while (fabs(angleRotationZCamera) >= M_PI)
                angleRotationZCamera -= angleRotationZCamera / fabs(angleRotationZCamera) * 2.0 * M_PI;
    }
    if (toucheGaucheAppuyee)
    {
        angleRotationZCamera -= pasAngle * M_PI / 180.0;
        lancerTimer = true;
    }
    bool b = false;
    if (toucheBasAppuyee)
    {
        x -= pasDistance * cos(angleRotationZCamera);
        y -= pasDistance * sin(angleRotationZCamera);
        b = true;
    }
    if (toucheHautAppuyee)
    {
        x += pasDistance * cos(angleRotationZCamera);
        y += pasDistance * sin(angleRotationZCamera);
        b = true;
    }
    if (b)
    {
        if (l->getEmplacementXJoueur() * tailleCase.width() + 1 <= x && x <= (l->getEmplacementXJoueur() + 1) * tailleCase.width() - 1 &&
            l->getEmplacementYJoueur() * tailleCase.height() + 1 <= y && y <= (l->getEmplacementYJoueur() + 1) * tailleCase.height() - 1)
        {
            deplacementX = 0;
            deplacementY = 0;
        }
        else
        {
			auto const& grid{labyrinth->grid()};
			
            if (x < l->getEmplacementXJoueur() * tailleCase.width() + 1)
            {
                if (!l->getEmplacementXJoueur()
                    || grid.at(l->getEmplacementYJoueur(), l->getEmplacementXJoueur() - 1) == 1)
                    x = xCamera;
                else if (x < l->getEmplacementXJoueur() * tailleCase.width())
                    deplacementX = -1;
            }
            else if ((l->getEmplacementXJoueur() + 1) * tailleCase.width() - 1 < x)
            {
                if (l->getEmplacementXJoueur() == l->getLongueur()-1
                    || grid.at(l->getEmplacementYJoueur(), l->getEmplacementXJoueur() + 1) == 1)
                    x = xCamera;
                else if ((l->getEmplacementXJoueur() + 1) * tailleCase.width() < x)
                    deplacementX = 1;
            }
            if (y < l->getEmplacementYJoueur() * tailleCase.height()+1)
            {
                if (!l->getEmplacementYJoueur()
                    || grid.at(l->getEmplacementYJoueur() - 1, l->getEmplacementXJoueur()) == 1)
                    y = yCamera;
                else if (y < l->getEmplacementYJoueur() * tailleCase.height())
                    deplacementY = -1;
            }
            else if ((l->getEmplacementYJoueur() + 1) * tailleCase.height() - 1 < y)
            {
                if (l->getEmplacementYJoueur() == l->getLargeur() - 1
                    || grid.at(l->getEmplacementYJoueur() + 1, l->getEmplacementXJoueur()) == 1)
                    y = yCamera;
                else if ((l->getEmplacementYJoueur() + 1) * tailleCase.height() < y)
                    deplacementY = 1;
            }
            if (fabs(deplacementX) && fabs(deplacementY))
            {
                deplacementX = 0;
                deplacementY = 0;
                x = xCamera;
                y = yCamera;
            }
        }

        if (l->getEmplacementXJoueur() + deplacementX == l->getXSortie()
            && l->getEmplacementYJoueur() + deplacementY == l->getYSortie())
        {
            timer->stop();
            toucheDroiteAppuyee = false;
            toucheGaucheAppuyee = false;
            toucheBasAppuyee = false;
            toucheHautAppuyee = false;
        }

        xCamera = x;
        yCamera = y;
        lancerTimer = true;
    }

    update();

    emit deplacementJoueur(deplacementX, deplacementY);

    if (!l->getEnResolution())
    {
        if (lancerTimer)
        {
            if (!timer->isActive())
                timer->start();
        }
        else
            timer->stop();
    }

    return (deplacementX || deplacementY);
}

void GLLabyrinth::rechargerTextures()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    QLabyrinth::Texture textures[3] = {l->getTextureFond(), l->getTextureMur(), l->getTextureParcours()};

    //Chargement des images
    pixmapMotifFond = QPixmap(textures[0].motif).scaled(32, 32);
    pixmapImageFond = QPixmap(textures[0].image);
    pixmapMotifMur = QPixmap(textures[1].motif).scaled(32, 32);
    pixmapImageMur = QPixmap(textures[1].image);
    pixmapMotifParcours = QPixmap(textures[2].motif).scaled(32, 32);
    pixmapImageParcours = QPixmap(textures[2].image);

    libererTextures();

    initTextures();
}

void GLLabyrinth::libererTextures()
{
    for (auto const& t : textures_)
        delete t;
    textures_.clear();
/*
    if (glIDMotifFond)
    {
        glDeleteTextures(1, &glIDMotifFond);
        glIDMotifFond = 0;
    }
    if (glIDImageFond)
    {
        glDeleteTextures(1, &glIDImageFond);
        glIDImageFond = 0;
    }
    if (glIDMotifMur)
    {
        glDeleteTextures(1, &glIDMotifMur);
        glIDMotifMur = 0;
    }
    if (glIDImageMur)
    {
        glDeleteTextures(1, &glIDImageMur);
        glIDImageMur = 0;
    }
    if (glIDMotifParcours)
    {
        glDeleteTextures(1, &glIDMotifParcours);
        glIDMotifParcours = 0;
    }
    if (glIDImageParcours)
    {
        glDeleteTextures(1, &glIDImageParcours);
        glIDImageParcours = 0;
    }*/
}

void GLLabyrinth::solveLabyrinth()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    QSize const tailleCase = l->getTailleCase();
    int const hauteurMur = 2 * qMax(tailleCase.width(), tailleCase.height());
    int const emplacementXJoueur = l->getEmplacementXJoueur();
    int const emplacementYJoueur = l->getEmplacementYJoueur();
    int const xSortie = l->getXSortie();
    int const ySortie = l->getYSortie();
    int const longueur = l->getLongueur();
    int const largeur = l->getLargeur();

    if (emplacementXJoueur == xSortie && emplacementYJoueur == ySortie)
    {
        emit deplacementJoueur(0, 0);

        update();

        return;
    }

    xCamera = (GLdouble)(emplacementXJoueur + 1.0 / 2.0) * tailleCase.width();
    yCamera = (GLdouble)(emplacementYJoueur + 1.0 / 2.0) * tailleCase.height();
    zCamera = (GLdouble)(hauteurMur / 2);//3

    toucheDroiteAppuyee = false;
    toucheGaucheAppuyee = false;
    toucheHautAppuyee = false;
    toucheBasAppuyee = false;
    directionAtteinte = false;
    pasAngle = PASANGLERESOLUTION;
    pasDistance = PASDISTANCERESOLUTION;
    timerResolution->setInterval(1);

    int nombrePossibilites = 0;

    direction = -1;

	auto const& grid{labyrinth->grid()};

    if (emplacementXJoueur)
    {
        if (!grid.at(emplacementYJoueur, emplacementXJoueur - 1))
            nombrePossibilites++;
        bool b = true;
        if (emplacementYJoueur == ySortie)
        {
            for (int i = emplacementXJoueur - 1; i >= 0; i--)
            {
                if (grid.at(emplacementYJoueur, i) == 1)
                {
                    b = false;
                    break;
                }
                else if (i == xSortie)
                    break;
            }
        }
        else
            b = false;
        if (b)
            direction = 1;
    }

    if (emplacementXJoueur + 1 < longueur)
    {
        if (!grid.at(emplacementYJoueur, emplacementXJoueur + 1))
            nombrePossibilites++;
        bool b = true;
        if (emplacementYJoueur == ySortie)
        {
            for (int i = emplacementXJoueur + 1; i < longueur; i++)
            {
                if (grid.at(emplacementYJoueur, i) == 1)
                {
                    b = false;
                    break;
                }
                else if (i == xSortie)
                    break;
            }
        }
        else
            b = false;
        if (b)
            direction = 3;
    }

    if (emplacementYJoueur)
    {
        if (!grid.at(emplacementYJoueur - 1, emplacementXJoueur))
            nombrePossibilites++;
        bool b = true;
        if (emplacementXJoueur == xSortie)
        {
            for (int j = emplacementYJoueur - 1; j >= 0; j--)
            {
                if (grid.at(j, emplacementXJoueur) == 1)
                {
                    b = false;
                    break;
                }
                else if (j == ySortie)
                    break;
            }
        }
        else
            b = false;
        if (b)
            direction = 0;
    }

    if (emplacementYJoueur + 1 < largeur)
    {
        if (!grid.at(emplacementYJoueur + 1, emplacementXJoueur))
            nombrePossibilites++;
        bool b = true;
        if (emplacementXJoueur == xSortie)
        {
            for (int j = emplacementYJoueur + 1; j < largeur; j++)
            {
                if (grid.at(j, emplacementXJoueur) == 1)
                {
                    b = false;
                    break;
                }
                else if (j == ySortie)
                    break;
            }
        }
        else
            b = false;
        if (b)
            direction = 2;
    }

    if (direction == -1)
    {
        if (nombrePossibilites)
        {
            int directionHorizontale = 0, directionVerticale = 0;

            do
            {
                if (!(rand() % 2))
                    directionHorizontale = 0;
                else
                {
                    if (!(rand() % 2))
                        directionHorizontale = -1;
                    else
                        directionHorizontale = 1;
                }

                if (!directionHorizontale)
                {
                    if (!(rand() % 2))
                        directionVerticale = -1;
                    else
                        directionVerticale = 1;
                }
                else
                    directionVerticale = 0;
            } while (emplacementXJoueur + directionHorizontale < 0 || emplacementXJoueur + directionHorizontale >= longueur || emplacementYJoueur + directionVerticale < 0 || emplacementYJoueur + directionVerticale >= largeur || grid.at(emplacementYJoueur + directionVerticale, emplacementXJoueur + directionHorizontale));

            if (directionHorizontale)
                direction = 2 + directionHorizontale;
            else// if (directionVerticale
                direction = 1 + directionVerticale;
        }/*
        else
        {
            //Correct comparison with 2
            bool b = true;

            if (emplacementXJoueur)
            {
                if (grid.at(emplacementYJoueur, emplacementXJoueur - 1) == 2)
                {
                    direction = 1;
                    b = false;
                }
            }

            if (b && emplacementXJoueur + 1 < longueur)
            {
                if (grid.at(emplacementYJoueur, emplacementXJoueur + 1) == 2)
                {
                    direction = 3;
                    b = false;
                }
            }

            if (b && emplacementYJoueur)
            {
                if (grid.at(emplacementYJoueur - 1, emplacementXJoueur) == 2)
                {
                    direction = 0;
                    b = false;
                }
            }

            if (b && emplacementYJoueur + 1 < largeur)
            {
                if (grid.at(emplacementYJoueur + 1, emplacementXJoueur) == 2)
                    direction = 2;
            }
        }*/
    }

    timerResolution->start();
}

void GLLabyrinth::arreterResolution()
{
    timerResolution->stop();
}

void GLLabyrinth::resolutionLabyrinthe()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    if (l->getEnResolution() && !l->getResolutionProgressive())
    {
        timerResolution->stop();
        update();
        l->resoudre();
    }

    if (!l->getArretResolution() && (l->getEmplacementXJoueur() != l->getXSortie() || l->getEmplacementYJoueur() != l->getYSortie()) && l->getEnResolution())
    {
        switch (direction)
        {
            case 0://y--
                if (fabs(angleRotationZCamera + M_PI_2) > 0.005)
                {
                    if (angleRotationZCamera <= M_PI_2 && angleRotationZCamera >= -M_PI_2)
                        toucheGaucheAppuyee = true;
                    else
                        toucheDroiteAppuyee = true;
                }
                else
                {
                    angleRotationZCamera = -M_PI_2;
                    toucheGaucheAppuyee = false;
                    toucheDroiteAppuyee = false;
                    toucheHautAppuyee = true;
                }
                break;
            case 1://x--
                if (fabs(angleRotationZCamera - M_PI) > 0.005 && fabs(angleRotationZCamera + M_PI) > 0.005)
                {
                    if (angleRotationZCamera <= M_PI && angleRotationZCamera >= 0)
                        toucheDroiteAppuyee = true;
                    else
                        toucheGaucheAppuyee = true;
                }
                else
                {
                    angleRotationZCamera = M_PI;
                    toucheGaucheAppuyee = false;
                    toucheDroiteAppuyee = false;
                    toucheHautAppuyee = true;
                }
                break;
            case 2://y++
                if (fabs(angleRotationZCamera - M_PI_2) > 0.005)
                {
                    if (angleRotationZCamera <= M_PI_2 && angleRotationZCamera >= -M_PI_2)
                        toucheDroiteAppuyee = true;
                    else
                        toucheGaucheAppuyee = true;
                }
                else
                {
                    angleRotationZCamera = M_PI_2;
                    toucheGaucheAppuyee = false;
                    toucheDroiteAppuyee = false;
                    toucheHautAppuyee = true;
                }
                break;
            case 3://x++
                if (fabs(angleRotationZCamera) > 0.005)
                {
                    if (angleRotationZCamera <= M_PI && angleRotationZCamera >= 0)
                        toucheGaucheAppuyee = true;
                    else
                        toucheDroiteAppuyee = true;
                }
                else
                {
                    angleRotationZCamera = 0;
                    toucheGaucheAppuyee = false;
                    toucheDroiteAppuyee = false;
                    toucheHautAppuyee = true;
                }
                break;
            default:
                break;
        }
        if (routineDeplacement())
            directionAtteinte = true;
        if (directionAtteinte && fabs(xCamera - (GLdouble)(l->getEmplacementXJoueur() + 1.0 / 2.0) * l->getTailleCase().width()) <= 0.01 && fabs(yCamera - (GLdouble)(l->getEmplacementYJoueur() + 1.0 / 2.0) * l->getTailleCase().height()) <= 0.01)
        {
            timerResolution->stop();
            solveLabyrinth();
        }
    }
    else
        timerResolution->stop();
}

void GLLabyrinth::setCamera(GLdouble x, GLdouble y, GLdouble z, GLdouble angleRotationZ)
{
    xCamera = x;
    yCamera = y;
    zCamera = z;
    angleRotationZCamera = angleRotationZ;
}

GLdouble GLLabyrinth::getXCamera() const
{
    return xCamera;
}

GLdouble GLLabyrinth::getYCamera() const
{
    return yCamera;
}

GLdouble GLLabyrinth::getZCamera() const
{
    return zCamera;
}

GLdouble GLLabyrinth::getAngleRotationZCamera() const
{
    return angleRotationZCamera;
}

void GLLabyrinth::setTypeResolution(unsigned int type)
{
    assert(type < 4);

    typeResolution_ = type;
}

unsigned int GLLabyrinth::getTypeResolution() const
{
    return typeResolution_;
}

void GLLabyrinth::mousePressEvent(QMouseEvent* event)
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l && l->enModeEcranDeVeille())
    {
        l->quitterModeEcranDeVeille();
        return;
    }

    QOpenGLWidget::mousePressEvent(event);
}

void GLLabyrinth::mouseMoveEvent(QMouseEvent* event)
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l && l->enModeEcranDeVeille())
    {
        l->quitterModeEcranDeVeille();
        return;
    }

    QOpenGLWidget::mouseMoveEvent(event);
}

void GLLabyrinth::wheelEvent(QWheelEvent* event)
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l && l->enModeEcranDeVeille())
    {
        l->quitterModeEcranDeVeille();
        return;
    }

    QOpenGLWidget::wheelEvent(event);
}

void GLLabyrinth::loadTexture(QImage const& image, GLuint& id)
{
    auto const im{image.mirrored()};

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.width(), im.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, im.constBits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

QOpenGLTexture* GLLabyrinth::loadTexture(QImage const& image)
{
    // Loadimage
    QOpenGLTexture* texture = new QOpenGLTexture(image.mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);

    return texture;
}

void GLLabyrinth::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/resources/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/resources/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void GLLabyrinth::initTextures()
{
    textures_ << loadTexture(pixmapMotifFond.toImage());
    textures_ << loadTexture(pixmapImageFond.toImage());
    textures_ << loadTexture(pixmapMotifMur.toImage());
    textures_ << loadTexture(pixmapImageMur.toImage());
    textures_ << loadTexture(pixmapMotifParcours.toImage());
    textures_ << loadTexture(pixmapImageParcours.toImage());
}
