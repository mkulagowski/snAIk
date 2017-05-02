#include "PyramidObject.hpp"


ImportObject::ImportObject(const std::string& path)
{

    mVertexData = {};

    mNormalData = {};

    mIndices = {};
}

ImportObject::~ImportObject()
{
}

void ImportObject::AddShape()
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