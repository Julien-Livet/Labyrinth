#include "Labyrinth3d/Qt/GlWidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

GLWidget::GLWidget(Labyrinth3d::Labyrinth& labyrinth,
                   QVector3D const& wallsSize,
                   QVector3D const& waysSize) : QOpenGLWidget(), labyrinth_{labyrinth}
{
    setWallsSize(wallsSize);
    setWaysSize(waysSize);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    for (auto& t : textures_)
    {
        for (unsigned int i(0); i < 6; ++i)
        {
            if (t[i])
                delete t[i];
        }
    }
    delete program;
    doneCurrent();
}

Labyrinth3d::Labyrinth const& GLWidget::labyrinth() const
{
    return labyrinth_;
}

Labyrinth3d::Labyrinth& GLWidget::labyrinth()
{
    return labyrinth_;
}

QVector3D const& GLWidget::wallsSize() const
{
    return wallsSize_;
}
void GLWidget::setWallsSize(QVector3D const& wallsSize)
{
    assert(wallsSize.x() > 0 && wallsSize.y() > 0 && wallsSize.z() > 0);

    wallsSize_ = wallsSize;
}

QVector3D const& GLWidget::waysSize() const
{
    return waysSize_;
}

void GLWidget::setWaysSize(QVector3D const& waysSize)
{
    assert(waysSize.x() > 0 && waysSize.y() > 0 && waysSize.z() > 0);

    waysSize_ = waysSize;
}

void GLWidget::setClearColor(const QColor& color)
{
    clearColor_ = color;
    update();
}

void GLWidget::setTexture(unsigned int i, unsigned int j, const QImage& image)
{
    assert(i < images_.size());
    assert(j < 6);

    images_[i][j] = image;

    if (textures_[i][j])
        delete textures_[i][j];

    textures_[i][j] = new QOpenGLTexture(images_[i][j].mirrored());

    update();
}

const QImage& GLWidget::imageTexture(unsigned int i, unsigned int j) const
{
    assert(i < images_.size());
    assert(j < 6);

    return images_[i][j];
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    for (size_t k{0}; k < 2; ++k)
    {
        for (size_t i{0}; i < 2; ++i)
        {
            for (size_t j{0}; j < 2; ++j)
            {
                makeBox(QVector3D(j % 2 ? waysSize_.x() : wallsSize_.x(),
                                  i % 2 ? waysSize_.y() : wallsSize_.y(),
                                  k % 2 ? waysSize_.z() : wallsSize_.z()));
                for (unsigned int l(0); l < 6; ++l)
                {
                    setTexture(textures_.size() - 1, l, QImage(":/Images/resources/wall_pattern.png"));
                    /*QImage image(128, 128, QImage::Format_ARGB32);
                    //image.fill(Qt::black);
                    image.fill(QColor(255, float(l) / 5 * 255, 0, 255));
                    setTexture(textures_.size() - 1, l, image);*/
                }
            }
        }
    }

    makeBox(QVector3D(1, 1, 1));
    playerBoxId_ = boxes_.size() - 1;

    makeBox(sfRatio_ * waysSize_);
    sfBoxId_ = boxes_.size() - 1;

    for (size_t k{0}; k < 2; ++k)
    {
        for (size_t i{0}; i < 2; ++i)
        {
            for (size_t j{0}; j < 2; ++j)
                makeBox(traceRatio_ * QVector3D(j % 2 ? waysSize_.x() : wallsSize_.x(),
                                                i % 2 ? waysSize_.y() : wallsSize_.y(),
                                                k % 2 ? waysSize_.z() : wallsSize_.z()));
        }
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
}

void GLWidget::paintGL()
{
    glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), clearColor_.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QMatrix4x4 matrix;
    matrix.perspective(70.0, (double)width() / height(), 1.0, 1000.0);

    if (!labyrinth_.playerIds().empty())
    {
        cameraMatrix_ = QMatrix4x4();
        auto const i{labyrinth_.player(firstPlayerId_).i()};
        auto const j{labyrinth_.player(firstPlayerId_).j()};
        auto const k{labyrinth_.player(firstPlayerId_).k()};
        QVector3D const point((j + 1) / 2 * wallsSize_.x() + j / 2 * waysSize_.x() + (j % 2 ? waysSize_.x() : wallsSize_.x()) / 2,
                              (i + 1) / 2 * wallsSize_.y() + i / 2 * waysSize_.y() + (i % 2 ? waysSize_.y() : wallsSize_.y()) / 2,
                              (k + 1) / 2 * wallsSize_.z() + k / 2 * waysSize_.z() + (k % 2 ? waysSize_.z() : wallsSize_.z()) / 2);
        cameraMatrix_.translate(point);
        cameraMatrix_.setColumn(0, rotationMatrix_.column(0));
        cameraMatrix_.setColumn(1, rotationMatrix_.column(1));
        cameraMatrix_.setColumn(2, rotationMatrix_.column(2));

        cameraMatrix_ = matrix;
        cameraMatrix_.lookAt(point,
                             point + rotationMatrix_.column(0).toVector3D(),
                             rotationMatrix_.column(2).toVector3D());
    }

    for (size_t k{0}; k < labyrinth_.grid().depth(); ++k)
    {
        for (size_t i{0}; i < labyrinth_.grid().height(); ++i)
        {
            for (size_t j{0}; j < labyrinth_.grid().width(); ++j)
            {
                if (labyrinth_.grid().at(i, j, k))
                {
                    QMatrix4x4 m{cameraMatrix_};
                    translateModelView(boxSides(((k % 2) * 2 + i % 2) * 2 + j % 2) / 2, m, i, j, k);
                    displayBox(((k % 2) * 2 + i % 2) * 2 + j % 2, m);
                }
            }
        }
    }

    for (size_t m{0}; m < labyrinth_.playerIds().size(); ++m)
    {
        if (m != firstPlayerId_)
            continue;

        auto const playerId{labyrinth_.playerIds()[m]};
        auto const& player{labyrinth_.player(playerId)};
        for (size_t n{0}; n < 6; ++n)
        {
            QImage image(128, 128, QImage::Format_ARGB32);
            image.fill(QColor{0, 255, 0, 63});
            setTexture(sfBoxId_, n, image);
        }
        auto mv{cameraMatrix_};
        translateModelView(boxSides(sfBoxId_) / 2 / sfRatio_, mv, player.startI(), player.startJ(), player.startK());
        displayBox(sfBoxId_, mv);

        for (size_t n{0}; n < player.finishI().size(); ++n)
        {
            mv = cameraMatrix_;
            for (size_t l{0}; l < 6; ++l)
            {
                QImage image(128, 128, QImage::Format_ARGB32);
                image.fill(QColor{255, 0, 0, 63});
                setTexture(sfBoxId_, l, image);
            }
            translateModelView(boxSides(sfBoxId_) / 2 / sfRatio_, mv, player.finishI()[n], player.finishJ()[n], player.finishK()[n]);
            displayBox(sfBoxId_, mv);
        }
    }

    for (auto it{playerDisplays_.begin()}; it != playerDisplays_.end(); ++it)
        displayPlayer(it->first, it->second.color,
                      it->first == firstPlayerId_ ? false : (labyrinth_.player(it->first).i() != labyrinth_.player(firstPlayerId_).i()
                                                             || labyrinth_.player(it->first).j() != labyrinth_.player(firstPlayerId_).j()
                                                             || labyrinth_.player(it->first).k() != labyrinth_.player(firstPlayerId_).k()),
                      it->second.displayTrace, it->second.traceColor);
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GLWidget::makeBox(QVector3D const& bottom, QVector3D const& top, std::array<QImage, 6> const& images)
{
    boxes_.emplace_back(std::make_pair(bottom, top));

    images_.emplace_back(images);
    textures_.emplace_back(std::array<QOpenGLTexture*, 6>{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr});
    for (int j = 0; j < 6; ++j)
        textures_.back()[j] = new QOpenGLTexture(images_.back()[j].mirrored());
}

void GLWidget::changeBox(size_t i, QVector3D const& bottom, QVector3D const& top)
{
    assert(i < boxes_.size());

    boxes_[i].first = bottom;
    boxes_[i].second = top;
}

void GLWidget::changeBoxSides(size_t i, QVector3D const& sides)
{
    changeBox(i, -sides / 2, sides / 2);
}

void GLWidget::bindBox(size_t i)
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

void GLWidget::makeBox(QVector3D const& sides, std::array<QImage, 6> const& images)
{
    assert(sides.x() > 0.0 && sides.y() > 0.0 && sides.z() > 0.0);

    makeBox(-sides / 2, sides / 2, images);
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    QOpenGLWidget::keyPressEvent(event);

    if (labyrinth_.playerIds().empty())
        return;

    if (!event->modifiers().testFlag(Qt::ShiftModifier))
    {
        auto move{
            [this] (QVector4D const& u, Labyrinth3d::Direction d)
            {
                auto& l{this->labyrinth_};
                auto const player1Id{l.playerIds().front()};
                float constexpr epsilon{1e-6};

                switch (d)
                {
                    case Labyrinth3d::Front:
                    case Labyrinth3d::Left:
                    case Labyrinth3d::Up:
                        if ((u - QVector4D(1, 0, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Front);
                        else if ((u + QVector4D(1, 0, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Back);
                        else if ((u - QVector4D(0, 1, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Left);
                        else if ((u + QVector4D(0, 1, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Right);
                        else if ((u - QVector4D(0, 0, 1, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Up);
                        else if ((u + QVector4D(0, 0, 1, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Down);
                        break;

                    case Labyrinth3d::Back:
                    case Labyrinth3d::Right:
                    case Labyrinth3d::Down:
                        if ((u - QVector4D(1, 0, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Back);
                        else if ((u + QVector4D(1, 0, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Front);
                        else if ((u - QVector4D(0, 1, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Right);
                        else if ((u + QVector4D(0, 1, 0, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Left);
                        else if ((u - QVector4D(0, 0, 1, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Down);
                        else if ((u + QVector4D(0, 0, 1, 0)).length() < epsilon)
                            l.player(player1Id).move(Labyrinth3d::Up);
                        break;
                }
            }
        };

        switch (event->key())
        {
            case Qt::Key_Right:
                move(rotationMatrix_.column(1), Labyrinth3d::Right);
                break;
            case Qt::Key_Left:
                move(rotationMatrix_.column(1), Labyrinth3d::Left);
                break;
            case Qt::Key_Down:
                move(rotationMatrix_.column(0), Labyrinth3d::Back);
                break;
            case Qt::Key_Up:
                move(rotationMatrix_.column(0), Labyrinth3d::Front);
                break;
            case Qt::Key_PageDown:
                move(rotationMatrix_.column(2), Labyrinth3d::Down);
                break;
            case Qt::Key_PageUp:
                move(rotationMatrix_.column(2), Labyrinth3d::Up);
                break;
            case Qt::Key_Enter:
                rotationMatrix_.setToIdentity();
                break;
            case Qt::Key_Escape:
                qApp->quit();
                break;
            default:
                break;
        }
    }
    else
    {
        float const stepAngle{90.0 / 2.0};

        switch (event->key())
        {
            case Qt::Key_Right:
                rotationMatrix_.rotate(-stepAngle, QVector3D(0, 0, 1));
                break;
            case Qt::Key_Left:
                rotationMatrix_.rotate(stepAngle, QVector3D(0, 0, 1));
                break;
            case Qt::Key_Down:
                rotationMatrix_.rotate(stepAngle, QVector3D(0, 1, 0));
                break;
            case Qt::Key_Up:
                rotationMatrix_.rotate(-stepAngle, QVector3D(0, 1, 0));
                break;
            case Qt::Key_PageDown:
                rotationMatrix_.rotate(-stepAngle, QVector3D(1, 0, 0));
                break;
            case Qt::Key_PageUp:
                rotationMatrix_.rotate(stepAngle, QVector3D(1, 0, 0));
                break;
            default:
                break;
        }
    }

    update();
}

QMatrix4x4 const& GLWidget::cameraMatrix() const
{
    return cameraMatrix_;
}

void GLWidget::translateModelView(QVector3D const& offset, QMatrix4x4& mv, size_t i, size_t j, size_t k) const
{
    mv.translate((j / 2) * waysSize_.x() + ((j + 1) / 2) * wallsSize_.x() + offset.x(),
                 (i / 2) * waysSize_.y() + ((i + 1) / 2) * wallsSize_.y() + offset.y(),
                 (k / 2) * waysSize_.z() + ((k + 1) / 2) * wallsSize_.z() + offset.z());
}

QVector3D GLWidget::boxSides(size_t i) const
{
    assert(i < boxes_.size());

    return boxes_[i].second - boxes_[i].first;
}

void GLWidget::displayPlayer(size_t playerId, QColor const& color, bool displayPlayer,
                             bool displayTrace, QColor const& traceColor)
{
    auto const& player{labyrinth_.player(playerId)};

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
        translateModelView(boxSides(playerBoxId_) / 2 / pRatio, mv, player.i(), player.j(), player.k());
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
                            setTexture(sfBoxId_ + 1 + (k * 2 + i) * 2 + j, i, image);
                        }
                    }
                }
            }

            for (size_t l{0}; l < player.traceIntersections().size() - 1; ++l)
            {
                size_t i1(std::get<0>(player.traceIntersections()[l]));
                size_t j1(std::get<1>(player.traceIntersections()[l]));
                size_t k1(std::get<2>(player.traceIntersections()[l]));
                size_t const i2(std::get<0>(player.traceIntersections()[l + 1]));
                size_t const j2(std::get<1>(player.traceIntersections()[l + 1]));
                size_t const k2(std::get<2>(player.traceIntersections()[l + 1]));
                long di(i2 - i1);
                long dj(j2 - j1);
                long dk(k2 - k1);

                if (di)
                    di /= std::abs(di);
                if (dj)
                    dj /= std::abs(dj);
                if (dk)
                    dk /= std::abs(dk);

                while (i1 != i2 || j1 != j2 || k1 != k2)
                {
                    auto const index{sfBoxId_ + 1 + ((k1 % 2) * 2 + i1 % 2) * 2 + j1 % 2};
                    auto mv{cameraMatrix_};
                    translateModelView(boxSides(index) / 2 / traceRatio_, mv, i1, j1, k1);
                    displayBox(index, mv);

                    i1 += di;
                    j1 += dj;
                    k1 += dk;
                }
            }

            {
                size_t i1(std::get<0>(player.traceIntersections().back()));
                size_t j1(std::get<1>(player.traceIntersections().back()));
                size_t k1(std::get<2>(player.traceIntersections().back()));
                size_t const i2(player.i());
                size_t const j2(player.j());
                size_t const k2(player.k());
                long di(i2 - i1);
                long dj(j2 - j1);
                long dk(k2 - k1);

                if (di)
                    di /= std::abs(di);
                if (dj)
                    dj /= std::abs(dj);
                if (dk)
                    dk /= std::abs(dk);

                while (i1 != i2 || j1 != j2 || k1 != k2)
                {
                    auto const index{sfBoxId_ + 1 + ((k1 % 2) * 2 + i1 % 2) * 2 + j1 % 2};
                    auto mv{cameraMatrix_};
                    translateModelView(boxSides(index) / 2 / traceRatio_, mv, i1, j1, k1);
                    displayBox(index, mv);

                    i1 += di;
                    j1 += dj;
                    k1 += dk;
                }
            }
        }
    }
}

void GLWidget::displayBox(size_t i, QMatrix4x4 const& mv)
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

GLWidget::PlayerDisplay const& GLWidget::playerDisplay(size_t i) const
{
    return playerDisplays_.at(i);
}

GLWidget::PlayerDisplay& GLWidget::playerDisplay(size_t i)
{
    return playerDisplays_[i];
}
void GLWidget::setPlayerDisplay(size_t i, PlayerDisplay const& playerDisplay)
{
    playerDisplays_[i] = playerDisplay;
}
