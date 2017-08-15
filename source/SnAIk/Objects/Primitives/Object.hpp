#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>
#include <vector>

class Object
{
public:
    Object();
    ~Object();

    void Init(btVector3 color, float mass);
    void Bind() const;

    GLsizei GetIndicesCount() const;
    GLsizei GetVerticesCount() const;

    btVector3 GetPosition() const;
    void SetPosition(btVector3 position);

    btRigidBody* GetBody() const;

    btVector3 GetColor() const;
    void SetColor(btVector3 color);

    void Move(double distance, btVector3 direction);
    btVector3 GetRotation() const;
    void Rotate (float degrees, btVector3 rotationAxis);
    void ApplyTorque(btVector3 torque);

    const bool isMoveable() const;

protected:
    GLuint mVertexArrayID, mNormalArrayID, mIndexArrayID;
    std::vector<float> mVertexData;
    std::vector<float> mNormalData;
    std::vector<unsigned short> mIndices;

    btCollisionShape* mShape;
    btRigidBody* mBody;

    float mVelocity;
    btVector3 mColor;
    bool mInitDone;

    virtual void AddShape() = 0;
    void InitPhysics(float mass);
};