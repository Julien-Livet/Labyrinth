#include <cassert>

#include <Eigen/Geometry>

#include "Labyrinth3d/Sfml/GlBlock.h"
#include "Labyrinth3d/Sfml/Utility.h"

GlBlock::GlBlock(Eigen::Vector3f const& sides,
             Eigen::Transform<float, 3, Eigen::Affine> const& position)
 : position_{position}, sides_{}, vertices_{}, verticesUpdated_{false}, colors_{},
   textureVertices_{1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                    0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                    1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                    0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                    1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                    0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                    1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                    0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                    1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                    0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
                    1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                    0.0, 1.0, 0.0, 0.0, 1.0, 0.0},
   glBufferVertices_{GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW},
   glBufferColors_{GL_ARRAY_BUFFER, colors_.size() * sizeof(float), colors_.data(), GL_STATIC_DRAW},
   glBufferTextureVertices_{GL_ARRAY_BUFFER, textureVertices_.size() * sizeof(float), textureVertices_.data(), GL_STATIC_DRAW},
   glVertexArrayColors_{},
   glVertexArrayTexturesVertices_{}
{
    changeSides(sides);
    colors_.fill(0);

    GLuint constexpr verticesIndex{0};
    GLuint constexpr colorsIndex{1};
    GLuint constexpr textureVerticesIndex{1};

    {
        glVertexArrayColors_.bind();

        glBufferVertices_.bind();
        glVertexAttribPointer(verticesIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(verticesIndex);
        glBufferVertices_.unbind();

        glBufferColors_.bind();
        glVertexAttribPointer(colorsIndex, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(colorsIndex);
        glBufferColors_.unbind();

        glVertexArrayColors_.unbind();
    }

    {
        glVertexArrayTexturesVertices_.bind();

        glBufferVertices_.bind();
        glVertexAttribPointer(verticesIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(verticesIndex);
        glBufferVertices_.unbind();

        glBufferTextureVertices_.bind();
        glVertexAttribPointer(textureVerticesIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(textureVerticesIndex);
        glBufferTextureVertices_.unbind();

        glVertexArrayTexturesVertices_.unbind();
    }
}

Eigen::Transform<float, 3, Eigen::Affine> const& GlBlock::position() const
{
    return position_;
}

Eigen::Transform<float, 3, Eigen::Affine>& GlBlock::position()
{
    return position_;
}

Eigen::Vector3f const& GlBlock::sides() const
{
    return sides_;
}

void GlBlock::changeSides(Eigen::Vector3f const& sides)
{
    for (unsigned char i{0}; i <3; ++i)
        assert(sides[i] >= 0.0);

    sides_ = sides;

    verticesUpdated_ = false;
}

void GlBlock::changeSide(unsigned char i, float side)
{
    assert(i < 3);
    assert(side >= 0.0);

    sides_[i] = side;

    verticesUpdated_ = false;
}

void GlBlock::updateVertices_() const
{
    if (!previousPosition_.isApprox(position_))
        verticesUpdated_ = false;

    if (verticesUpdated_)
        return;
/*
    unsigned char m{0};

    //Facets 0 and 1
    {
        std::array<Eigen::Vector3f, 6> u{};

        u[0] << sides_[0], sides_[1], -sides_[2];
        u[1] << sides_[0], sides_[1], sides_[2];
        u[2] << sides_[0], -sides_[1], -sides_[2];
        u[3] = u[2];
        u[4] << sides_[0], -sides_[1], -sides_[2];
        u[5] = u[0];

        auto p{position_};

        //p.linear() *= ;

        float angle{0.0};

        for (unsigned char n{0}; ++n; n < 2)
        {
            p.linear() *= Eigen::AngleAxis{angle, Eigen::Vector3f::UnitZ()}.toRotationMatrix();

            for (unsigned char i{0}; i < 6; ++i)
            {
                auto const v{p * u[i]};

                for (unsigned char j{0}; j < 3; ++j)
                    vertices_[(m * 6 + i) * 3 + j] = v[j];
            }

            angle += Utility::Float::pi();

            ++m;
        }
    }

    //Facets 2 and 3
    {
        std::array<Eigen::Vector3f, 6> u{};

        u[0] << -sides_[0], sides_[1], sides_[2];
        u[1] << sides_[0], sides_[1], sides_[2];
        u[2] << sides_[0], sides_[1], -sides_[2];
        u[3] = u[2];
        u[4] << -sides_[0], sides_[1], -sides_[2];
        u[5] = u[0];

        auto p{position_};

        p.linear() *= (Eigen::AngleAxisf{-Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitZ()}
                       * Eigen::AngleAxisf{-Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitY()}).toRotationMatrix();

        float angle{0.0};

        for (unsigned char n{0}; ++n; n < 2)
        {
            p.linear() *= Eigen::AngleAxis{angle, Eigen::Vector3f::UnitX()}.toRotationMatrix();

            for (unsigned char i{0}; i < 6; ++i)
            {
                auto const v{p * u[i]};

                for (unsigned char j{0}; j < 3; ++j)
                    vertices_[(m * 6 + i) * 3 + j] = v[j];
            }

            angle += Utility::Float::pi();

            ++m;
        }
    }

    //Facets 4 and 5
    {
        std::array<Eigen::Vector3f, 6> u{};

        u[0] << sides_[0], -sides_[1], sides_[2];
        u[1] << sides_[0], sides_[1], sides[2];
        u[2] << -sides_[0], sides_[1], sides_[2];
        u[3] = u[2];
        u[4] << -sides_[0], -sides_[1], sides_[2];
        u[5] = u[0];

        auto p{position_};

        p.linear() *= (Eigen::AngleAxisf{Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitY()}
                       * Eigen::AngleAxisf{Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitZ()}).toRotationMatrix();

        float angle{0.0};

        for (unsigned char n{0}; ++n; n < 2)
        {
            p.linear() *= Eigen::AngleAxis{angle, Eigen::Vector3f::UnitY()}.toRotationMatrix();

            for (unsigned char i{0}; i < 6; ++i)
            {
                auto const v{p * u[i]};

                for (unsigned char j{0}; j < 3; ++j)
                    vertices_[(m * 6 + i) * 3 + j] = v[j];
            }

            angle += Utility::Float::pi();

            ++m;
        }
    }

    for (auto& v : vertices_)
        v /= 2.0;
*/
    std::array<Eigen::Transform<float, 3, Eigen::Affine>, 3> positions{};

    positions.fill(Eigen::Transform<float, 3, Eigen::Affine>{Eigen::Matrix4f::Identity()});
    positions[1].linear() = (Eigen::AngleAxisf{-Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitZ()}
                             * Eigen::AngleAxisf{-Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitY()}).toRotationMatrix();
    positions[2].linear() = (Eigen::AngleAxisf{Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitY()}
                             * Eigen::AngleAxisf{Utility::Float::pi() / 2.0, Eigen::Vector3f::UnitZ()}).toRotationMatrix();

    unsigned char m{0};

    for (unsigned char i{0}; i < 3; ++i)
    {
        auto p{position_ * positions[i]};
        Eigen::Vector3f const v{(p.inverse() * (Eigen::Vector3f{} << sides_[0] / 2.0, sides_[1] / 2.0, sides_[2] / 2.0).finished())};
        auto const& x{v[0]};
        auto const& y{v[1]};
        auto const& z{v[2]};

        for (unsigned char j{0}; j < 2; ++j)
        {
            {
                auto const u{p * (Eigen::Vector3f{} << x, y, -z).finished()};

                for (unsigned char n{0}; n < 3; ++n)
                    vertices_[m * 3 + n] = u[n];

                ++m;
            }

            {
                auto const u{p * (Eigen::Vector3f{} << x, y, z).finished()};

                for (unsigned char n{0}; n < 3; ++n)
                    vertices_[m * 3 + n] = u[n];

                ++m;
            }

            for (unsigned char o{0}; o < 2; ++o)
            {
                auto const u{p * (Eigen::Vector3f{} << x, -y, z).finished()};

                for (unsigned char n{0}; n < 3; ++n)
                    vertices_[m * 3 + n] = u[n];

                ++m;
            }

            {
                auto const u{p * (Eigen::Vector3f{} << x, -y, -z).finished()};

                for (unsigned char n{0}; n < 3; ++n)
                    vertices_[m * 3 + n] = u[n];

                ++m;
            }

            {
                auto const u{p * (Eigen::Vector3f{} << x, y, -z).finished()};

                for (unsigned char n{0}; n < 3; ++n)
                    vertices_[m * 3 + n] = u[n];

                ++m;
            }

            p.linear() *= Eigen::AngleAxisf{Utility::Float::pi(), Eigen::Vector3f::UnitZ()}.toRotationMatrix();
        }
    }

    glBufferVertices_.update();

    previousPosition_ = position_;
    verticesUpdated_ = true;
}

void GlBlock::display(glm::mat4 const& projection, glm::mat4 const& modelView,
                      sf::Shader const& colorShader) const
{
    updateVertices_();

    glUseProgram(colorShader.getNativeHandle());

    glVertexArrayColors_.bind();

    glUniformMatrix4fv(glGetUniformLocation(colorShader.getNativeHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(colorShader.getNativeHandle(), "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));

    glDrawArrays(GL_TRIANGLES, 0, vertices_.size() / 3);

    glVertexArrayColors_.unbind();

    glUseProgram(0);
}

void GlBlock::display(glm::mat4 const& projection, glm::mat4 const& modelView,
                      sf::Shader const& textureShader, std::array<std::reference_wrapper<const sf::Texture>, 6> const& facetTextures) const
{
    updateVertices_();

    glUseProgram(textureShader.getNativeHandle());

    glVertexArrayTexturesVertices_.bind();

    glUniformMatrix4fv(glGetUniformLocation(textureShader.getNativeHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(textureShader.getNativeHandle(), "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));

    for (unsigned char i{0}; i < 6; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, facetTextures[i].get().getNativeHandle());

        glDrawArrays(GL_TRIANGLES, i * 2 * 3, 2 * 3);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glVertexArrayTexturesVertices_.unbind();

    glUseProgram(0);
}

std::array<float, 6 * 2 * 3 * 3> const& GlBlock::vertices() const
{
    updateVertices_();

    return vertices_;
}

void GlBlock::changeFacetColors(std::array<float, 6 * 4> const& colors)
{
    for (unsigned char i{0}; i < 6; ++i)
        for (unsigned char j{0}; j < 6; ++j)
            for (unsigned char k{0}; k < 4; ++k)
                colors_[(i * 6 + j) * 4 + k] = colors[i * 4 + k];

    glBufferColors_.update();
}
