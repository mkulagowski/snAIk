#include "gtest/gtest.h"
#include "../SnAIk/Objects/Primitives/BoxObject.hpp"
#include "../SnAIk/GameManager.hpp"

#define M_PI 3.14159265358979323846

class BoxObjectTest : public ::testing::Test {
    virtual void SetUp() {
        GameManager::GetInstance().Init();
    }
};

TEST_F(BoxObjectTest, CtorTest)
{
    // Check 3int ctor and all possible getters
    btVector3 emptyVec(0,0,0);
    BoxObject test(2, 2, 2);
    ASSERT_EQ(36, test.GetIndicesCount());
    ASSERT_EQ(36, test.GetVerticesCount());
    ASSERT_TRUE(test.IsMoveable());
    ASSERT_EQ(nullptr, test.GetBody());
    ASSERT_EQ(emptyVec, test.GetColor());
    ASSERT_EQ(emptyVec, test.GetPosition());
    ASSERT_EQ(emptyVec, test.GetRotation());

    // Check btVector3 ctor and all possible getters
    btVector3 boxSize(2, 2, 2);
    BoxObject test2(boxSize);
    ASSERT_EQ(36, test2.GetIndicesCount());
    ASSERT_EQ(36, test2.GetVerticesCount());
    ASSERT_TRUE(test2.IsMoveable());
    ASSERT_EQ(nullptr, test2.GetBody());
    ASSERT_EQ(emptyVec, test2.GetColor());
    ASSERT_EQ(emptyVec, test2.GetPosition());
    ASSERT_EQ(emptyVec, test2.GetRotation());
}

TEST_F(BoxObjectTest, InitTest)
{
    // Check initial state after creation
    btVector3 emptyVec(0, 0, 0);
    BoxObject test(2, 2, 2);
    //bool res = GameManager::GetInstance().Init();
    ASSERT_EQ(36, test.GetIndicesCount());
    ASSERT_EQ(36, test.GetVerticesCount());
    ASSERT_TRUE(test.IsMoveable());
    ASSERT_EQ(nullptr, test.GetBody());
    ASSERT_EQ(emptyVec, test.GetColor());
    ASSERT_EQ(emptyVec, test.GetPosition());
    ASSERT_EQ(emptyVec, test.GetRotation());

    // After initializing position and rotation should be set to 0 (no change) and color should be set
    btVector3 color(0,1,0);
    float mass = 10.f;
    test.Init(color, mass);
    ASSERT_EQ(36, test.GetIndicesCount());
    ASSERT_EQ(36, test.GetVerticesCount());
    ASSERT_TRUE(test.IsMoveable());
    ASSERT_NE(nullptr, test.GetBody());
    ASSERT_EQ(color, test.GetColor());
    ASSERT_EQ(emptyVec, test.GetPosition());
    ASSERT_EQ(emptyVec, test.GetRotation());

}

TEST_F(BoxObjectTest, SetPosTest)
{
    btVector3 emptyVec(0, 0, 0);
    btVector3 color(0,1,0);
    float mass = 10.f;
    btVector3 newPos(10, 20, 30);
    // After calling ctor, position is 0,0,0
    BoxObject test(2, 2, 2);
    ASSERT_EQ(emptyVec, test.GetPosition());

    // Before calling init(), setPosition should not work
    test.SetPosition(newPos);
    ASSERT_EQ(emptyVec, test.GetPosition());

    // After calling init() position is still 0,0,0
    test.Init(color, mass);
    ASSERT_EQ(emptyVec, test.GetPosition());

    // After calling init(), setPosition should work
    test.SetPosition(newPos);
    ASSERT_EQ(newPos, test.GetPosition());
}

TEST_F(BoxObjectTest, MoveTest)
{
    btVector3 emptyVec(0, 0, 0);
    btVector3 color(0,1,0);
    float mass = 10.f;
    btVector3 moveDirection(2, 0, 3);
    double moveDistance = 10.2;

    // After calling ctor, position is 0,0,0
    BoxObject test(2, 2, 2);
    ASSERT_EQ(emptyVec, test.GetPosition());

    // Before calling init(), Move should not work
    test.Move(moveDistance, moveDirection);
    ASSERT_EQ(emptyVec, test.GetPosition());

    // After calling init() position is still 0,0,0
    test.Init(color, mass);
    ASSERT_EQ(emptyVec, test.GetPosition());

    // After calling init(), Move should work
    test.Move(moveDistance, moveDirection);
    btVector3 newPos = emptyVec;
    newPos += (moveDirection * moveDistance);
    ASSERT_EQ(newPos, test.GetPosition());

    // Test consecutive Move()
    test.Move(moveDistance, moveDirection);
    newPos += (moveDirection * moveDistance);
    ASSERT_EQ(newPos, test.GetPosition());

    // Test Move() in null direction
    test.Move(moveDistance, btVector3());
    ASSERT_EQ(newPos, test.GetPosition());

    // Test Move() for null distance
    test.Move(0.0, moveDirection);
    ASSERT_EQ(newPos, test.GetPosition());
}

TEST_F(BoxObjectTest, RotateTest)
{
    btVector3 emptyVec(0, 0, 0);
    btVector3 color(0,1,0);
    float mass = 10.f;
    btVector3 rotateAxisX(1, 0, 0),rotateAxisY(0, 1, 0), rotateAxisZ(0, 0, 1);
    float rotateDegrees = 30.f;
    float rotateRadians = M_PI * rotateDegrees / 180;

    // After calling ctor, rotation is 0,0,0
    BoxObject test(2, 2, 2);
    ASSERT_EQ(emptyVec, test.GetRotation());

    // Before calling init(), Rotate should not work
    test.Rotate(rotateDegrees, rotateAxisY);
    ASSERT_EQ(emptyVec, test.GetRotation());

    // After calling init() rotation is still 0,0,0
    test.Init(color, mass);
    ASSERT_EQ(emptyVec, test.GetRotation());

    // After calling init(), Rotate should work
    test.Rotate(rotateDegrees, rotateAxisX);
    btQuaternion quat(rotateAxisX, rotateRadians);
    btVector3 newRot = btVector3(quat.getX(), 0, 0);
    ASSERT_GT(0.0001f, newRot.distance(test.GetRotation()));
    ASSERT_LT(-0.0001f, newRot.distance(test.GetRotation()));

    test.Rotate(rotateDegrees, rotateAxisY);
    quat = btQuaternion(rotateAxisY, rotateRadians);
    newRot = btVector3(0, quat.getY(), 0);
    ASSERT_GT(0.0001f, newRot.distance(test.GetRotation()));
    ASSERT_LT(-0.0001f, newRot.distance(test.GetRotation()));

    test.Rotate(rotateDegrees, rotateAxisZ);
    quat = btQuaternion(rotateAxisZ, rotateRadians);
    newRot = btVector3(0, 0, quat.getZ());
    ASSERT_GT(0.0001f, newRot.distance(test.GetRotation()));
    ASSERT_LT(-0.0001f, newRot.distance(test.GetRotation()));

    // Test Rotate() for null degrees
    test.Rotate(0.f, rotateAxisX);
    ASSERT_EQ(emptyVec, test.GetRotation());
}

TEST_F(BoxObjectTest, SetColorTest)
{
    btVector3 emptyVec(0, 0, 0);
    btVector3 color(0, 1, 0);
    float mass = 10.f;
    btVector3 newColor(1, 0, 0);

    // After calling ctor, color is 0,0,0
    BoxObject test(2, 2, 2);
    ASSERT_EQ(emptyVec, test.GetColor());

    // Before calling init(), setColor should not work
    test.SetColor(newColor);
    ASSERT_EQ(emptyVec, test.GetColor());

    // After calling init() color should be changed
    test.Init(color, mass);
    ASSERT_EQ(color, test.GetColor());

    // After calling init(), setColor should work
    
    test.SetColor(newColor);
    ASSERT_EQ(newColor, test.GetColor());
}