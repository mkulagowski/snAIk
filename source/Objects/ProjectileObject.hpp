#pragma once

#include "Primitives/PyramidObject.hpp"


class ProjectileObject : public PyramidObject
{
public:
    ProjectileObject();
    ~ProjectileObject();

    CollisionResult Collide(ObjectType otherObj) override;
};
