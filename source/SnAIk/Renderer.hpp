#pragma once

#include <vector>
#include <GL/glew.h>
#include <forward_list>

#include "Objects/Primitives/Object.hpp"

class Renderer
{
public:
    Renderer();
    ~Renderer();

	bool Init();

    void Draw() const;
    void AddObject(const Object* obj);
	void RemoveAllByTag(const uint8_t& tag);
	void MoveCamera(const btVector3 moveVect);
	void ChangeLightPower(const float power);

private:
    GLint mProgram;
    GLint mMMatrixUni;
    GLint mVMatrixUni;
    GLint mPMatrixUni;
    GLint mColorUni;
    GLint mLightUni;
	GLint mLightPowerUni;
    GLuint mDummyVAO;
	btVector3 mCameraPos;
	float mLightPower;
    std::forward_list<const Object*> mObjectList;

	bool LoadShaders(std::string vertex, std::string fragment);
};