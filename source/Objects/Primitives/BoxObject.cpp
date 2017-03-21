#include "BoxObject.hpp"


BoxObject::BoxObject(btVector3 halfSize)
    : mHalfSize(halfSize)
{
    float x = static_cast<float>(halfSize.getX());
    float y = static_cast<float>(halfSize.getY());
    float z = static_cast<float>(halfSize.getZ());

    mVertexData = {
        // front wall
        x, y, z, // 0
        -x, y, z, // 1
        -x,-y, z, // 2

        -x,-y, z, // 2
        x,-y, z, // 3
        x, y, z, // 0

        // right wall
        x, y, z, // 0
        x,-y, z, // 3
        x,-y,-z, // 4

        x,-y,-z, // 4
        x, y,-z, // 5
        x, y, z, // 0

        // top wall
        x, y, z, // 0
        x, y,-z, // 5
        -x, y,-z, // 6

        -x, y,-z, // 6
        -x, y, z, // 1
        x, y, z, // 0

        // left wall
        -x, y, z, // 1
        -x, y,-z, // 6
        -x,-y,-z, // 7

        -x,-y,-z, // 7
        -x,-y, z, // 2
        -x, y, z, // 1

         // bottom wall
        -x,-y,-z, // 7
        x,-y,-z, // 4
        x,-y, z, // 3

        x,-y, z, // 3
        -x,-y, z, // 2
        -x,-y,-z, // 7

        // back wall
        x,-y,-z, // 4
        -x,-y,-z, // 7
        -x, y,-z, // 6

        -x, y,-z, // 6
        x, y,-z, // 5
        x,-y,-z, // 4
    };

    mNormalData = {
        // front
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // right
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // top
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // left
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        // bottom
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,

        // back
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
    };

    mIndices = {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,
        14,15,16,17,18,19,20,21,22,23,24,
        25,26,27,28,29,30,31,32,33,34,35,
    };
}

BoxObject::BoxObject(float x, float y, float z)
    : BoxObject(btVector3(x, y, z))
{
}

BoxObject::~BoxObject()
{
}

void BoxObject::AddShape()
{
    mShape = new btBoxShape(mHalfSize);
}