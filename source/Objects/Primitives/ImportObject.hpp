#pragma once

#include "Object.hpp"


class ImportObject : public Object
{
public:
    ImportObject(const std::string& path);
    ~ImportObject();

private:
    btVector3 mHalfSize;

    void AddShape() override;
};
