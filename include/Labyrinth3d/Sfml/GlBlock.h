#ifndef GLBLOCK_H
#define GLBLOCK_H

/*!
 *  \file GLBlock.h
 *  \brief Use a 3D Open GL block
 *  \author Julien LIVET
 *  \version 1.2
 *  \date 25/02/2018
 */

#include <array>
#include <functional>

#include <Eigen/Geometry>

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Labyrinth3d/Sfml/openGl.h"
#include "Labyrinth3d/Sfml/GlBuffer.h"
#include "Labyrinth3d/Sfml/GlVertexArray.h"

class Shader;

class GlBlock
{
    public:
        explicit GlBlock(Eigen::Vector3f const& sides, Eigen::Transform<float, 3, Eigen::Affine> const& position = Eigen::Transform<float, 3, Eigen::Affine>::Identity());
        GlBlock(GlBlock const&) = default;
        GlBlock(GlBlock&&) = default;
        ~GlBlock() = default;
        Eigen::Transform<float, 3, Eigen::Affine> const& position() const;
        Eigen::Transform<float, 3, Eigen::Affine>& position();
        Eigen::Vector3f const& sides() const;
        void changeSides(Eigen::Vector3f const& sides);
        void changeSide(unsigned char i, float side);
        void display(glm::mat4 const& projection, glm::mat4 const& modelView,
                     sf::Shader const& colorShader) const;
        void display(glm::mat4 const& projection, glm::mat4 const& modelView,
                     sf::Shader const& textureShader, std::array<std::reference_wrapper<const sf::Texture>, 6> const& facetTextures) const;
        GlBlock& operator=(GlBlock const&) = default;
        std::array<float, 6 * 2 * 3 * 3> const& vertices() const;
        void changeFacetColors(std::array<float, 6 * 4> const& colors);

    private:
        Eigen::Transform<float, 3, Eigen::Affine> position_;
        Eigen::Vector3f sides_;
        mutable std::array<float, 6 * 2 * 3 * 3> vertices_;
        mutable bool verticesUpdated_;
        std::array<float, 6 * 2 * 3 * 4> colors_;
        std::array<float, 6 * 2 * 3 * 2> textureVertices_;
        mutable Eigen::Transform<float, 3, Eigen::Affine> previousPosition_;
        GlVertexArray glVertexArrayColors_;
        GlVertexArray glVertexArrayTexturesVertices_;
        mutable GlBuffer glBufferVertices_;
        mutable GlBuffer glBufferColors_;
        mutable GlBuffer glBufferTextureVertices_;

        void updateVertices_() const;
};

#endif // GLBLOCK_H
