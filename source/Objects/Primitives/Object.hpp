#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>
#include <vector>

enum class ObjectType
{
    None,
    Wall,
    Player,
    Projectile,
    Asteroid
};

enum class CollisionResult
{
    None,
    Reinit,
    Score,
};


class Object
{
public:
    ObjectType mType;


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
    void Rotate (float degrees, btVector3 rotationAxis);

    bool ToDelete() const;

    virtual CollisionResult Collide(ObjectType otherObj) = 0;

protected:
    GLuint mVertexArrayID, mNormalArrayID, mIndexArrayID;
    std::vector<float> mVertexData;
    std::vector<float> mNormalData;
    std::vector<unsigned short> mIndices;

    btCollisionShape* mShape;
    btRigidBody* mBody;

    float mVelocity;
    btVector3 mColor;
    bool mToDelete;
    bool mInitDone;


    virtual void AddShape() = 0;
    void InitPhysics(float mass);
};