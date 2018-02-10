#pragma once

#include <vector>
#include <GL/glew.h>
#include <forward_list>

#include "Objects/Primitives/Object.hpp"

const std::string FSHADER_PATH1 = "source\\SnAIk\\Shaders\\Main.fragmentshader";
const std::string VSHADER_PATH1 = "source\\SnAIk\\Shaders\\Main.vertexshader";


class Renderer
{
public:
    Renderer();
    ~Renderer();

	bool Init();

    void Draw() const;
    void AddObject(const Object* obj);
	void RemoveAllByTag(const uint8_t& tag);

private:
    GLint mProgram;
    GLint mMMatrixUni;
    GLint mVMatrixUni;
    GLint mPMatrixUni;
    GLint mColorUni;
    GLint mCameraUni;
    GLuint mDummyVAO;
    std::forward_list<const Object*> mObjectList;

	bool LoadShaders(std::string vertex, std::string fragment);
};