#include<iostream>
#include<map>
#include <random>

#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include"../../OpenGL-Mathematics-master/glm/glm.hpp"
#include"../../OpenGL-Mathematics-master/glm/gtc//matrix_transform.hpp"
#include"../../OpenGL-Mathematics-master/glm/gtc//type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include"MyCamera.h"
#include"MyShader.h"
#include"MyModel.h"

using namespace glm;

//屏幕大小
float SCR_WIDTH = 800.0f;
float SCR_HEIGHT = 600.0f;
//阴影贴图大小
float depthMapResolution = 4096.0f;
//摄像机
MyCamera camera(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, -90.0f, 45.0f);
//时间
float lastFrame;
float deltaTime;
//鼠标位置
bool firstmouse = true;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
//摄像机远近平面
float cameraNearPlane = 0.1f;
float cameraFarPlane = 100.0f;
//光线方向
const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
//视锥体各个近远平面的集合
std::vector<float> shadowCascadeLevels_near;
std::vector<float> shadowCascadeLevels_far;
//随机数好像，不太清楚
std::random_device device;
std::mt19937 generator = std::mt19937(device());

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

//计算将视锥体分为几块
void CalcShadowCascadeLevels(int num) {
    float lambda = 0.75;
    float ratio = cameraFarPlane / cameraNearPlane;
    shadowCascadeLevels_near.push_back(cameraNearPlane);
    for (int i = 1; i < num; i++) {
        float si = i / (float)(num);
        std::cout << std::to_string(si) << std::endl;
        float nearPlane = lambda * (cameraNearPlane * pow(ratio, si)) + (1 - lambda) * (cameraNearPlane + (cameraFarPlane - cameraNearPlane) * si);
        float farPlane = nearPlane * 1.005f;
        shadowCascadeLevels_near.push_back(nearPlane);
        shadowCascadeLevels_far.push_back(farPlane);
    }
    shadowCascadeLevels_far.push_back(cameraFarPlane);
}

//计算摄像机坐标下各个子视锥体顶点在世界坐标下的位置
std::vector<vec4> getFrustumCornersWorldSpace(mat4 projection,mat4 view) {

    mat4 inv = inverse(projection * view);

    std::vector<vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    return frustumCorners;
}

//计算各个光空间转换矩阵
std::vector<mat4> getLightSpaceMatrix() {

    std::vector<mat4> lightSpaceMatrix;
    std::vector<std::vector<vec4>> frustums;

    mat4 cameraView = camera.getView();
    for (int i = 0; i < shadowCascadeLevels_far.size(); i++) {
        mat4 projection = perspective(radians(camera.Fov), SCR_WIDTH / SCR_HEIGHT, shadowCascadeLevels_near[i], shadowCascadeLevels_far[i]);
        frustums.push_back(getFrustumCornersWorldSpace(projection, cameraView));
    }

    //计算每个子视锥体独特的光空间以减少渲染深度贴图的性能消耗。这里光空间正交投影长宽按子视锥体顶点最大小值决定，深度按所有物体最大小位置决定，因为如果只按顶点
    //最大小值决定，那么在视锥体之外的物体的深度无法映射到深度纹理中，那么可能无法得到正确的的深度来比较（如视锥体中的物体比视锥体外的物体深度大，但由于
    //视锥体外的物体的深度没有映射到，那么视锥体内的物体上的阴影就无法显示.
    for (int i = 0; i < frustums.size();i++) {

        vec3 center = vec3(0.0f);
        for (int j = 0; j < 8; j++) {
            center += vec3(frustums[i][j]);
        }
        center /= 8;

        mat4 lightViewMatrix = lookAt(lightDir + center, center, vec3(0.0f, 1.0f, 0.0f));

        //这里应该继续比较光空间下顶点与其他物体的深度大小，但是我现在无法的到物体深度（想要实现必须先记录物体顶点位置)。
        vec4 transf = lightViewMatrix * frustums[i][0];

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        for (int j = 0; j < 8; j++) {
            transf = lightViewMatrix * frustums[i][j];
            minX = std::min(minX, transf.x);
            maxX = std::max(maxX, transf.x);
            minY = std::min(minY, transf.y);
            maxY = std::max(maxY, transf.y);
            minZ = std::min(minZ, transf.z);
            maxZ = std::max(maxZ, transf.z);
        }
        //所有我们拉近与推远近远平面，来使深度贴图包含非视锥体内的物体，但可能任有误差
        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        lightSpaceMatrix.push_back(ortho(minX, maxX, minY, maxY, minZ, maxZ) * lightViewMatrix);
    }

    return lightSpaceMatrix;
}

void renderCube();

unsigned int planeVAO;
void renderScene(MyShader& shader)
{
    // floor
    glm::mat4 model;
    shader.setMaxtrix4fv("model", model);
    glBindVertexArray(planeVAO);
    glFrontFace(GL_CW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glFrontFace(GL_CCW);

    static std::vector<glm::mat4> modelMatrices;
    if (modelMatrices.size() == 0)
    {
        for (int i = 0; i < 10; ++i)
        {
            static std::uniform_real_distribution<float> offsetDistribution = std::uniform_real_distribution<float>(-10, 10);
            static std::uniform_real_distribution<float> scaleDistribution = std::uniform_real_distribution<float>(1.0, 2.0);
            static std::uniform_real_distribution<float> rotationDistribution = std::uniform_real_distribution<float>(0, 180);

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(offsetDistribution(generator), offsetDistribution(generator) + 10.0f, offsetDistribution(generator)));
            model = glm::rotate(model, glm::radians(rotationDistribution(generator)), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
            model = glm::scale(model, glm::vec3(scaleDistribution(generator)));
            modelMatrices.push_back(model);
        }
    }

    for (const auto& model : modelMatrices)
    {
        shader.setMaxtrix4fv("model", model);
        renderCube();
    }
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
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


int main() {
    //初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    //设定视锥体分为几块
    CalcShadowCascadeLevels(4);
    for (int i = 0; i < shadowCascadeLevels_far.size(); i++) {
        std::cout << shadowCascadeLevels_far[i] << std::endl;
        std::cout << shadowCascadeLevels_near[i] << std::endl;
    }

    unsigned int depth_FBO;
    glGenFramebuffers(1, &depth_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);

    unsigned int depthMapArray;
    glGenTextures(1, &depthMapArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution,(int)shadowCascadeLevels_far.size(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapArray,0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        throw 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int UBO;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4) * 16, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

    MyShader light_depth_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\csm_depth.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\csm_depth.fs");
    light_depth_shader.addGeoShader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\csm_depth.gs");
    MyShader scene_shader("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\csm_scene.vs", "C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textureSource\\csm_scene.fs");

    unsigned int wood_texture = loadTexture("C:\\D\\OpenGl\\openGL\\openGL\\opengl_4\\opengl_4\\textures\\container.jpg");

    mat4 projection = perspective(radians(camera.Fov), SCR_WIDTH / SCR_HEIGHT, cameraNearPlane, cameraFarPlane);
    scene_shader.use();
    scene_shader.setMaxtrix4fv("projection", projection);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 view = camera.getView();

        glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, depthMapResolution, depthMapResolution);

        std::vector<mat4> lightSpaceMatrices = getLightSpaceMatrix();
        for (int i = 0; i < lightSpaceMatrices.size(); i++) {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(mat4), sizeof(mat4),&lightSpaceMatrices[i]);
        }

        light_depth_shader.use();
        glCullFace(GL_FRONT);
        renderScene(light_depth_shader);
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        scene_shader.use();
        scene_shader.setMaxtrix4fv("view", view);
        scene_shader.setVec3("lightDir", lightDir);
        scene_shader.setVec3("viewPos", camera.Position);
        scene_shader.setFloat("farPlane", cameraFarPlane);
        for (int i = 0; i < shadowCascadeLevels_far.size(); i++) {
            scene_shader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels_far[i]);
        }
        scene_shader.setInt("cascadeCount", 4);

        glBindTexture(GL_TEXTURE_2D, wood_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapArray);
        scene_shader.setInt("shadowMap", 1);

        renderScene(scene_shader);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}