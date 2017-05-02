#include "PlaneObject.hpp"


PlaneObject::PlaneObject(btVector3 planeNormal, float x, float y)
    : mPlaneNormal(planeNormal)
{
    mVertexData = {
         x, y, 0, // 0
        -x, y, 0, // 1
        -x,-y, 0, // 2

        -x,-y, 0, // 2
         x,-y, 0, // 3
         x, y, 0, // 0
    };

    mNormalData = {
        // front
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
    };

    mIndices = {
        0,1,2,
        3,4,5,
    };
}

PlaneObject::PlaneObject(btVector3 planeNormal)
    : PlaneObject(planeNormal, 50.0f, 50.0f)
{
}

PlaneObject::~PlaneObject()
{}

void PlaneObject::AddShape()
{
    mShape = new btStaticPlaneShape(mPlaneNormal, 0);
}