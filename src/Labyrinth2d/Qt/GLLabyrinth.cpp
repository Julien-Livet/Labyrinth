#include <cmath>
#include <random>

#include <GL/glu.h>

#include <QTimer>
#include <QTest>

#include "Labyrinth2d/Qt/GLLabyrinth.h"
#include "Labyrinth2d/Qt/QLabyrinth.h"
#include "Labyrinth2d/Qt/constants.h"
#include "Labyrinth2d/Solver/Solver.h"

GLLabyrinth::GLLabyrinth(QWidget *parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f), QOpenGLFunctions()
{
    labyrinth = 0;

    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
    setMouseTracking(true);
}

GLLabyrinth::~GLLabyrinth()
{
    makeCurrent();
    libererTextures();
    delete program;
    doneCurrent();
}

void GLLabyrinth::setLabyrinth(Labyrinth2d::Labyrinth *l)
{
    labyrinth = l;

    QLabyrinth* lab = qobject_cast<QLabyrinth*>(parentWidget());

    if (!lab)
        return;

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

    wallsSize_.setX(lab->wallsSize().width());
    wallsSize_.setY(lab->wallsSize().height());
    wallsSize_.setZ(qMax(wallsSize_.x(), wallsSize_.y()));
    waysSize_.setX(lab->waysSize().width());
    waysSize_.setY(lab->waysSize().height());
    waysSize_.setZ(qMax(waysSize_.x(), waysSize_.y()));

    for (size_t i{0}; i < 2; ++i)
    {
        for (size_t j{0}; j < 2; ++j)
            changeBoxSides(i * 2 + j, QVector3D(j % 2 ? waysSize_.x() : wallsSize_.x(),
                                                i % 2 ? waysSize_.y() : wallsSize_.y(),
                                                waysSize_.z()));
    }
}

void GLLabyrinth::initializeGL()
{
    initializeOpenGLFunctions();

    for (size_t i{0}; i < 2; ++i)
    {
        for (size_t j{0}; j < 2; ++j)
        {
            makeBox(QVector3D(j % 2 ? waysSize_.x() : wallsSize_.x(),
                              i % 2 ? waysSize_.y() : wallsSize_.y(),
                              waysSize_.z()));
            for (unsigned int l(0); l < 6; ++l)
                setTexture(textures_.size() - 1, l, pixmapMotifMur.toImage());
        }
    }

    makeBox(QVector3D(1, 1, 1));
    playerBoxId_ = boxes_.size() - 1;

    makeBox(sfRatio_ * waysSize_);
    sfBoxId_ = boxes_.size() - 1;

    for (size_t i{0}; i < 2; ++i)
    {
        for (size_t j{0}; j < 2; ++j)
            makeBox(traceRatio_ * QVector3D(j % 2 ? waysSize_.x() : wallsSize_.x(),
                                            i % 2 ? waysSize_.y() : wallsSize_.y(),
                                            waysSize_.z()));
    }

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char* vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char* fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);

    QLabyrinth* lab = qobject_cast<QLabyrinth*>(parentWidget());

    if (!lab)
        return;

    if (lab->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {
        glEnable(GL_FOG);
    }
    else
    {
        glDisable(GL_FOG);
    }
}

void GLLabyrinth::paintGL()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    QLabyrinth::Texture const textures[3] = {l->getTextureFond(), l->getTextureMur(), l->getTextureParcours()};

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (l->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
    {
        QColor c = l->getModeLabyrinthe().couleurObscurite;
        glClearColor(c.redF(), c.greenF(), c.blueF(), c.alphaF());
    }
    else
        glClearColor(textures[0].couleur.redF(), textures[0].couleur.greenF(), textures[0].couleur.blueF(), textures[0].couleur.alphaF());

    QMatrix4x4 matrix;
    matrix.perspective(70.0, (double)width() / height(), 1.0, 1000.0);

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

    auto const playerId{labyrinth->playerIds().front()};

    if (!labyrinth->playerIds().empty())
    {
        cameraMatrix_ = QMatrix4x4();
        auto const i{labyrinth->player(playerId).i()};
        auto const j{labyrinth->player(playerId).j()};
        QVector3D const point((j + 1) / 2 * wallsSize_.x() + j / 2 * waysSize_.x() + (j % 2 ? waysSize_.x() : wallsSize_.x()) / 2,
                              (i + 1) / 2 * wallsSize_.y() + i / 2 * waysSize_.y() + (i % 2 ? waysSize_.y() : wallsSize_.y()) / 2,
                              waysSize_.z() / 2);
        cameraMatrix_.translate(point);
        cameraMatrix_.setColumn(0, rotationMatrix_.column(0));
        cameraMatrix_.setColumn(1, rotationMatrix_.column(1));
        cameraMatrix_.setColumn(2, rotationMatrix_.column(2));

        cameraMatrix_ = matrix;
        cameraMatrix_.lookAt(point,
                             point + rotationMatrix_.column(0).toVector3D(),
                             rotationMatrix_.column(2).toVector3D());
    }

    for (size_t i{0}; i < labyrinth->grid().height(); ++i)
    {
        for (size_t j{0}; j < labyrinth->grid().width(); ++j)
        {
            if (labyrinth->grid().at(i, j))
            {
                QMatrix4x4 m{cameraMatrix_};
                translateModelView(boxSides((i % 2) * 2 + j % 2) / 2, m, i, j);
                displayBox((i % 2) * 2 + j % 2, m);
            }
        }
    }

    for (size_t m{0}; m < 1; ++m)
    {
        auto const playerId{labyrinth->playerIds()[m]};
        auto const& player{labyrinth->player(playerId)};
        for (size_t n{0}; n < 6; ++n)
        {
            QImage image(128, 128, QImage::Format_ARGB32);
            image.fill(QColor{0, 255, 0, 63});
            setTexture(sfBoxId_, n, image);
        }
        auto mv{cameraMatrix_};
        translateModelView(boxSides(sfBoxId_) / 2 / sfRatio_, mv, player.startI(), player.startJ());
        displayBox(sfBoxId_, mv);

        for (size_t n{0}; n < player.finishI().size(); ++n)
        {
            mv = cameraMatrix_;
            for (size_t o{0}; o < 6; ++o)
            {
                QImage image(128, 128, QImage::Format_ARGB32);
                image.fill(QColor{255, 0, 0, 63});
                setTexture(sfBoxId_, o, image);
            }
            translateModelView(boxSides(sfBoxId_) / 2 / sfRatio_, mv, player.finishI()[n], player.finishJ()[n]);
            displayBox(sfBoxId_, mv);
        }
    }

    QColor traceColor{l->getTextureParcours().couleur};
    traceColor.setAlphaF(0.5);
    displayPlayer(playerId, l->getTextureParcours().couleur, false, l->getAfficherTrace(), traceColor);
}

void GLLabyrinth::resizeGL(int width, int height)
{
    int const side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

}

void GLLabyrinth::keyPressEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyPressEvent(event);

    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    if (l->enModeEcranDeVeille())
        l->quitterModeEcranDeVeille();

    if (l->getPartieEnPause() || (l->getEmplacementXJoueur() == l->getXSortie() && l->getEmplacementYJoueur() == l->getYSortie()) || l->getEnResolution())
        return;

    if (labyrinth->playerIds().empty())
        return;

    auto const i{labyrinth->player(labyrinth->playerIds().front()).i()};
    auto const j{labyrinth->player(labyrinth->playerIds().front()).j()};

    if (!event->modifiers().testFlag(Qt::ShiftModifier))
    {
        auto move{
            [this, l] (QVector4D const& u, Labyrinth2d::Direction d)
            {
                auto const player1Id{labyrinth->playerIds().front()};
                float constexpr epsilon{1e-6};

                switch (d)
                {
                    case Labyrinth2d::Left:
                    case Labyrinth2d::Up:
                        if ((u - QVector4D(1, 0, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Right);
                        else if ((u + QVector4D(1, 0, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Left);
                        else if ((u - QVector4D(0, 1, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Down);
                        else if ((u + QVector4D(0, 1, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Up);
                        break;

                    case Labyrinth2d::Right:
                    case Labyrinth2d::Down:
                        if ((u - QVector4D(1, 0, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Left);
                        else if ((u + QVector4D(1, 0, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Right);
                        else if ((u - QVector4D(0, 1, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Up);
                        else if ((u + QVector4D(0, 1, 0, 0)).length() < epsilon)
                            labyrinth->player(player1Id).move(Labyrinth2d::Down);
                        break;
                }
            }
        };

        switch (event->key())
        {
            case Qt::Key_Right:
                move(rotationMatrix_.column(1), Labyrinth2d::Right);
                break;
            case Qt::Key_Left:
                move(rotationMatrix_.column(1), Labyrinth2d::Left);
                break;
            case Qt::Key_Down:
                move(rotationMatrix_.column(0), Labyrinth2d::Down);
                break;
            case Qt::Key_Up:
                move(rotationMatrix_.column(0), Labyrinth2d::Up);
                break;
            default:
                break;
        }
    }
    else
    {
        float constexpr stepAngle{90.0 / 2.0};

        switch (event->key())
        {
            case Qt::Key_Right:
                rotationMatrix_.rotate(-stepAngle, QVector3D(0, 0, 1));
                break;
            case Qt::Key_Left:
                rotationMatrix_.rotate(stepAngle, QVector3D(0, 0, 1));
                break;
            default:
                break;
        }
    }

    emit deplacementJoueur(labyrinth->player(labyrinth->playerIds().front()).j() - j,
                           labyrinth->player(labyrinth->playerIds().front()).i() - i);

    update();
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

    for (size_t i{0}; i < boxes_.size(); ++i)
    {
        for (size_t j{0}; j < boxes_.size(); ++j)
            setTexture(i, j, pixmapMotifMur.toImage().mirrored());
    }
}

void GLLabyrinth::libererTextures()
{
    for (auto& t : textures_)
    {
        for (unsigned int i(0); i < 6; ++i)
        {
            if (t[i])
                delete t[i];
        }
    }
    textures_.clear();
}

void GLLabyrinth::solveLabyrinth()
{
    QLabyrinth* l = qobject_cast<QLabyrinth*>(parentWidget());

    if (!l)
        return;

    int const emplacementXJoueur = l->getEmplacementXJoueur();
    int const emplacementYJoueur = l->getEmplacementYJoueur();
    int const xSortie = l->getXSortie();
    int const ySortie = l->getYSortie();

    if (emplacementXJoueur == xSortie && emplacementYJoueur == ySortie)
    {
        emit deplacementJoueur(0, 0);

        update();

        return;
    }

    size_t const seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine g(seed);
    //std::random_device g;

    auto const sleep{
        [this] (std::chrono::milliseconds const& ms) -> void
        {
            QTest::qWait(ms);
            this->update();
        }
    };

    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(100 * 50);

    auto const playerId{labyrinth->playerIds().front()};

    if (typeResolution_ == 0)
    {
        Labyrinth2d::Solver::AStar ass;
        labyrinth->player(playerId).solve(g, ass, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
    else if (typeResolution_ == 1)
    {
        Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};
        labyrinth->player(playerId).solve(g, whs, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
    else if (typeResolution_ == 2)
    {
        Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};
        labyrinth->player(playerId).solve(g, whs, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
    else //if (typeResolution_ == 3)
    {
        Labyrinth2d::Solver::Blind bs;
        labyrinth->player(playerId).solve(g, bs, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
}

void GLLabyrinth::arreterResolution()
{
    labyrinth->player(labyrinth->playerIds().front()).stopSolving();
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

void GLLabyrinth::translateModelView(QVector3D const& offset, QMatrix4x4& mv, size_t i, size_t j) const
{
    mv.translate((j / 2) * waysSize_.x() + ((j + 1) / 2) * wallsSize_.x() + offset.x(),
                 (i / 2) * waysSize_.y() + ((i + 1) / 2) * wallsSize_.y() + offset.y(),
                 offset.z());
}

QVector3D GLLabyrinth::boxSides(size_t i) const
{
    assert(i < boxes_.size());

    return boxes_[i].second - boxes_[i].first;
}

void GLLabyrinth::displayPlayer(size_t playerId, QColor const& color, bool displayPlayer,
                                bool displayTrace, QColor const& traceColor)
{
    auto const& player{labyrinth->player(playerId)};

    float constexpr pRatio{0.25};
    changeBoxSides(playerBoxId_, pRatio * waysSize_);

    if (displayPlayer)
    {
        for (size_t i{0}; i < 6; ++i)
        {
            QImage image(128, 128, QImage::Format_ARGB32);
            image.fill(color);
            setTexture(playerBoxId_, i, image);
        }

        auto mv{cameraMatrix_};
        translateModelView(boxSides(playerBoxId_) / 2 / pRatio, mv, player.i(), player.j());
        displayBox(playerBoxId_, mv);
    }

    if (displayTrace)
    {
        if (!player.traceIntersections().empty())
        {
            for (size_t k{0}; k < 2; ++k)
            {
                for (size_t i{0}; i < 2; ++i)
                {
                    for (size_t j{0}; j < 2; ++j)
                    {
                        for (size_t l{0}; l < 6; ++l)
                        {
                            QImage image(128, 128, QImage::Format_ARGB32);
                            image.fill(traceColor);
                            setTexture(sfBoxId_ + 1 + i * 2 + j, i, image);
                        }
                    }
                }
            }

            for (size_t l{0}; l < player.traceIntersections().size() - 1; ++l)
            {
                size_t i1(player.traceIntersections()[l].first);
                size_t j1(player.traceIntersections()[l].second);
                size_t const i2(player.traceIntersections()[l + 1].first);
                size_t const j2(player.traceIntersections()[l + 1].second);
                long di(i2 - i1);
                long dj(j2 - j1);

                if (di)
                    di /= std::abs(di);
                if (dj)
                    dj /= std::abs(dj);

                while (i1 != i2 || j1 != j2)
                {
                    auto const index{sfBoxId_ + 1 + (i1 % 2) * 2 + j1 % 2};
                    auto mv{cameraMatrix_};
                    translateModelView(boxSides(index) / 2 / traceRatio_, mv, i1, j1);
                    displayBox(index, mv);

                    i1 += di;
                    j1 += dj;
                }
            }

            {
                size_t i1(player.traceIntersections().back().first);
                size_t j1(player.traceIntersections().back().second);
                size_t const i2(player.i());
                size_t const j2(player.j());
                long di(i2 - i1);
                long dj(j2 - j1);

                if (di)
                    di /= std::abs(di);
                if (dj)
                    dj /= std::abs(dj);

                while (i1 != i2 || j1 != j2)
                {
                    auto const index{sfBoxId_ + 1 + (i1 % 2) * 2 + j1 % 2};
                    auto mv{cameraMatrix_};
                    translateModelView(boxSides(index) / 2 / traceRatio_, mv, i1, j1);
                    displayBox(index, mv);

                    i1 += di;
                    j1 += dj;
                }
            }
        }
    }
}

void GLLabyrinth::displayBox(size_t i, QMatrix4x4 const& mv)
{
    assert(i < boxes_.size());

    bindBox(i);

    vbo.bind();
    program->bind();
    program->setUniformValue("matrix", mv);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    for (int l = 0; l < 6; ++l)
    {
        textures_[i][l]->bind();
        glDrawArrays(GL_TRIANGLE_FAN, l * 4, 4);
    }
}

void GLLabyrinth::makeBox(QVector3D const& bottom, QVector3D const& top, std::array<QImage, 6> const& images)
{
    boxes_.emplace_back(std::make_pair(bottom, top));

    images_.emplace_back(images);
    textures_.emplace_back(std::array<QOpenGLTexture*, 6>{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr});
    for (int j = 0; j < 6; ++j)
        textures_.back()[j] = new QOpenGLTexture(images_.back()[j].mirrored());
}

void GLLabyrinth::changeBox(size_t i, QVector3D const& bottom, QVector3D const& top)
{
    assert(i < boxes_.size());

    boxes_[i].first = bottom;
    boxes_[i].second = top;
}

void GLLabyrinth::changeBoxSides(size_t i, QVector3D const& sides)
{
    changeBox(i, -sides / 2, sides / 2);
}

void GLLabyrinth::bindBox(size_t i)
{
    assert(i < boxes_.size());

    auto const& bottom{boxes_[i].first};
    auto const& top{boxes_[i].second};

    std::array<std::array<std::array<float, 3>, 4>, 6> const coords{
        std::array<std::array<float, 3>, 4>{
            std::array<float, 3>{top.x(), bottom.y(), bottom.z()},
            std::array<float, 3>{bottom.x(), bottom.y(), bottom.z()},
            std::array<float, 3>{bottom.x(), top.y(), bottom.z()},
            std::array<float, 3>{top.x(), top.y(), bottom.z()}
        },
        std::array<std::array<float, 3>, 4>{
            std::array<float, 3>{top.x(), top.y(), bottom.z()},
            std::array<float, 3>{bottom.x(), top.y(), bottom.z()},
            std::array<float, 3>{bottom.x(), top.y(), top.z()},
            std::array<float, 3>{top.x(), top.y(), top.z()}
        },
        std::array<std::array<float, 3>, 4>{
            std::array<float, 3>{top.x(), bottom.y(), top.z()},
            std::array<float, 3>{top.x(), bottom.y(), bottom.z()},
            std::array<float, 3>{top.x(), top.y(), bottom.z()},
            std::array<float, 3>{top.x(), top.y(), top.z()}
        },
        std::array<std::array<float, 3>, 4>{
            std::array<float, 3>{bottom.x(), bottom.y(), bottom.z()},
            std::array<float, 3>{bottom.x(), bottom.y(), top.z()},
            std::array<float, 3>{bottom.x(), top.y(), top.z()},
            std::array<float, 3>{bottom.x(), top.y(), bottom.z()}
        },
        std::array<std::array<float, 3>, 4>{
            std::array<float, 3>{top.x(), bottom.y(), top.z()},
            std::array<float, 3>{bottom.x(), bottom.y(), top.z()},
            std::array<float, 3>{bottom.x(), bottom.y(), bottom.z()},
            std::array<float, 3>{top.x(), bottom.y(), bottom.z()}
        },
        std::array<std::array<float, 3>, 4>{
            std::array<float, 3>{bottom.x(), bottom.y(), top.z()},
            std::array<float, 3>{top.x(), bottom.y(), top.z()},
            std::array<float, 3>{top.x(), top.y(), top.z()},
            std::array<float, 3>{bottom.x(), top.y(), top.z()}
        }
    };

    QList<GLfloat> vertData;

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            // vertex position
            vertData.append(coords[i][j][0]);
            vertData.append(coords[i][j][1]);
            vertData.append(coords[i][j][2]);
            // texture coordinate
            vertData.append(j == 0 || j == 3);
            vertData.append(j == 0 || j == 1);
        }
    }

    if (vbo.isCreated())
        vbo.destroy();
    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

void GLLabyrinth::makeBox(QVector3D const& sides, std::array<QImage, 6> const& images)
{
    assert(sides.x() > 0.0 && sides.y() > 0.0 && sides.z() > 0.0);

    makeBox(-sides / 2, sides / 2, images);
}

void GLLabyrinth::setTexture(unsigned int i, unsigned int j, const QImage& image)
{
    assert(i < images_.size());
    assert(j < 6);

    images_[i][j] = image;

    if (textures_[i][j])
        delete textures_[i][j];

    textures_[i][j] = new QOpenGLTexture(images_[i][j].mirrored());

    update();
}

const QImage& GLLabyrinth::imageTexture(unsigned int i, unsigned int j) const
{
    assert(i < images_.size());
    assert(j < 6);

    return images_[i][j];
}

void GLLabyrinth::resolutionLabyrinthe()
{
    solveLabyrinth();
}
