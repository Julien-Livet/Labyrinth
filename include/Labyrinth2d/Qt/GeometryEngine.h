// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GeometryEngine : protected QOpenGLFunctions
{
    public:
    GeometryEngine(QVector3D const& bottom = QVector3D(-1.0, -1.0, -1.0),
                   QVector3D const& top = QVector3D(1.0, 1.0, 1.0));
        virtual ~GeometryEngine();

        void drawCubeGeometry(QOpenGLShaderProgram *program);

    private:
        void initCubeGeometry(QVector3D const& bottom, QVector3D const& top);

        QOpenGLBuffer arrayBuf;
        QOpenGLBuffer indexBuf;
};

#endif // GEOMETRYENGINE_H
