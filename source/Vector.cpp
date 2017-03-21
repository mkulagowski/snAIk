#include "Vector.hpp"

Vector::Vector(int x, int y, int z)
    : x(x)
    , y(y)
    , z(z)
{
}

Vector::Vector()
    : Vector(0, 0, 0)
{
}

Vector::Vector(btVector3 vect)
    : Vector(static_cast<int>(vect.getX()),
             static_cast<int>(vect.getY()),
             static_cast<int>(vect.getZ()))
{
}

Vector::Vector(glm::vec3 vect)
    : Vector(static_cast<int>(vect.x),
             static_cast<int>(vect.y),
             static_cast<int>(vect.z))
{
}

Vector::~Vector()
{
}

Vector Vector::operator+=(const Vector& other)
{
    return Vector(this->x + other.x, this->y + other.y, this->z + other.z);
}

Vector Vector::operator*(const double m)
{
    return Vector(static_cast<int>(this->x * m),
                  static_cast<int>(this->y * m),
                  static_cast<int>(this->z * m));
}

Vector::operator btVector3()
{
    return btVector3(static_cast<btScalar>(x),
                     static_cast<btScalar>(y),
                     static_cast<btScalar>(z));
}

Vector::operator glm::vec3()
{
    return glm::vec3(x, y, z);
}