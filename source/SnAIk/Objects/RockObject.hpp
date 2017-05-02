#pragma once

#include "Primitives/BoxObject.hpp"


class RockObject : public BoxObject
{
public:
    RockObject(btVector3 halfSize);
    ~RockObject();

    CollisionResult Collide(ObjectType otherObj) override;
};
