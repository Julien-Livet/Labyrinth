#include <stdexcept>

#include "Labyrinth3d/Sfml/Texture.h"

Texture::Texture() : image_{}, id_{0}
{
}

Texture::~Texture()
{
    if (id_)
    {
        glDeleteTextures(1, &id_);

        id_ = 0;
    }
}

Texture::Texture(std::string const& fileName) : image_{}, id_{0}
{
    if (!image_.loadFromFile(fileName))
        throw std::runtime_error{"Unable to load image!"};

    image_.flipVertically();

    glGenTextures(1, &id_);

    glBindTexture(GL_TEXTURE_2D, id_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_.getSize().x, image_.getSize().y,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image_.getPixelsPtr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

sf::Image const& Texture::image() const
{
    return image_;
}

GLuint Texture::id() const
{
    return id_;
}
