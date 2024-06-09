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
                    //setTexture(textures_.size() - 1, l, QImage("C:/Users/juju0/Documents/GitHub/Labyrinth/resources/wall_pattern.png"));
                    QImage image(128, 128, QImage::Format_ARGB32);
                    //image.fill(Qt::black);
                    image.fill(QColor(255, float(l) / 5 * 255, 0, 255));
                    setTexture(textures_.size() - 1, l, image);
                }
            }
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
        auto const i{labyrinth_.player(labyrinth_.playerIds().front()).i()};
        auto const j{labyrinth_.player(labyrinth_.playerIds().front()).j()};
        auto const k{labyrinth_.player(labyrinth_.playerIds().front()).k()};
        QVector3D const point((j + 1) / 2 * wallsSize_.x() + j / 2 * waysSize_.x() + (j % 2 ? waysSize_.x() : wallsSize_.x()) / 2,
                              (i + 1) / 2 * wallsSize_.y() + i / 2 * waysSize_.y() + (i % 2 ? waysSize_.y() : wallsSize_.y()) / 2,
                              (k + 1) / 2 * wallsSize_.z() + k / 2 * waysSize_.z() + (k % 2 ? waysSize_.z() : wallsSize_.z()) / 2);
        cameraMatrix_.translate(point);
        cameraMatrix_.setColumn(0, rotationMatrix_.column(0));
        cameraMatrix_.setColumn(1, rotationMatrix_.column(1));
        cameraMatrix_.setColumn(2, rotationMatrix_.column(2));

        qDebug() << "hop";
        qDebug() << cameraMatrix_;
        cameraMatrix_ = matrix;
        cameraMatrix_.lookAt(point,
                             point + rotationMatrix_.column(0).toVector3D(),
                             rotationMatrix_.column(2).toVector3D());

        QMatrix4x4 m;
        m.lookAt(QVector3D(0, 0, 0), QVector3D(1, 0, 0), QVector3D(0, 0, 1));
        qDebug() << m;
    }

    qDebug() << matrix;
    qDebug() << cameraMatrix_;

    for (size_t k{0}; k < labyrinth_.grid().depth(); ++k)
    {
        for (size_t i{0}; i < labyrinth_.grid().height(); ++i)
        {
            for (size_t j{0}; j < labyrinth_.grid().width(); ++j)
            {
                if (labyrinth_.grid().at(i, j, k))
                {
                    QMatrix4x4 m{cameraMatrix_};
                    translateModelView(sidesBox(((k % 2) * 2 + i % 2) * 2 + j % 2) / 2, m, i, j, k);

                    vbo.bind();
                    program->bind();
                    program->setUniformValue("matrix", matrix * m);
                    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
                    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
                    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
                    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

                    for (int l = 0; l < 6; ++l)
                    {
                        //textures_[count][l]->bind();
                        textures_.front().front()->bind();
                        glDrawArrays(GL_TRIANGLE_FAN, l * 4, 4);
                    }
                }
            }
        }
    }
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GLWidget::makeBox(QVector3D const& bottom, QVector3D const& top, std::array<QImage, 6> const& images)
{
    boxes_.emplace_back(std::make_pair(bottom, top));

    coords_.emplace_back(
    std::array<std::array<std::array<float, 3>, 4>, 6>{
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
    });

    images_.emplace_back(images);
    textures_.emplace_back(std::array<QOpenGLTexture*, 6>{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr});
    for (int j = 0; j < 6; ++j)
        textures_.back()[j] = new QOpenGLTexture(images_.back()[j].mirrored());

    QList<GLfloat> vertData;
    for (auto const& coords : coords_)
    {
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

                switch (d)
                {
                    case Labyrinth3d::Front:
                    case Labyrinth3d::Left:
                    case Labyrinth3d::Up:
                        if ((u - QVector4D(1, 0, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Front);
                        else if ((u + QVector4D(1, 0, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Back);
                        else if ((u - QVector4D(0, 1, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Left);
                        else if ((u + QVector4D(0, 1, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Right);
                        else if ((u - QVector4D(0, 0, 1, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Up);
                        else if ((u + QVector4D(0, 0, 1, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Down);
                        break;

                    case Labyrinth3d::Back:
                    case Labyrinth3d::Right:
                    case Labyrinth3d::Down:
                        if ((u - QVector4D(1, 0, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Back);
                        else if ((u + QVector4D(1, 0, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Front);
                        else if ((u - QVector4D(0, 1, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Right);
                        else if ((u + QVector4D(0, 1, 0, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Left);
                        else if ((u - QVector4D(0, 0, 1, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Down);
                        else if ((u + QVector4D(0, 0, 1, 0)).isNull())
                            l.player(player1Id).move(Labyrinth3d::Up);
                        break;
                }
            }
        };

        switch (event->key())
        {
            case Qt::Key_Right:
                move(rotationMatrix_.column(0), Labyrinth3d::Right);
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

void GLWidget::setCameraMatrix(QMatrix4x4 const& matrix)
{
    cameraMatrix_ = matrix;
}

void GLWidget::translateModelView(QVector3D const& offset, QMatrix4x4& mv, size_t i, size_t j, size_t k) const
{
    mv.translate((j / 2) * waysSize_.x() + ((j + 1) / 2) * wallsSize_.x() + offset.x(),
                 (i / 2) * waysSize_.y() + ((i + 1) / 2) * wallsSize_.y() + offset.y(),
                 (k / 2) * waysSize_.z() + ((k + 1) / 2) * wallsSize_.z() + offset.z());
}

QVector3D GLWidget::sidesBox(size_t i) const
{
    assert(i < boxes_.size());

    return boxes_[i].second - boxes_[i].first;
}
