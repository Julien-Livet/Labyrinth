#ifndef TEXTURE_H
#define TEXTURE_H

/*!
 *  \file Texture.h
 *  \brief Use a texture
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 19/02/2017
 */

#include <SFML/Graphics/Image.hpp>

#include "openGl.h"

class Texture
{
    public:
        explicit Texture();
        explicit Texture(std::string const& fileName);
        Texture(Texture const&) = delete;
        Texture(Texture&&) = default;
        ~Texture();
        sf::Image const& image() const;
        GLuint id() const;
        Texture& operator=(Texture const&) = delete;

    private:
        sf::Image image_;
        GLuint id_;
};

#endif // TEXTURE_H
