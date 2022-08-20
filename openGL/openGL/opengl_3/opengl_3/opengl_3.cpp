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
float lastX = SCR_WIDTH/2;
float lastY = SCR_HEIGHT / 2;

void framebuffer_size_callback(GLFWwindow* window,int width,int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.cameraKeyMove(FORWARD,deltaTime);
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

void mouse_callback(GLFWwindow* window,double xpos,double ypos){
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

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset) {
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
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels,0);
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

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learnOpengl", NULL, NULL);
	if (window == NULL) {
		std::cout << "window fail to create" << std::endl;
		glfwTerminate();
		return - 1;
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
    /*
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //规定模板缓冲的写入时刻与值
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    */
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    //设置剔除那一面
    glCullFace(GL_BACK);
    //设置逆时针为正面，默认也是逆时针正面
    //glFrontFace(GL_CCW);

    //MyShader depth_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\depth_vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\depth_framgent.fs");
    /*
    MyShader sten_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\sten_vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\sten_framgent.fs");
    //MyShader sten_shader_big("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\sten_vertex_big.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\sten_framgent_big.fs");

    MyShader blend_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\blend_vertex.vs","C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\blend_framgent.fs");
    
    MyShader buffer_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\buffer_vertex.vs","C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\buffer_framgent.fs");
    */
    MyShader skybox_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\skybox_vertex.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\skybox_framgent.fs");

    MyShader model_shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\shaderSource\\model.fs");

    MyModel obj_model("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\object\\nanosuit-rec\\nanosuit.obj");
    /*
    //面剔除，逆时针为正面，顺时针为背面
    float cubeVertices[] = {
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
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    float windowVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    vector<glm::vec3> windows
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3(1.5f, 0.0f, 0.51f),
        glm::vec3(0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3(0.5f, 0.0f, -0.6f)
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
    */
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    /*
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0); 
    
    // window VAO
    unsigned int windowVAO, windowVBO;
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);
    glBindVertexArray(windowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), &windowVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
    */
    // cube VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // load textures
    // -------------
    /*
    unsigned int cubeTexture = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\container.jpg");
    unsigned int floorTexture = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\container.jpg");
    unsigned int windowTexture = loadTexture("C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\window.png");
    */
    vector<std::string> faces
    {
        "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\right.jpg",
        "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\left.jpg",
        "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\top.jpg",
        "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\bottom.jpg",
        "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\front.jpg",
        "C:\\Users\\fzb\\Desktop\\openGL\\opengl_3\\opengl_3\\textures\\back.jpg"
    };
    unsigned int cubeMap = loadCudeMap(faces);

    //纹理缓冲对象可以对帧缓存读取和写入
    //渲染缓冲对象可以将渲染数据直接转换为原生格式，不需要对纹理类型进行转换，所以交换缓冲非常迅速，写入非常快，但读取效率低
    //所以频繁读取深度或模板缓冲数据，用纹理缓冲，否则用渲染缓冲
    
    //创建纹理帧缓存
    /*
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int textureBuffer;
    glGenTextures(1, &textureBuffer);
    glBindTexture(GL_TEXTURE_2D, textureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //将深度，模板按24，8的精度绑定到纹理缓冲上
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer,0);

    //创建渲染缓冲对象
    unsigned int RBO;
    glGenFramebuffers(1, &RBO);
    glBindFramebuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "创建帧缓存失败" << std::endl;
    }
    */
    //计算透明物体的向后位置，后面的物体先渲染，以免被前面物体深度判断后丢除
    /*
    std::map<float, vec3> windows_sored;
    for (int i = 0; i < windows.size(); i++) {
        float distance = length(camera.Position - windows[i]);
        windows_sored[distance] = windows[i];
    }
    */
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
            // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        /*
        sten_shader_big.use();
        sten_shader_big.setMaxtrix4fv("view", view);
        sten_shader_big.setMaxtrix4fv("projection", projection);
        */
        /*
        sten_shader.use();
        sten_shader.setMaxtrix4fv("view", view);
        sten_shader.setMaxtrix4fv("projection", projection);

        blend_shader.use();
        blend_shader.setMaxtrix4fv("view", view);
        blend_shader.setMaxtrix4fv("projection", projection);
        */
        /*
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        depth_shader.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        depth_shader.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        depth_shader.setMaxtrix4fv("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        */

        /*
        //模板缓冲写入方式为Func后两位参数相乘与模板缓冲值比较，如果与参数1相同，则通过op函数判断写入。如果为replcae则将第二位参数与Mask相乘写入模板函数。
        glStencilMask(0X00);

        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        sten_shader.setMaxtrix4fv("model", mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glStencilMask(0XFF);
        glStencilFunc(GL_ALWAYS, 1, 0XFF);
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        sten_shader.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        sten_shader.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilMask(0X00);
        glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
        glDisable(GL_DEPTH_TEST);
        sten_shader_big.use();
        float scale = 1.1f;
        // cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        sten_shader_big.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        sten_shader_big.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
        */
        
        //使用自定义帧缓存
        //glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        /*
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        sten_shader.use();
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        sten_shader.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        sten_shader.setMaxtrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        sten_shader.setMaxtrix4fv("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        //window
        glBindVertexArray(windowVAO);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        blend_shader.use();
        for (std::map<float,vec3>::reverse_iterator it = windows_sored.rbegin(); it != windows_sored.rend(); it++) {
            mat4 model;
            model = translate(model, it->second);
            blend_shader.setMaxtrix4fv("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        */

        model_shader.use();
        model_shader.setMaxtrix4fv("projection", projection);
        model_shader.setMaxtrix4fv("view", view);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model_shader.setMaxtrix4fv("model", model);
        obj_model.Draw(model_shader);
            
        
        glDepthFunc(GL_LEQUAL);
        skybox_shader.use();
        skybox_shader.setMaxtrix4fv("projection", projection);
        mat4 skyview = mat4(mat3(camera.getView()));
        skybox_shader.setMaxtrix4fv("view", skyview);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        /*
        //帧缓存会将渲染的场景以纹理的形式输出，我们再用一个视口大小的正方形展示，即可显示场景
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        buffer_shader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        */


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &cubeVAO);
    //glDeleteVertexArrays(1, &planeVAO);
    //glDeleteBuffers(1, &cubeVBO);
    //glDeleteBuffers(1, &planeVBO);
    //glDeleteFramebuffers(1, &FBO);

    glfwTerminate();
    return 0;
}