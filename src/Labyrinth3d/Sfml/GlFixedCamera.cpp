#include <cassert>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Labyrinth3d/Sfml/GlFixedCamera.h"
#include "Labyrinth3d/Sfml/Utility.h"

GlFixedCamera::GlFixedCamera()
    : projections_{Eigen::Transform<float, 3, Eigen::Projective>::Identity()},
      modelViews_{Eigen::Transform<float, 3, Eigen::Affine>::Identity()}
{
}

GlFixedCamera::GlFixedCamera(Eigen::Transform<float, 3, Eigen::Projective> const& projection,
                             Eigen::Transform<float, 3, Eigen::Affine> const& modelView)
    : GlFixedCamera{}
{
    changeProjection(projection);
    changeModelView(modelView);
}

GlFixedCamera::GlFixedCamera(glm::mat4 const& projection, glm::mat4 const& modelView) : GlFixedCamera{}
{
    changeProjection(projection);
    changeModelView(modelView);
}

Eigen::Transform<float, 3, Eigen::Projective> const& GlFixedCamera::projection() const
{
    return projections_.back();
}

glm::mat4 GlFixedCamera::glmProjection() const
{
    return Utility::eigenToGlm(projections_.back());
}

void GlFixedCamera::changeProjection(Eigen::Transform<float, 3, Eigen::Projective> const& projection)
{
    projections_.back() = projection;
}

void GlFixedCamera::changeProjection(glm::mat4 const& projection)
{
    projections_.back().matrix() = Utility::glmToEigen(projection);
}

Eigen::Transform<float, 3, Eigen::Affine> const& GlFixedCamera::modelView() const
{
    return modelViews_.back();
}

glm::mat4 GlFixedCamera::glmModelView() const
{
    return Utility::eigenToGlm(modelViews_.back());
}

void GlFixedCamera::changeModelView(Eigen::Transform<float, 3, Eigen::Affine> const& modelView)
{
    modelViews_.back() = modelView;
}

void GlFixedCamera::changeModelView(glm::mat4 const& modelView)
{
    modelViews_.back().matrix() = Utility::glmToEigen(modelView);
}

Eigen::Matrix4f GlFixedCamera::view() const
{
    return projections_.back().matrix() * modelViews_.back().matrix();
}

glm::mat4 GlFixedCamera::glmView() const
{
    return Utility::eigenToGlm(view());
}

void GlFixedCamera::push()
{
    projections_.push_back(projections_.back());
    modelViews_.push_back(modelViews_.back());
}

void GlFixedCamera::pop()
{
    if (projections_.size() == 1)
        return;

    projections_.pop_back();
    modelViews_.pop_back();
}

void GlFixedCamera::preTransform(Eigen::Transform<float, 3, Eigen::Affine> const& t)
{
    modelViews_.back() = modelViews_.back() * t;
}

void GlFixedCamera::preTransform(Operation operation, int dx, int dy, float lengthRatio, float angleRatio, bool reverse)
{
    if (!dx && !dy)
        return;

    if (reverse)
        dy = -dy;

    switch (operation)
    {
        case Operation::Pan:
            preTranslate((Eigen::Vector3f{} << static_cast<float>(dx) * lengthRatio,
                                               static_cast<float>(dy) * lengthRatio,
                                               0.0).finished());
            break;

        case Operation::Zoom:
            preTranslate((Eigen::Vector3f{} << 0.0, 0.0, static_cast<float>(dy) * lengthRatio).finished());
            break;

        case Operation::Turn:
            preRotate((Eigen::AngleAxisf{static_cast<float>(-dy) * angleRatio, Eigen::Vector3f::UnitX()}
                       * Eigen::AngleAxisf{static_cast<float>(dx) * angleRatio, Eigen::Vector3f::UnitY()}).toRotationMatrix());
            break;

        case Operation::Rotate:
            preRotate(Eigen::AngleAxisf{static_cast<float>(dy) * angleRatio, Eigen::Vector3f::UnitZ()}.toRotationMatrix());
            break;

        default:
            assert(0 && "Unexpected case!");
    }
}

void GlFixedCamera::postTransform(Eigen::Transform<float, 3, Eigen::Affine> const& t)
{
    modelViews_.back() = t * modelViews_.back();
}

void GlFixedCamera::postTransform(Operation operation, int dx, int dy, float lengthRatio, float angleRatio, bool reverse)
{
    if (!dx && !dy)
        return;

    if (reverse)
        dy = -dy;

    switch (operation)
    {
        case Operation::Pan:
            postTranslate((Eigen::Vector3f{} << static_cast<float>(dx) * lengthRatio,
                                                static_cast<float>(dy) * lengthRatio,
                                                0.0).finished());
            break;

        case Operation::Zoom:
            postTranslate((Eigen::Vector3f{} << 0.0, 0.0, static_cast<float>(dy) * lengthRatio).finished());
            break;

        case Operation::Turn:
            postRotate((Eigen::AngleAxisf{static_cast<float>(-dy) * angleRatio, Eigen::Vector3f::UnitX()}
                        * Eigen::AngleAxisf{static_cast<float>(dx) * angleRatio, Eigen::Vector3f::UnitY()}).toRotationMatrix());
            break;

        case Operation::Rotate:
            postRotate(Eigen::AngleAxisf{static_cast<float>(dy) * angleRatio, Eigen::Vector3f::UnitZ()}.toRotationMatrix());
            break;

        default:
            assert(0 && "Unexpected case!");
    }
}

void GlFixedCamera::preTranslate(Eigen::Vector3f const& t)
{
    modelViews_.back().translation() += t;
}

void GlFixedCamera::postTranslate(Eigen::Vector3f const& t)
{
    modelViews_.back().translation() += modelViews_.back().linear() * t;
}

void GlFixedCamera::preRotate(Eigen::Matrix3f const& r)
{
    modelViews_.back().linear() = r * modelViews_.back().linear();
}

void GlFixedCamera::postRotate(Eigen::Matrix3f const& r)
{
    modelViews_.back().linear() *= r;
}

void GlFixedCamera::lookAt(Eigen::Vector3f const& eye, Eigen::Vector3f const& center, Eigen::Vector3f const& up)
{
    lookAt(glm::vec3{eye[0], eye[1], eye[2]},
           glm::vec3{center[0], center[1], center[2]},
           glm::vec3{up[0], up[1], up[2]});
}

void GlFixedCamera::lookAt(glm::vec3 const& eye, glm::vec3 const& center, glm::vec3 const& up)
{
    modelViews_.back().matrix() = Utility::glmToEigen(glm::lookAt(eye, center, up));
}

void GlFixedCamera::perspective(float fovy, float aspect, float near, float far)
{
    projections_.back().matrix() = Utility::glmToEigen(glm::perspective(fovy, aspect, near, far));
}

void GlFixedCamera::ortho(float left, float right, float top, float bottom)
{
    projections_.back().matrix() = Utility::glmToEigen(glm::ortho(left, right, top, bottom));
}

void GlFixedCamera::ortho(float left, float right, float top, float bottom, float zNear, float zFar)
{
    projections_.back().matrix() = Utility::glmToEigen(glm::ortho(left, right, top, bottom, zNear, zFar));
}

Eigen::Transform<float, 3, Eigen::Affine> GlFixedCamera::interpolate(float t, Eigen::Transform<float, 3, Eigen::Affine> const& otherModelView) const
{
    return Utility::interpolate<float>(t, modelView(), otherModelView);
}

GlFixedCamera& GlFixedCamera::interpolate(float t, GlFixedCamera const& otherCamera)
{
    modelViews_.back() = interpolate(t, otherCamera.modelView());

    return *this;
}

GlFixedCamera GlFixedCamera::interpolate(float t, GlFixedCamera const& otherCamera) const
{
    GlFixedCamera c{*this};

    auto const p{c.projection()};
    auto const m{c.modelView()};
    c.projections_.clear();
    c.modelViews_.clear();
    c.projections_.push_back(p);
    c.modelViews_.push_back(m);
    c.interpolate(t, otherCamera.modelView());

    return c;
}

Eigen::Vector3f GlFixedCamera::direction() const
{
    return -modelView().matrix().block(2, 0, 1, 3).transpose();
}

Eigen::Vector3f GlFixedCamera::eye() const
{
    Eigen::Vector3f v{Eigen::Vector3f::Zero()};

    for (unsigned char i{0}; i < 2; ++i)
        v += modelView().matrix()(i, 3) * modelView().matrix().block(i, 0, 1, 3).transpose();

    v -= modelView().matrix()(2, 3) * modelView().matrix().block(2, 0, 1, 3).transpose();

    return v;
}
