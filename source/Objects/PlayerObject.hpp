#pragma once

#include "Primitives/BoxObject.hpp"


class PlayerObject : public BoxObject
{
public:
    PlayerObject();
    ~PlayerObject();

    CollisionResult Collide(ObjectType otherObj) override;
};