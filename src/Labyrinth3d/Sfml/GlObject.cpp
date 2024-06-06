#include <cassert>

#include "Labyrinth3d/Sfml/GlObject.h"

GlObject::InvalidId::InvalidId() : std::runtime_error{"Invalid id of GL buffer!"}
{
}

GlObject::GlObject(void (*glGen)(GLsizei, GLuint*),
                   void (*glBind1)(GLuint),
                   GLboolean (*glIs)(GLuint),
                   void (*glDelete)(GLsizei, const GLuint*))
    : target_{GL_ARRAY_BUFFER}, glGen_{glGen}, glBind1_{glBind1}, glBind2_{nullptr}, glIs_{glIs}, glDelete_{glDelete}
{
    assert(glGen_ != nullptr
           && glBind1_ != nullptr
           && glIs_ != nullptr
           && glDelete_ != nullptr);

    generate();
}

GlObject::GlObject(GLenum target,
                   void (*glGen)(GLsizei, GLuint*),
                   void (*glBind2)(GLenum, GLuint),
                   GLboolean (*glIs)(GLuint),
                   void (*glDelete)(GLsizei, const GLuint*))
    : target_{target}, glGen_{glGen}, glBind1_{nullptr}, glBind2_{glBind2}, glIs_{glIs}, glDelete_{glDelete}
{
    assert(glGen_ != nullptr
           && glBind2_ != nullptr
           && glIs_ != nullptr
           && glDelete_ != nullptr);

    generate();
}

GlObject::~GlObject()
{
    clear();
}

void GlObject::generate()
{
    clear();

    (*glGen_)(1, &id_);
}

GLuint GlObject::id() const
{
    return id_;
}

void GlObject::bind() const
{
    assert((glBind1_ != nullptr && glBind2_ == nullptr)
           || (glBind1_ == nullptr && glBind2_ != nullptr));

    if (glBind1_ != nullptr)
        (*glBind1_)(id_);
    else //if (glBind2_ != nullptr)
        (*glBind2_)(target_, id_);
}

void GlObject::unbind() const
{
    assert((glBind1_ != nullptr && glBind2_ == nullptr)
           || (glBind1_ == nullptr && glBind2_ != nullptr));

    if (glBind1_ != nullptr)
        (*glBind1_)(0);
    else //if (glBind2_ != nullptr)
        (*glBind2_)(target_, 0);
}

void GlObject::clear()
{
    if ((*glIs_)(id_) == GL_TRUE)
        (*glDelete_)(1, &id_);
}

GLenum GlObject::target() const
{
    assert(glBind1_ == nullptr && glBind2_ != nullptr);

    return target_;
}
