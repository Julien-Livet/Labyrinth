#ifndef GLVERTEXARRAY_H
#define GLVERTEXARRAY_H

/*!
 *  \file GlVertexArray.h
 *  \brief Use a GL vertex array object
 *  \author Julien LIVET
 *  \version 1.0
 *  \date 22/02/2018
 */

#include "Labyrinth3d/Sfml/GlObject.h"

class GlVertexArray : public GlObject
{
    public:
        GlVertexArray();
        GlVertexArray(GlVertexArray&&) = default;
        ~GlVertexArray() = default;
        GlVertexArray(GlVertexArray const&) = delete;
        GlVertexArray& operator=(GlVertexArray const&) = delete;
};

#endif // GLVERTEXARRAY_H
