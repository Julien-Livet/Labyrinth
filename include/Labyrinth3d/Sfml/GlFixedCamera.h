#ifndef GLFIXEDCAMERA_H
#define GLFIXEDCAMERA_H

/*!
 *  \file GlFixedCamera.h
 *  \brief Use a fixed GL camera (the aimed object stays fixed by linear part [rotation] manipulations)
 *  \author Julien LIVET
 *  \version 1.1
 *  \date 26/02/2018
 */

#include <vector>

#include <Eigen/Geometry>

#include <glm/glm.hpp>

#include "openGl.h"

class GlFixedCamera
{
    public:
        enum class Operation : char
        {
            Pan = 0x0,
            Zoom = 0x1,
            Turn = 0x2,
            Rotate = 0x3
        };

        explicit GlFixedCamera(Eigen::Transform<float, 3, Eigen::Projective> const& projection,
                               Eigen::Transform<float, 3, Eigen::Affine> const& modelView);
        explicit GlFixedCamera(glm::mat4 const& projection, glm::mat4 const& modelView);
        Eigen::Transform<float, 3, Eigen::Projective> const& projection() const;
        glm::mat4 glmProjection() const;
        void changeProjection(Eigen::Transform<float, 3, Eigen::Projective> const& projection);
        void changeProjection(glm::mat4 const& projection);
        Eigen::Transform<float, 3, Eigen::Affine> const& modelView() const;
        glm::mat4 glmModelView() const;
        void changeModelView(Eigen::Transform<float, 3, Eigen::Affine> const& modelView);
        void changeModelView(glm::mat4 const& modelView);
        Eigen::Matrix4f view() const;
        glm::mat4 glmView() const;
        void push();
        void pop();
        void preTransform(Eigen::Transform<float, 3, Eigen::Affine> const& t);
        void preTransform(Operation operation, int dx, int dy, float lengthRatio, float angleRatio, bool reverse = false);
        void postTransform(Eigen::Transform<float, 3, Eigen::Affine> const& t);
        void postTransform(Operation operation, int dx, int dy, float lengthRatio, float angleRatio, bool reverse = false);
        void preTranslate(Eigen::Vector3f const& t);
        void postTranslate(Eigen::Vector3f const& t);
        void preRotate(Eigen::Matrix3f const& r);
        void postRotate(Eigen::Matrix3f const& r);
        void lookAt(Eigen::Vector3f const& eye, Eigen::Vector3f const& center, Eigen::Vector3f const& up);
        void lookAt(glm::vec3 const& eye, glm::vec3 const& center, glm::vec3 const& up);
        void perspective(float fovy, float aspect, float near, float far);
        void ortho(float left, float right, float top, float bottom);
        void ortho(float left, float right, float top, float bottom, float zNear, float zFar);
        Eigen::Transform<float, 3, Eigen::Affine> interpolate(float t, Eigen::Transform<float, 3, Eigen::Affine> const& otherModelView) const;
        GlFixedCamera& interpolate(float t, GlFixedCamera const& otherCamera);
        GlFixedCamera interpolate(float t, GlFixedCamera const& otherCamera) const;
        Eigen::Vector3f direction() const;
        Eigen::Vector3f eye() const; //not accurate

    private:
        GlFixedCamera();
        std::vector<Eigen::Transform<float, 3, Eigen::Projective>,
                    Eigen::aligned_allocator<Eigen::Transform<float, 3, Eigen::Projective> > > projections_;
        std::vector<Eigen::Transform<float, 3, Eigen::Affine>,
                    Eigen::aligned_allocator<Eigen::Transform<float, 3, Eigen::Affine> > > modelViews_;
};

#endif // GLFIXEDCAMERA_H
