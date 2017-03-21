#include "PyramidObject.hpp"


PyramidObject::PyramidObject(btVector3 halfSize)
    : mHalfSize(halfSize)
{
    float x = static_cast<float>(halfSize.getX());
    float y = static_cast<float>(halfSize.getY());
    float z = static_cast<float>(halfSize.getZ());

    mVertexData = {
        // back 1
         x, y, z,
        -x, y, z,
        -x,-y, z,

        // back 2
        -x,-y, z,
         x,-y, z,
         x, y, z,

        // bottom side
         x,-y, z,
        -x,-y, z,
         0, 0,-z,

        // left side
        -x,-y, z,
        -x, y, z,
         0, 0,-z,

        // top side
        -x, y, z,
         x, y, z,
         0, 0,-z,

        // right side
         x, y, z,
         x,-y, z,
         0, 0,-z,
    };

    auto simpleNormal = [&](btVector3 v1, btVector3 v2, btVector3 v3)
    {
        btVector3 edge1 = v2 - v1;
        btVector3 edge2 = v3 - v1;
        return btCross(edge1, edge2).normalize();
    };

    btVector3 simpleNormB = simpleNormal(btVector3( x,-y, z), btVector3(-x,-y, z),
                                         btVector3( 0, 0,-z));
    btVector3 simpleNormL = simpleNormal(btVector3(-x,-y, z), btVector3(-x, y, z),
                                         btVector3( 0, 0,-z));
    btVector3 simpleNormT = simpleNormal(btVector3(-x, y, z), btVector3( x, y, z),
                                         btVector3( 0, 0,-z));
    btVector3 simpleNormR = simpleNormal(btVector3( x, y, z), btVector3( x,-y, z),
                                         btVector3( 0, 0,-z));

    mNormalData = {
        // back
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // bottom
        simpleNormB.getX(),simpleNormB.getY(),simpleNormB.getZ(),
        simpleNormB.getX(),simpleNormB.getY(),simpleNormB.getZ(),
        simpleNormB.getX(),simpleNormB.getY(),simpleNormB.getZ(),

        // left
        simpleNormL.getX(),simpleNormL.getY(),simpleNormL.getZ(),
        simpleNormL.getX(),simpleNormL.getY(),simpleNormL.getZ(),
        simpleNormL.getX(),simpleNormL.getY(),simpleNormL.getZ(),

        // top
        simpleNormT.getX(),simpleNormT.getY(),simpleNormT.getZ(),
        simpleNormT.getX(),simpleNormT.getY(),simpleNormT.getZ(),
        simpleNormT.getX(),simpleNormT.getY(),simpleNormT.getZ(),

        // right
        simpleNormR.getX(),simpleNormR.getY(),simpleNormR.getZ(),
        simpleNormR.getX(),simpleNormR.getY(),simpleNormR.getZ(),
        simpleNormR.getX(),simpleNormR.getY(),simpleNormR.getZ(),
    };

    mIndices = {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
    };
}

PyramidObject::PyramidObject(float x, float y, float z)
    : PyramidObject(btVector3(x, y, z))
{
}

PyramidObject::~PyramidObject()
{
}

void PyramidObject::AddShape()
{
    mShape = new btConvexHullShape();
    btConvexHullShape* shapePtr = reinterpret_cast<btConvexHullShape*>(mShape);

    // Create collision shape based on vertices
    for (int i = 0; i < GetVerticesCount(); i++)
    {
        btVector3 vertex(mVertexData[i], mVertexData[i + 1], mVertexData[i + 2]);
        shapePtr->addPoint(vertex);
    }
}