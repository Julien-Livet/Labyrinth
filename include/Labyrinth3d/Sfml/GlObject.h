#ifndef GLOBJECT_H
#define GLOBJECT_H

/*!
 *  \file GlObject.h
 *  \brief Use a GL frame buffer
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 22/02/2018
 */

#include "openGl.h"

class GlObject
{
    public:
        class InvalidId : public std::runtime_error
        {
            public:
                InvalidId();
        };

        GlObject(void (*glGen)(GLsizei, GLuint*),
                 void (*glBind1)(GLuint),
                 GLboolean (*glIs)(GLuint),
                 void (*glDelete)(GLsizei, const GLuint*));
        GlObject(GLenum target,
                 void (*glGen)(GLsizei, GLuint*),
                 void (*glBind2)(GLenum, GLuint),
                 GLboolean (*glIs)(GLuint),
                 void (*glDelete)(GLsizei, const GLuint*));
        GlObject(GlObject&&) = default;
        virtual ~GlObject();
        GlObject(GlObject const&) = delete;
        GlObject& operator=(GlObject const&) = delete;
        GLuint id() const;
        void bind() const;
        void unbind() const;

    protected:
        void generate();
        void clear();
        GLenum target() const;

    private:
        GLenum target_;
        void (*glGen_)(GLsizei, GLuint*);
        void (*glBind1_)(GLuint);
        void (*glBind2_)(GLenum, GLuint);
        GLboolean (*glIs_)(GLuint);
        void (*glDelete_)(GLsizei, const GLuint*);
        GLuint id_;
};

#endif // GLOBJECT_H
