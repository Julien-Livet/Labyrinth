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

    for (size_t k{0}; k < labyrinth_.grid().depth(); ++k)
    {
        for (size_t i{0}; i < labyrinth_.grid().height(); ++i)
        {
            for (size_t j{0}; j < labyrinth_.grid().width(); ++j)
            {
                if (!(i % 2) || !(j % 2) || !(k % 2))
                {
                    QVector3D const bottom{(j + 1) / 2 * wallsSize_.x() + j / 2 * waysSize_.x(),
                                           (i + 1) / 2 * wallsSize_.y() + i / 2 * waysSize_.y(),
                                           (k + 1) / 2 * wallsSize_.z() + k / 2 * waysSize_.z()};
                    makeBox(bottom,
                            bottom + QVector3D(j % 2 ? waysSize_.x() : wallsSize_.x(),
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
    glEnable(GL_CULL_FACE);

    QMatrix4x4 matrix;
    matrix.perspective(70, (double)width() / height(), 0.1, 100.0);
    //matrix.scale(-1, 1, 1);
/*
    matrix.setToIdentity();
    matrix.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    matrix.translate(0.0f, 0.0f, 0.0f);
*/
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
        QMatrix4x4 rotation;
        rotation.rotate(rzCamera_, QVector3D(0, 0, 1));
        rotation.rotate(ryCamera_, QVector3D(0, 1, 0));
        rotation.rotate(rxCamera_, QVector3D(1, 0, 0));/*
        cameraMatrix_.setToIdentity();
        cameraMatrix_.lookAt(point,
                             point + rotation.column(0).toVector3D(),
                             rotation.column(1).toVector3D());*/
    }/*
    //cameraMatrix_.translate(xCamera_, yCamera_, zCamera_);
    cameraMatrix_.rotate(rzCamera_, QVector3D(0, 0, 1));
    cameraMatrix_.rotate(ryCamera_, QVector3D(0, 1, 0));
    cameraMatrix_.rotate(rxCamera_, QVector3D(1, 0, 0));*/
    xCamera_ = 0;
    yCamera_ = 0;
    zCamera_ = 0;
    rxCamera_ = 0;
    ryCamera_ = 0;
    rzCamera_ = 0;

    qDebug() << cameraMatrix_;

    matrix *= cameraMatrix_;

    qDebug() << matrix;

    size_t count{0};

    for (size_t k{0}; k < labyrinth_.grid().depth(); ++k)
    {
        for (size_t i{0}; i < labyrinth_.grid().height(); ++i)
        {
            for (size_t j{0}; j < labyrinth_.grid().width(); ++j)
            {
                if (labyrinth_.grid().at(i, j, k))
                {
                    vbo.bind();
                    program->bind();
                    program->setUniformValue("matrix", matrix);
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

                if (!(i % 2) || !(j % 2) || !(k % 2))
                    ++count;
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

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    QOpenGLWidget::keyPressEvent(event);

    if (labyrinth_.playerIds().empty())
        return;

    if (!event->modifiers().testFlag(Qt::ShiftModifier))
    {
        switch (event->key())
        {
            case Qt::Key_Right:
                labyrinth_.player(labyrinth_.playerIds().front()).move(Labyrinth3d::Right);
                xCamera_ = -1.0e-2;
                break;
            case Qt::Key_Left:
                labyrinth_.player(labyrinth_.playerIds().front()).move(Labyrinth3d::Left);
                xCamera_ = 1.0e-2;
                break;
            case Qt::Key_Down:
                labyrinth_.player(labyrinth_.playerIds().front()).move(Labyrinth3d::Back);
                yCamera_ = -1.0e-2;
                break;
            case Qt::Key_Up:
                labyrinth_.player(labyrinth_.playerIds().front()).move(Labyrinth3d::Front);
                yCamera_ = 1.0e-2;
                break;
            case Qt::Key_PageDown:
                labyrinth_.player(labyrinth_.playerIds().front()).move(Labyrinth3d::Down);
                zCamera_ = 1.0e-2;
                break;
            case Qt::Key_PageUp:
                labyrinth_.player(labyrinth_.playerIds().front()).move(Labyrinth3d::Up);
                zCamera_ = -1.0e-2;
                break;
            default:
                break;
        }
    }
    else
    {
        switch (event->key())
        {
        case Qt::Key_Right:
            ryCamera_ = 1.0;
            ryCamera_ = 90.0;
            rotationMatrix_.rotate(-90.0, QVector3D(0, 1, 0));
            break;
        case Qt::Key_Left:
            ryCamera_ = -1.0;
            ryCamera_ = -90.0;
            rotationMatrix_.rotate(90.0, QVector3D(0, 1, 0));
            break;
        case Qt::Key_Down:
            rxCamera_ = -1.0;
            rxCamera_ = -90.0;
            rotationMatrix_.rotate(-90.0, QVector3D(1, 0, 0));
            break;
        case Qt::Key_Up:
            rxCamera_ = 1.0;
            rxCamera_ = 90.0;
            rotationMatrix_.rotate(90.0, QVector3D(1, 0, 0));
            break;
        case Qt::Key_PageDown:
            rzCamera_ = -1.0;
            rzCamera_ = -90.0;
            rotationMatrix_.rotate(-90.0, QVector3D(0, 0, 1));
            break;
        case Qt::Key_PageUp:
            rzCamera_ = 1.0;
            rzCamera_ = 90.0;
            rotationMatrix_.rotate(90.0, QVector3D(0, 0, 1));
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
