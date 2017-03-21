#pragma once
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

class Vector
{
public:
    int x, y, z;

    Vector();
    Vector(btVector3 vect);
    Vector(glm::vec3 vect);
    Vector(int x, int y, int z);
    ~Vector();

    Vector operator+=(const Vector& other);
    Vector operator*(const double m);

    operator btVector3();
    operator glm::vec3();
};