#ifndef GLBUFFER_H
#define GLBUFFER_H

/*!
 *  \file GlBuffer.h
 *  \brief Use a GL buffer object
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 21/02/2018
 */

#include <initializer_list>
#include <vector>

#include "GlObject.h"

class GlBuffer : public GlObject
{
    public:
        explicit GlBuffer(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
        explicit GlBuffer(GLenum target, std::initializer_list<std::pair<GLsizeiptr, const GLvoid*> > const& subData, GLenum usage);
        template <class Iterator>
        explicit GlBuffer(GLenum target, Iterator begin, Iterator end, GLenum usage);
        GlBuffer(GlBuffer&&) = default;
        ~GlBuffer() = default;
        GlBuffer(GlBuffer const&) = delete;
        GlBuffer& operator=(GlBuffer const&) = delete;
        GLenum usage() const;
        std::size_t subDataCount() const;
        void update(std::size_t subDataIndex, const GLvoid* data, bool rememberData = false);
        void update(std::size_t subDataIndex);
        void update();
        const GLvoid* localAddress(std::size_t subDataIndex) const; //to use with glVertexAttribPointer
        GLenum target() const;

    protected:
        void subData(GLintptr offset, GLsizeiptr size, const GLvoid* data);
        void data(GLsizeiptr size, const GLvoid* data, GLenum usage);
        void* map(GLenum acces) const;
        void unmap();

    private:
        GLenum usage_;
        std::vector<std::pair<GLsizeiptr, const GLvoid*> > subData_;
};

#endif // GLBUFFER_H
