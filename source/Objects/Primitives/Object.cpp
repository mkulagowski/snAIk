#include "Object.hpp"

#define M_PI 3.14159265358979323846


Object::Object()
    : mType(ObjectType::None)
    , mVertexArrayID(GL_NONE)
    , mNormalArrayID(GL_NONE)
    , mIndexArrayID(GL_NONE)
    , mToDelete(false)
    , mVelocity(10.0f)
    , mInitDone(false)
    , mShape(nullptr)
    , mBody(nullptr)
{
}

Object::~Object()
{
    if (mInitDone)
    {
        // Delete pointers
        delete mShape;
        delete mBody->getMotionState();
        delete mBody;

        // Unbind buffers
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

        // Delete buffers
        glDeleteBuffers(1, &mVertexArrayID);
        glDeleteBuffers(1, &mIndexArrayID);
    }
}

void Object::Init(btVector3 color, float mass)
{
    // Generate and fill vertex buffer
    glGenBuffers(1, &mVertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayID);
    glBufferData(GL_ARRAY_BUFFER, mVertexData.size() * sizeof(float),
                 &mVertexData[0], GL_STATIC_DRAW);

    // Generate and fill normals buffer
    glGenBuffers(1, &mNormalArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayID);
    glBufferData(GL_ARRAY_BUFFER, mNormalData.size() * sizeof(float),
                 &mNormalData[0], GL_STATIC_DRAW);

    // Generate and fill index buffer
    glGenBuffers(1, &mIndexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexArrayID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mIndices.size() * sizeof(unsigned short),
                 &mIndices[0], GL_STATIC_DRAW);

    // Set color and initialize objects physics
    mColor = color;
    InitPhysics(mass);

    mInitDone = true;
}

void Object::InitPhysics(float mass)
{
    // Add collision shape to the object
    AddShape();

    // Create motion state at the start of coords
    btVector3 position(0, 0, 0);
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));

    // Calculate inertia
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);

    // Create a rigid body
    btRigidBody::btRigidBodyConstructionInfo bulletRigidBodyCI(mass, motionState, mShape, inertia);
    mBody = new btRigidBody(bulletRigidBodyCI);

    // Set rigid bodys pointer to this Object
    mBody->setUserPointer(this);

    // Set movement axes
    mBody->setLinearFactor(btVector3(1, 0, 1));
}

void Object::Bind() const
{
    // Bind vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayID);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );

    // Bind normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayID);
    glVertexAttribPointer(
        1,                  /* attribute */
        3,                  /* size */
        GL_FLOAT,           /* type */
        GL_FALSE,           /* normalized? */
        0,                  /* stride */
        (void*)0            /* array buffer offset */
        );

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexArrayID);
}

GLsizei Object::GetIndicesCount() const
{
    return static_cast<GLsizei>(mIndices.size());
}

GLsizei Object::GetVerticesCount() const
{
    return static_cast<GLsizei>(mVertexData.size() / 3);
}

void Object::SetPosition(btVector3 newPosition)
{
    // If rigid body exists, set it's world matrix's position
    if (mBody)
    {
        btTransform trans = mBody->getWorldTransform();
        trans.setOrigin(newPosition);
        mBody->setWorldTransform(trans);
    }
}

btVector3 Object::GetPosition() const
{
    // Get position from rigid body
    if (mBody)
        return mBody->getWorldTransform().getOrigin();

    return btVector3();
}

void Object::SetColor(btVector3 color)
{
    mColor = color;
}

btVector3 Object::GetColor() const
{
    return mColor;
}

bool Object::ToDelete() const
{
    return mToDelete;
}

btRigidBody* Object::GetBody() const
{
    if (mBody)
        return mBody;

    return nullptr;
}

void Object::Rotate(float degrees, btVector3 rotationAxis)
{
    // Update rigid bodies center of mass transformation by a rotation
    if (mBody)
    {
        btScalar radians = static_cast<btScalar>(M_PI * degrees / 180);
        btQuaternion quat(rotationAxis, radians);

        btTransform bulletRotation = mBody->getCenterOfMassTransform();
        bulletRotation.setRotation(quat);

        mBody->setCenterOfMassTransform(bulletRotation);
    }
}

void Object::Move(double distance, btVector3 direction)
{
    btVector3 movement = direction * mVelocity;
    movement *= static_cast<btScalar>(distance);
    SetPosition(GetPosition() + movement);
}