#include "Labyrinth3d/Sfml/GlVertexArray.h"

GlVertexArray::GlVertexArray() : GlObject{glGenVertexArrays, glBindVertexArray, glIsVertexArray, glDeleteVertexArrays}
{
}
