#include <numeric>
#include <cassert>
#include <cstring>

#include "Labyrinth3d/Sfml/GlBuffer.h"

GlBuffer::GlBuffer(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
    : GlObject{target, glGenBuffers, glBindBuffer, glIsBuffer, glDeleteBuffers}, subData_{std::make_pair(size, data)}
{
    bind();

    try
    {
        this->data(size, data, usage);
    }
    catch (GlException const& e)
    {
        clear();

        throw e;
    }

    unbind();
}

GlBuffer::GlBuffer(GLenum target, std::initializer_list<std::pair<GLsizeiptr, const GLvoid*> > const& subData, GLenum usage)
    : GlBuffer{target, subData.begin(), subData.end(), usage}
{
}

template <class Iterator>
GlBuffer::GlBuffer(GLenum target, Iterator begin, Iterator end, GLenum usage)
    : GlObject{target, glGenBuffers, glBindBuffer, glIsBuffer, glDeleteBuffers}, usage_{}, subData_{}
{
    GLsizeiptr const size{std::accumulate(begin, end, 0, [](auto const& lhs, auto const& rhs) {return lhs + rhs.first;})};

    assert(size);

    bind();

    try
    {
        this->data(size, NULL, usage);
    }
    catch (GlException const& e)
    {
        clear();

        throw e;
    }

    subData_.reserve(std::distance(begin, end));

    GLsizeiptr offset{0};

    for (; begin != end; ++begin)
    {
        subData(offset, begin->first, begin->second);

        subData_.push_back(*begin);

        offset += begin->first;
    }

    unbind();
}

GLenum GlBuffer::usage() const
{
    return usage_;
}

void GlBuffer::data(GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    glBufferData(target(), size, data, usage);

    if (glGetError())
        throw GlException{};
}

void GlBuffer::subData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(target(), offset, size, data);

    if (glGetError())
        throw GlException{};
}

std::size_t GlBuffer::subDataCount() const
{
    return subData_.size();
}

void* GlBuffer::map(GLenum access) const
{
    void* data{glMapBuffer(target(), access)};

    if (glGetError())
        throw GlException{};

    return data;
}

void GlBuffer::unmap()
{
    glUnmapBuffer(target());
}

void GlBuffer::update(std::size_t subDataIndex, const GLvoid* data, bool rememberData)
{
    assert(subDataIndex < subData_.size());
    assert(data != NULL);

    if (rememberData)
        subData_[subDataIndex].second = data;

    bind();

    auto address{map(GL_WRITE_ONLY)};

    std::size_t offset{0};

    for (std::size_t i{0}; i < subDataIndex; ++i)
        offset += subData_[subDataIndex].first;

    std::memcpy(static_cast<char*>(address + offset), data, subData_[subDataIndex].first);

    unmap();

    unbind();
}

void GlBuffer::update(std::size_t subDataIndex)
{
    assert(subDataIndex < subData_.size());

    update(subDataIndex, subData_[subDataIndex].second);
}

void GlBuffer::update()
{
    bind();

    auto adress{map(GL_WRITE_ONLY)};

    std::size_t offset{0};

    for (std::size_t i{0}; i < subData_.size(); ++i)
    {
        std::memcpy(static_cast<char*>(adress + offset), subData_[i].second, subData_[i].first);

        offset += subData_[i].first;
    }

    unmap();

    unbind();
}

const GLvoid* GlBuffer::localAddress(std::size_t subDataIndex) const
{
    std::size_t offset{0};

    for (std::size_t i{0}; i < subDataIndex; ++i)
        offset += subData_[subDataIndex].first;

    return static_cast<char*>(static_cast<void*>(NULL) + offset);
}

GLenum GlBuffer::target() const
{
    return GlObject::target();
}
