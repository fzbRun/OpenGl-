#include<iostream>
#include<map>

#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include"../../OpenGL-Mathematics-master/glm/glm.hpp"
#include"../../OpenGL-Mathematics-master/glm/gtc//matrix_transform.hpp"
#include"../../OpenGL-Mathematics-master/glm/gtc//type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include"MyCamera.h"
#include"MyShader.h"
#include"MyModel.h"
#include <random>
#include"tga.h"

using namespace glm;

//屏幕大小
float SCR_WIDTH = 800.0f;
float SCR_HEIGHT = 600.0f;
//摄像机
MyCamera camera(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, -90.0f, 45.0f);
//时间
float lastFrame;
float deltaTime;
//鼠标位置
bool firstmouse = true;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.cameraKeyMove(FORWARD, deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.cameraKeyMove(BACKWARD, deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.cameraKeyMove(LEFT, deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.cameraKeyMove(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstmouse) {
        lastX = xpos;
        lastY = ypos;
        firstmouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.cameraMouseMove(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.cameraScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

//加载3D纹理
unsigned int loadCudeMap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "cubeMap fail to load" << std::endl;
        }
        stbi_image_free(data);

    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadHDRTexture(char const* path) {

    stbi_set_flip_vertically_on_load(true);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int main() {
    //初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learnOpengl", NULL, NULL);
    if (window == NULL) {
        std::cout << "window fail to create" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "glad fail to create" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(30.0f),
        glm::vec3(30.0f),
        glm::vec3(30.0f),
        glm::vec3(30.0f)
    };

    /*
    float nrRow = 7.0f;
    float nrColumn = 7.0f;
    float spacing = 2.5f;
    */

    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    unsigned int albedo = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\rustediron1-alt2-Unreal-Engine\\rustediron2_basecolor.png");
    unsigned int metallic = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\rustediron1-alt2-Unreal-Engine\\rustediron2_metallic.png");
    unsigned int normal = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\rustediron1-alt2-Unreal-Engine\\rustediron2_normal.png");
    unsigned int roughness = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\rustediron1-alt2-Unreal-Engine\\rustediron2_roughness.png");


    //MyShader PBR_direct_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_direct.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_direct.fs");
    //MyShader PBR_diffuse_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_direct.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse.fs");
    MyShader PBR_diffuse_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_direct.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_specular.fs");
    MyShader PBR_diffuse_env_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_env.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_env.fs");
    MyShader PBR_diffuse_env_irradiance_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_env.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_env_irradiance.fs");
    MyShader PBR_specular_env_prefliter_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_env.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_specular_prefilter.fs");
    MyShader PBR_specular_env_brdf_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_specular_brdf.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_specular_brdf.fs");
    MyShader PBR_diffuse_envbox_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_envbox.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_diffuse_envbox.fs");

    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    /*
    PBR_direct_shader.use();
    PBR_direct_shader.setMaxtrix4fv("projection", projection);
    //PBR_direct_shader.setVec3("albedo", vec3(0.5f, 0.0f, 0.0f));
    PBR_direct_shader.setFloat("ao", 1.0f);
    PBR_direct_shader.setInt("albedoMap", 0);
    PBR_direct_shader.setInt("metallicMap", 1);
    PBR_direct_shader.setInt("roughnessMap", 2);
    PBR_direct_shader.setInt("normalMap", 3);
    */


    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    //unsigned int hdrTexture = loadHDRTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\ibl_hdr_radiance.png");
    unsigned int hdrTexture = loadHDRTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Desert_Highway\\Road_to_MonumentValley_8k.jpg");

    unsigned int envCubeMap;
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    PBR_diffuse_env_shader.use();
    PBR_diffuse_env_shader.setMaxtrix4fv("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glViewport(0, 0, 512.0f, 512.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (int i = 0; i < 6; i++) {
        PBR_diffuse_env_shader.setMaxtrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);


    PBR_diffuse_env_irradiance_shader.use();
    PBR_diffuse_env_irradiance_shader.setMaxtrix4fv("projection", captureProjection);
    glViewport(0.0f, 0.0f, 32.0f, 32.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (int i = 0; i < 6; i++) {
        PBR_diffuse_env_irradiance_shader.setMaxtrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }

    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    PBR_specular_env_prefliter_shader.use();
    PBR_specular_env_prefliter_shader.setMaxtrix4fv("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        PBR_specular_env_prefliter_shader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            PBR_specular_env_prefliter_shader.setMaxtrix4fv("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    PBR_specular_env_brdf_shader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PBR_diffuse_shader.use();
    PBR_diffuse_shader.setMaxtrix4fv("projection", projection);
    PBR_diffuse_shader.setFloat("ao", 1.0f);
    PBR_diffuse_shader.setInt("albedoMap", 0);
    PBR_diffuse_shader.setInt("metallicMap", 1);
    PBR_diffuse_shader.setInt("roughnessMap", 2);
    PBR_diffuse_shader.setInt("normalMap", 3);
    PBR_diffuse_shader.setInt("brdfMap", 4);
    PBR_diffuse_shader.setInt("irradianceMap", 5);
    PBR_diffuse_shader.setInt("prefilterMap", 6);


    PBR_diffuse_envbox_shader.use();
    PBR_diffuse_envbox_shader.setMaxtrix4fv("projection", projection);

    MyShader tga_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_tga.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\PBR_tga.fs");
    tga_shader.use();
    tga_shader.setMaxtrix4fv("projection", projection);
    tga_shader.setInt("albedoMap", 5);
    tga_shader.setInt("metallicMap", 6);
    tga_shader.setInt("roughnessMap", 7);
    tga_shader.setInt("normalMap", 8);
    tga_shader.setInt("brdfMap", 9);
    tga_shader.setInt("irradianceMap", 10);
    tga_shader.setInt("prefilterMap", 11);
    tga_shader.setInt("aoMap", 12);

    stbi_set_flip_vertically_on_load(false);
    unsigned int tga_albedoMap;
    tga_albedoMap = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Cerberus_by_Andrew_Maximov\\Textures\\Cerberus_A.tga");
    unsigned int tga_metallicMap;
    tga_metallicMap = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Cerberus_by_Andrew_Maximov\\Textures\\Cerberus_M.tga");
    unsigned int tga_normalMap;
    tga_normalMap = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Cerberus_by_Andrew_Maximov\\Textures\\Cerberus_N.tga");
    unsigned int tga_roughnessMap;
    tga_roughnessMap = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Cerberus_by_Andrew_Maximov\\Textures\\Cerberus_R.tga");
    unsigned int tga_aoMap;
    tga_aoMap = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Cerberus_by_Andrew_Maximov\\Textures\\Raw\\Cerberus_AO.tga");

    MyModel obj("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\Cerberus_by_Andrew_Maximov\\Cerberus_LP.FBX");

    glViewport(0.0f, 0.0f, SCR_WIDTH, SCR_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        mat4 view = camera.getView();

        /*
        PBR_direct_shader.use();
        PBR_direct_shader.setMaxtrix4fv("view", view);
        PBR_direct_shader.setVec3("camPos", camera.Position);
        glm::mat4 model = glm::mat4(1.0f);
        PBR_direct_shader.setMaxtrix4fv("model", model);

        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            PBR_direct_shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            PBR_direct_shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }
        */

        PBR_diffuse_shader.use();
        PBR_diffuse_shader.setMaxtrix4fv("view", view);
        PBR_diffuse_shader.setVec3("camPos", camera.Position);
        glm::mat4 model = glm::mat4(1.0f);
        PBR_diffuse_shader.setMaxtrix4fv("model", model);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            PBR_diffuse_shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            PBR_diffuse_shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metallic);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughness);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normal);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP,irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        renderSphere();
        /*
        tga_shader.use();
        tga_shader.setMaxtrix4fv("view", view);
        tga_shader.setVec3("camPos", camera.Position);
        model = glm::mat4(1.0f);
        model = scale(model, vec3(0.1f));
        model = rotate(model, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        model = rotate(model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        model = rotate(model, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        tga_shader.setMaxtrix4fv("model", model);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            tga_shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            tga_shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, tga_albedoMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, tga_metallicMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, tga_roughnessMap);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, tga_normalMap);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_2D, tga_aoMap);
        obj.Draw(tga_shader);
        */

        /*
        for (int row = 0; row < nrRow; ++row)
        {
            PBR_direct_shader.setFloat("metallic", (float)row / (float)nrRow);
            for (int col = 0; col < nrColumn; ++col)
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                PBR_direct_shader.setFloat("roughness", glm::clamp((float)col / (float)nrColumn, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumn / 2)) * spacing,
                    (row - (nrRow / 2)) * spacing,
                    0.0f
                ));
                PBR_direct_shader.setMaxtrix4fv("model", model);
                renderSphere();
            }
        }
        */

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        PBR_diffuse_envbox_shader.use();
        PBR_diffuse_envbox_shader.setMaxtrix4fv("view", view);
        model = mat4(1.0f);
        //model = scale(model, vec3(3.0f));
        PBR_diffuse_envbox_shader.setMaxtrix4fv("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        renderCube();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}