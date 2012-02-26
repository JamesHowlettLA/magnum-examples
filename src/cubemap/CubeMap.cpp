/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "CubeMap.h"

#include <fstream>

#include "Utility/Resource.h"

#include "CubeMapTexture.h"
#include "Scene.h"
#include "Camera.h"
#include "Trade/AbstractImporter.h"
#include "Primitives/Cube.h"
#include "MeshTools/CompressIndices.h"

#include "Reflector.h"

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Examples {

CubeMap::CubeMap(Trade::AbstractImporter* importer, const string& prefix, Object* parent): Object(parent) {
    Primitives::Cube cubeData;
    Buffer* buffer = cube.addBuffer(false);
    buffer->setData(*cubeData.vertices(0), Buffer::Usage::StaticDraw);
    cube.setVertexCount(cubeData.vertices(0)->size());
    cube.bindAttribute<Vector4>(buffer, CubeMapShader::Vertex);
    MeshTools::compressIndices(&cube, Buffer::Usage::StaticDraw, *cubeData.indices());

    scale(20.0f);

    /* Textures */
    Trade::ImageData2D* image;
    importer->open(prefix + "+x.tga");
    image = importer->image2D(0);
    texture.setData(CubeMapTexture::PositiveX, 0, AbstractTexture::InternalFormat::RGB, image);

    importer->open(prefix + "-x.tga");
    image = importer->image2D(0);
    texture.setData(CubeMapTexture::NegativeX, 0, AbstractTexture::InternalFormat::RGB, image);

    importer->open(prefix + "+y.tga");
    image = importer->image2D(0);
    texture.setData(CubeMapTexture::PositiveY, 0, AbstractTexture::InternalFormat::RGB, image);

    importer->open(prefix + "-y.tga");
    image = importer->image2D(0);
    texture.setData(CubeMapTexture::NegativeY, 0, AbstractTexture::InternalFormat::RGB, image);

    importer->open(prefix + "+z.tga");
    image = importer->image2D(0);
    texture.setData(CubeMapTexture::PositiveZ, 0, AbstractTexture::InternalFormat::RGB, image);

    importer->open(prefix + "-z.tga");
    image = importer->image2D(0);
    texture.setData(CubeMapTexture::NegativeZ, 0, AbstractTexture::InternalFormat::RGB, image);

    texture.setMagnificationFilter(CubeMapTexture::Filter::LinearInterpolation);
    texture.setMinificationFilter(CubeMapTexture::Filter::LinearInterpolation, CubeMapTexture::Mipmap::LinearInterpolation);
    texture.setWrapping(Math::Vector2<CubeMapTexture::Wrapping>(CubeMapTexture::Wrapping::ClampToEdge, CubeMapTexture::Wrapping::ClampToEdge));
    texture.generateMipmap();

    reflector = new Reflector(&texture, scene());
    reflector->scale(0.5f);
    reflector->translate(Vector3::xAxis(-0.5f));

    reflector = new Reflector(&texture, scene());
    reflector->scale(0.3f);
    reflector->translate(Vector3::xAxis(0.3f));
}

void CubeMap::draw(const Matrix4& transformationMatrix, Camera* camera) {
    texture.bind();
    shader.use();
    shader.setModelViewProjectionMatrixUniform(camera->projectionMatrix()*transformationMatrix);
    shader.setTextureUniform(&texture);
    cube.draw();
}

}}
