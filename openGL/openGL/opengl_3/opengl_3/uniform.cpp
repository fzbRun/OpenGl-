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

int main() {
    //初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

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
    /*
    float cubeVertices[] = {
        // positions         
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    float fragfaceVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };

    float points[] = {
    -0.5f,  0.5f, // 左上
     0.5f,  0.5f, // 右上
     0.5f, -0.5f, // 右下
    -0.5f, -0.5f  // 左下
    };

    float homepoints[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
    };

    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    vec2 offset[100];
    int index = 0;
    for (int i = -10; i < 10; i += 2) {
        for (int j = -10; j < 10; j += 2) {
            offset[index].y = (float)i / 10 + 0.1f;
            offset[index].x = (float)j / 10 + 0.1f;
            index++;
        }
    }
    */

     GLfloat cubeVertices[] = {
        // Positions       
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f
    };

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    // Setup cube VAO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    /*
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    //当只需要直接输入数据如更新数据等时，我们不再需要指定数据大小，以及方式。我们可以使用glMapBuffer函数来直接输入数据
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    mencpy(ptr,cubeVertices,sizeof(cubeVertices));
    if(glUnmapBuffer(GL_ARRAY_BUFFER) != GL_TURE){
        std::cout << "数据更新失败" << std::endl;   
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    */
    /*
    //如果有多种数据（如位置，法线，纹理），我们可以不将之拼成一个数组，我们可以直接为三个数组调用方法处理
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normal), &normal);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions)+sizeof(nnormal),sizeof(TexCoords), &TexCoords);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(positions)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(positions) + sizeof(normals)));
    glEnableVertexAttribArray(2);
    */
    /*
    //我们也可以将一个顶点幻宠对象的数据复制到另一个顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER,VBO1);或者glBindBuffer(GL_COPY_READ_BUFFER,VBO1）；
    glBindBuffer(GL_COPY_WRITE_BUFFER,VBO2);
    glCopyBufferSubData(GL_ARRAY_BUFFER\GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,0,sizeof(数据大小));
    */

    //事实证明顶点着色器中修改电大小好像无效
    //glEnable(GL_PROGRAM_POINT_SIZE);
    //MyShader pointSize_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\pointSize.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\pointSize.fs");
    /*
    unsigned int faceVAO, faceVBO;
    glGenVertexArrays(1, &faceVAO);
    glGenBuffers(1, &faceVBO);
    glBindVertexArray(faceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fragfaceVertices), &fragfaceVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    unsigned int homeVAO, homeVBO;
    glGenVertexArrays(1, &homeVAO);
    glGenBuffers(1, &homeVBO);
    glBindVertexArray(homeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, homeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(homepoints), &homepoints, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    unsigned int insVAO, insVBO1,insVBO2;
    glGenVertexArrays(1, &insVAO);
    glBindVertexArray(insVAO);
    glGenBuffers(1, &insVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, insVBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &insVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, insVBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*100, &offset, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    */

    //测试物体颜色随屏幕坐标而改变
    //MyShader fragCoord_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\pointSize.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\fragCoord.fs");

    //测试物体正反面判断
    //MyShader fragfaceCoord_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\depth_vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\frontFace.fs");
    //unsigned int frontFace_texture = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\container.jpg");
    //unsigned int backFace_texture = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\back.jpg");

    //MyShader fragdepth_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\depth_vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\fragdepth.fs");

    
    //MyShader uniform_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\uniform.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\uniform.fs");

    //MyShader line_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\ver_line.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\fra_line.fs");
    //line_shader.addGeoShader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\geo_line.gs");
    //MyShader home_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\ver_home.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\fra_home.fs");
    //home_shader.addGeoShader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\geo_home.gs");
    /*
    MyShader model_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model2.fs");
    model_shader.addGeoShader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model.gs");
    MyShader model_normal_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model_normal.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model_normal.fs");
    model_normal_shader.addGeoShader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model_normal.gs");
    MyModel obj_model("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\object\\nanosuit-rec\\nanosuit.obj");
    */
    //MyShader ins_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\intance.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\intance.fs");
    /*
    MyShader planet_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\planet.fs");
    MyShader rock_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model_star.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\rock.fs");
    MyModel planet("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\object\\planet\\planet.obj");
    MyModel rock("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\object\\rock\\rock.obj");

    unsigned int UBO;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(mat4));

    unsigned int shader_camera = glGetUniformBlockIndex(planet_shader.ID, "camera");
    glUniformBlockBinding(planet_shader.ID, shader_camera, 0);
    glUniformBlockBinding(rock_shader.ID, shader_camera, 0);

    mat4 projection = perspective(camera.Fov, SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(projection));

    int num = 10000;
    mat4* rock_models;
    rock_models = new mat4[num];
    srand(glfwGetTime());
    float radius = 100.0;
    float offset = 25.0f;
    for (int i = 0; i < num; i++) {
        mat4 model;
        float angle = (float)i / (float)num * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = translate(model, vec3(x, y, z));

        float scalesize = (rand() % 20) / 100.0f + 0.05f;
        model = scale(model, vec3(scalesize));

        float rotAngle = (rand() % 360);
        model = rotate(model, rotAngle,vec3(0.4f,0.6f,0.8f));
        
        rock_models[i] = model;
    }

    unsigned int starVBO;
    glGenBuffers(1, &starVBO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, num * sizeof(mat4), &rock_models[0],GL_STATIC_DRAW);
    for (int i = 0; i < rock.meshes.size(); i++) {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)sizeof(vec4));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(2* sizeof(vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(3* sizeof(vec4)));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }

    unsigned int rock_texture = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\object\\rock\\Rock-Texture-Surface.jpg");
    */

    MyShader MASS_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\MASS.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\MASS.fs");
    MyShader buffer_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\buffer_vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\buffer_framgent.fs");

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);

    unsigned int TBO;
    glGenTextures(1, &TBO);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TBO);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, 800, 600, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, TBO, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int FBO2;
    glGenFramebuffers(1, &FBO2);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO2);

    unsigned int textureBuffer;
    glGenTextures(1, &textureBuffer);
    glBindTexture(GL_TEXTURE_2D, textureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //将深度，模板按24，8的精度绑定到纹理缓冲上
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "创建帧缓存失败" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    while (!glfwWindowShouldClose(window)){
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        mat4 projection = perspective(camera.Fov, SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        mat4 view = camera.getView();
        mat4 model;

        /*
        pointSize_shader.use();
        pointSize_shader.setMaxtrix4fv("projection", projection);
        pointSize_shader.setMaxtrix4fv("view", view);
        pointSize_shader.setMaxtrix4fv("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        */

        /*
        fragCoord_shader.use();
        fragCoord_shader.setMaxtrix4fv("projection", projection);
        fragCoord_shader.setMaxtrix4fv("view", view);
        fragCoord_shader.setMaxtrix4fv("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        */
        /*
        fragfaceCoord_shader.use();
        fragfaceCoord_shader.setMaxtrix4fv("projection", projection);
        fragfaceCoord_shader.setMaxtrix4fv("view", view);
        fragfaceCoord_shader.setMaxtrix4fv("model", model);
        fragfaceCoord_shader.setInt("frontTexture", 0);
        fragfaceCoord_shader.setInt("backTexture", 1);
        glBindTexture(GL_TEXTURE_2D, frontFace_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, backFace_texture);
        glBindVertexArray(faceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        fragdepth_shader.use();
        fragdepth_shader.setMaxtrix4fv("projection", projection);
        fragdepth_shader.setMaxtrix4fv("view", view);
        fragdepth_shader.setMaxtrix4fv("model", model);
        glBindVertexArray(faceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        */
        
        /*
        uniform_shader.use();
        uniform_shader.setMaxtrix4fv("model", model);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(view));
        glBindTexture(GL_TEXTURE_2D, frontFace_texture);
        glBindVertexArray(faceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        */

        //line_shader.use();
        //glBindVertexArray(pointVAO);
        //glDrawArrays(GL_POINTS, 0, 4);

        //home_shader.use();
        //glBindVertexArray(homeVAO);
        //glDrawArrays(GL_POINTS, 0, 4);
        /*
        model_shader.use();
        model_shader.setFloat("time", currentFrame);
        model_shader.setMaxtrix4fv("model", model);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(view));
        obj_model.Draw(model_shader);

        model_normal_shader.use();
        model_normal_shader.setMaxtrix4fv("model", model);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(view));
        obj_model.Draw(model_normal_shader);
        */
        /*
        ins_shader.use();
        glBindVertexArray(insVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
        */

        /*
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(view));

        planet_shader.use();
        glBindVertexArray(0);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        planet_shader.setMaxtrix4fv("model", model);
        planet.Draw(planet_shader);

        rock_shader.use();
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, rock_texture);
        for (int i = 0; i < rock.meshes.size(); i++) {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indice.size(), GL_UNSIGNED_INT, 0, num);
        }
        */

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        MASS_shader.use();
        MASS_shader.setMaxtrix4fv("projection", projection);
        MASS_shader.setMaxtrix4fv("view", view);
        MASS_shader.setMaxtrix4fv("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO2);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        buffer_shader.use();
        glBindTexture(GL_TEXTURE_2D, textureBuffer);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}