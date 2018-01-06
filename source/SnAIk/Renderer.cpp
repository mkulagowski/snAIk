#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.hpp"
#include "GameManager.hpp"


Renderer::Renderer()
    : mDummyVAO(GL_NONE)
{
}

Renderer::~Renderer()
{
    // Disable vertex attribute arrays and delete VAO
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteVertexArrays(1, &mDummyVAO);
}

bool Renderer::Init()
{
    // Initialize glew
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "RENDERER: Failed to initialize GLEW.\n");
        return false;
    }

    // Set bg color - obviously black, as it usually is in outer space
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Set viewport
    glViewport(0, 0,
               static_cast<GLsizei>(GameManager::GetInstance().GetWidth()),
               static_cast<GLsizei>(GameManager::GetInstance().GetHeight()));

    // Enable culling and Z-buffer
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Generate a dummy VAO - OGL needs it, even if unused
    glGenVertexArrays(1, &mDummyVAO);
    glBindVertexArray(mDummyVAO);

    // Load shaders
    if (!LoadShaders(VSHADER_PATH, FSHADER_PATH))
    {
        fprintf(stderr, "RENDERER: Failed to load shaders.\n");
        return false;
    }

    // Get uniforms locations
    glUseProgram(mProgram);
    mMMatrixUni = glGetUniformLocation(mProgram, "Model");
    mVMatrixUni = glGetUniformLocation(mProgram, "View");
    mPMatrixUni = glGetUniformLocation(mProgram, "Projection");
    mColorUni = glGetUniformLocation(mProgram, "ObjectColor");
    mCameraUni = glGetUniformLocation(mProgram, "LightPosition_worldspace");

    // Set up camera view and load it into shaders
    glm::vec3 cameraPos = glm::vec3(50, 50, 50);
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(
        cameraPos,          // Camera at (0,75,1) in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
        );
    glUniformMatrix4fv(mVMatrixUni, 1, false, &View[0][0]);
    glUniformMatrix4fv(mPMatrixUni, 1, false, &Projection[0][0]);
    glUniform3f(mCameraUni, cameraPos.x, cameraPos.y, cameraPos.z);

    // Enable AttribArray - 0 for vertices, 1 for normals
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void Renderer::Draw() const
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all objects
    for (auto object : mObjectList)
    {
        // Bind object
        object->Bind();

        // Get model matrix from physics module and load it into shaders
        float matrix[16];
        object->GetBody()->getWorldTransform().getOpenGLMatrix(matrix);
        glUniformMatrix4fv(mMMatrixUni, 1, false, matrix);

        // Get color and load it into shaders
        btVector3 color = object->GetColor();
        glUniform3f(mColorUni, static_cast<GLfloat>(color.getX()),
                    static_cast<GLfloat>(color.getY()),
                    static_cast<GLfloat>(color.getZ()));

        // Draw
        glDrawElements(
            GL_TRIANGLES,                 // mode
            object->GetIndicesCount(),    // count
            GL_UNSIGNED_SHORT,            // type
            (void*)0                      // element array buffer offset
            );
    }
}

void Renderer::AddObject(const Object * obj)
{
    mObjectList.push_front(obj);
}


bool Renderer::LoadShaders(std::string vertex, std::string fragment)
{
    const char* vertex_file_path = vertex.c_str();
    const char* fragment_file_path = fragment.c_str();

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    } else
    {
        fprintf(stderr, "RENDERER: Impossible to open shader file: \"%s\".\n",
                vertex_file_path);
        return false;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    } else
    {
        fprintf(stderr, "RENDERER: Impossible to open shader file: \"%s\".\n",
                vertex_file_path);
		return false;
    }


    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    // When empty, Info Log contains only \0 char, making the length = 1
    if (InfoLogLength > 1)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL,
                           &VertexShaderErrorMessage[0]);
		if (!VertexShaderErrorMessage.empty())
		{
			fprintf(stderr, "RENDERER: Vertex shaders compilation errors:\n%s\n",
				&VertexShaderErrorMessage[0]);
			return false;
		}
			
    }


    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    
    // When empty, Info Log contains only \0 char, making the length = 1
    if (InfoLogLength > 1)
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL,
                           &FragmentShaderErrorMessage[0]);
		if (!FragmentShaderErrorMessage.empty())
		{
			fprintf(stderr, "RENDERER: Vertex shaders compilation errors:\n%s\n",
				&FragmentShaderErrorMessage[0]);
			return false;
		}
    }



    // Link the program
    mProgram = glCreateProgram();
    glAttachShader(mProgram, VertexShaderID);
    glAttachShader(mProgram, FragmentShaderID);
    glLinkProgram(mProgram);

    // Check the program
    glGetProgramiv(mProgram, GL_LINK_STATUS, &Result);
    glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);

    // When empty, Info Log contains only \0 char, making the length = 1
    if (InfoLogLength > 1)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(mProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		if (!ProgramErrorMessage.empty())
		{
			fprintf(stderr, "RENDERER: Shaders linking errors:\n%s\n",
				&ProgramErrorMessage[0]);
			return false;
		}
    }

    // Clean up
    glDetachShader(mProgram, VertexShaderID);
    glDetachShader(mProgram, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return true;
}
