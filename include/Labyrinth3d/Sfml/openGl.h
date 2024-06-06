#ifndef OPENGL_H
#define OPENGL_H

#include <stdexcept>

#include <GL/glew.h>

#include <SFML/OpenGL.hpp>

class GlException : public std::runtime_error
{
    public:
        GlException() : std::runtime_error{"GL error!"}, glError_{glError()}
        {
        }

        GLenum glError() const
        {
            return glError_;
        }

    private:
        GLenum glError_;
};

#endif // OPENGL_H
