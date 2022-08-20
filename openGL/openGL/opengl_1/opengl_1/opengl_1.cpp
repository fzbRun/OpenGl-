#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>;

#define STB_IMAGE_IMPLEMENTATION
#include"../../stb-image-master/stb_image.h"

#include "../../OpenGL-Mathematics-master/glm/glm.hpp"
#include "../../OpenGL-Mathematics-master/glm/gtc/matrix_transform.hpp"
#include "../../OpenGL-Mathematics-master/glm/gtc/type_ptr.hpp"

#include "MyShader.h"
#include "MyCamera.h"

using namespace glm;

// 屏幕大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 相机
MyCamera camera(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 1.0f, 0.0f), 0.0f, -90.0f, 45.0f);
//时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//鼠标位置
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstmouse = true;

//光源方向
vec3 lightPos(1.2f, 1.0f, 2.0f);

//光源强度
float lightStrength = 0.2f;
float specularStrength = 0.5f;


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

unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChomponent;
    unsigned char* data = stbi_load(path, &width, &height, &nrChomponent,0);
    if (data) {
        GLenum format;
        if (nrChomponent == 1) {
            format = GL_RED;
        }else if (nrChomponent == 3) {
            format = GL_RGB;
        }
        else if (nrChomponent == 4) {
            format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else {
        std::cout << "texture fail to load at path" << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(800.0f, 600.0f, "learnOpengl", NULL, NULL);
    if (window == NULL) {
        std::cout << "FAIL TO CREATE WINDOW" << std::endl;
        glfwTerminate();
        return -1;
}
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "FAIL TO INITIAIZE GLAD" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);


    MyShader shaderProgram("C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\shaderSource\\vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\shaderSource\\somelight.fs");
    MyShader lightshaderProgram("C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\shaderSource\\lightvs.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\shaderSource\\lightfs.fs");


    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shaderProgram.use();
    /*
    shaderProgram.setVec3("light.ambient", vec3(lightStrength));
    shaderProgram.setVec3("light.diffuse", vec3(0.5f));
    shaderProgram.setVec3("light.specular", vec3(1.0f));
    //shaderProgram.setFloat("specularStrength", specularStrength);
    //shaderProgram.setVec3("light.position", lightPos);
    //shaderProgram.setVec3("light.worldDirection", vec3(-0.2f, -1.0f, -0.3f));
    shaderProgram.setVec3("light.color", vec3(1.0f, 1.0f, 1.0f));
    //shaderProgram.setVec3("objectColor", vec3(1.0f, 0.5f, 0.31f));
    shaderProgram.setFloat("light.constant", 1.0f);
    shaderProgram.setFloat("light.linear", 0.9f);
    shaderProgram.setFloat("light.quadratic", 0.032f);

    shaderProgram.setFloat("light.cutoff", cos(radians(12.5f)));
    shaderProgram.setFloat("light.outerCutoff", cos(radians(17.5f)));

    //shaderProgram.setVec3("material.ambient", vec3(1.0f, 0.5f, 0.31f));
    //shaderProgram.setVec3("material.diffuse", vec3(1.0f, 0.5f, 0.31f));
    */
    unsigned int diffuseMap = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\texture\\container2.png");
    shaderProgram.setInt("material.diffuse", 0);

    //shaderProgram.setVec3("material.specular", vec3(0.5f, 0.5f, 0.5f));
    unsigned int specularMap = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\texture\\container2_specular.png");
    shaderProgram.setInt("material.specular", 1);

    //unsigned int glowMap = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_1\\opengl_1\\texture\\matrix.jpg");
    //shaderProgram.setInt("material.glow", 2);

    shaderProgram.setFloat("material.shininess", 32.0f);
    //全局光照
    shaderProgram.setVec3("worldLight.direction", vec3(-0.2f,-0.1f,-0.3f));
    shaderProgram.setVec3("worldLight.color", vec3(1.0f));
    shaderProgram.setFloat("worldLight.strength", 0.1f);
    //点光源
    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
    };
    for (int i = 0; i < 4; i++) {
        stringstream ss;
        ss << "pointLights[" << i << "].position";
        shaderProgram.setVec3(ss.str(), pointLightPositions[i]);
        ss.str("");
        ss << "pointLights[" << i << "].constant";
        shaderProgram.setFloat(ss.str(),1.0f);
        ss.str("");
        ss << "pointLights[" << i << "].linear";
        shaderProgram.setFloat(ss.str(), 0.09f);        
        ss.str("");
        ss << "pointLights[" << i << "].quadratic";
        shaderProgram.setFloat(ss.str(), 0.032f);
        ss.str("");
        ss << "pointLights[" << i << "].color";
        shaderProgram.setVec3(ss.str(), vec3(1.0f));
    }
    //聚光
    shaderProgram.setVec3("flashLight.direction", camera.Front);
    shaderProgram.setVec3("flashLight.color", vec3(1.0f));
    shaderProgram.setFloat("flashLight.cutoff", cos(radians(12.5f)));
    shaderProgram.setFloat("flashLight.outerCutoff", cos(radians(17.5f)));
    shaderProgram.setFloat("flashLight.constant", 1.0f);
    shaderProgram.setFloat("flashLight.linear", 0.09f);
    shaderProgram.setFloat("flashLight.quadratic", 0.032f);

    shaderProgram.setVec3("cameraPos", camera.Position);
    shaderProgram.setFloat("ambientStrength", 0.1f);

    mat4 model;
    shaderProgram.setMaxtrix4fv("model", model);
    mat3 Gmodel = mat3(transpose(inverse(model)));
    shaderProgram.setMaxtrix3fv("Gmodel", Gmodel);
    shaderProgram.setVec3("cameraPos", camera.Position);

    lightshaderProgram.use();
    model = translate(model, lightPos);
    model = scale(model, vec3(0.2f));
    lightshaderProgram.setMaxtrix4fv("model", model);

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();
        mat4 project = perspective(radians(camera.Fov), 0.75f, 0.1f, 100.0f);
        mat4 view = camera.getView();
        shaderProgram.setMaxtrix4fv("projection", project);
        shaderProgram.setMaxtrix4fv("view", view);

        shaderProgram.setVec3("cameraPos", camera.Position);
        shaderProgram.setVec3("flashLight.direction", camera.Front);

        for (int i = 0; i < 10; i++) {

            mat4 model;
            model = translate(model, cubePositions[i]);
            model = rotate(model, radians(20.0f * i), vec3(1.0f, 0.3f, 0.5f));
            shaderProgram.setMaxtrix4fv("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

            //glActiveTexture(GL_TEXTURE2);
            //glBindTexture(GL_TEXTURE_2D, glowMap);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        /*
        vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        shaderProgram.setVec3("light.color", lightColor);
        //shaderProgram.setVec3("lightPos", vec3(view * vec4(lightPos,1.0f)));
        */


        lightshaderProgram.use();
        lightshaderProgram.setMaxtrix4fv("projection", project);
        lightshaderProgram.setMaxtrix4fv("view", view);
        for (int i = 0; i < 4; i++) {

            mat4 model;
            model = translate(model, pointLightPositions[i]);
            lightshaderProgram.setMaxtrix4fv("model", model);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
