#include <iostream>

#include"glad/glad.h"//！！！注意这里，/前面的是h文件的上级目录，可能每个人安装不同，也有不同
#include"GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb-image-master/stb_image.h"

#include "../../OpenGL-Mathematics-master/glm/glm.hpp"
#include "../../OpenGL-Mathematics-master/glm/gtc/matrix_transform.hpp"
#include "../../OpenGL-Mathematics-master/glm/gtc/type_ptr.hpp"

#include "MyShader.h"
#include "MyCamera.h"


using namespace std;

//摄像机
MyCamera* camera = new MyCamera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -90.0f, 45.0f);

//帧时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//鼠标坐标
bool firstmouse = true;
float lastX = 400.0f;
float lastY = 300.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	float currentTime = glfwGetTime();
	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->cameraKeyMove(FORWARD, deltaTime);
	}	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->cameraKeyMove(BACKWARD, deltaTime);
	}
	if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
		camera->cameraKeyMove(LEFT, deltaTime);
	}
	if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
		camera->cameraKeyMove(RIGHT, deltaTime);
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

	camera->cameraMouseMove(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double Xoffset, double Yoffset) {
	camera->cameraScroll(Yoffset);
}

/*
//顶点着色器源码
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor; \n"
	"out vec3 vertexColor;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0f);\n"
	"	vertexColor = aColor;\n"
	"}\0";


//片段着色器源码
const char *fragmentShaderSource = "#version 330 core\n"
	"in vec3 vertexColor;\n"
	//"uniform vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(vertexColor,1.0f);\n"
    "}\n\0";
*/

int main() {

	//glfw初始化，设置版本
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//初始化窗口
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Fail to create glfw window" << endl;
		glfwTerminate();
		return -1;
	}
	//将窗口设置为glfw上下文
	glfwMakeContextCurrent(window);
	//将当前glfw地址传给glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Fail to initialize GLAD" << endl;
		return -1;
	}
	//设置视口大小
	glViewport(0, 0, 800, 600);
	//当窗口大小变换时，改变视口大小
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	/*
	//顶点着色器（类型）
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//将源码附着到顶点着色器上,并编译
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//判断编译是否成功
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << endl;
	}

	//片段着色器（类型）
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//将源码附着到片段着色器上,并编译
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//判断编译是否成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		cout << "ERROR::SHADER::FRAMGENT::COMPILATION_FAILED\n" << infolog << endl;
	}

	//创建着色器连接对象
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//判断连接是否成功
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		cout << "ERROR::SHADER::ShaderProgram::COMPILATION_FAILED\n" << infolog << endl;
	}
	//连接成功后删除着色器(已不再需要的)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	*/
	
	MyShader shaderProgram("C:\\Users\\fzb\\Desktop\\openGL\\openGL_0\\openGL_0\\ShaderSource\\opengl_0\\vertex_0.vs","C:\\Users\\fzb\\Desktop\\openGL\\openGL_0\\openGL_0\\ShaderSource\\opengl_0\\fragment_0.fs");

	//顶点输入
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
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

	/*float vertices[] = {
	0.5f, 0.5f, 0.0f,   // 右上角
	0.5f, -0.5f, 0.0f,  // 右下角
	-0.5f, -0.5f, 0.0f, // 左下角
	-0.5f, 0.5f, 0.0f   // 左上角
	};

	unsigned int indices[] = { // 注意索引从0开始! 
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};
	*/

	//缓冲对象
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//定义顶点数组对象
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//解析顶点数据
	//位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//颜色
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(1);
	//纹理
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//定义索引缓冲对象
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//绑定缓存对象与数组对象
	//glBindBuffer(GL_ARRAY_BUFFER,0);
	//glBindVertexArray(0);

	//防止纹理上下颠倒
	stbi_set_flip_vertically_on_load(true);

	//生成纹理
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	//为当前绑定的纹理对象设置环绕，过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("C:\\Users\\fzb\\Desktop\\openGL\\openGL_0\\openGL_0\\photo\\container.jpg", &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "FAIL TO LOAD TEXTURE" << endl;
	}

	stbi_image_free(data);	
	
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//为当前绑定的纹理对象设置环绕，过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	data = stbi_load("C:\\Users\\fzb\\Desktop\\openGL\\openGL_0\\openGL_0\\photo\\awesomeface.png", &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "FAIL TO LOAD TEXTURE" << endl;
	}

	stbi_image_free(data);

	shaderProgram.use();
	shaderProgram.setInt("texture1", 0);
	shaderProgram.setInt("texture2", 1);
	shaderProgram.setFloat("alpha", 0.2);

	//变换
	/*
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
	shaderProgram.setMaxtrix4fv("transform", trans);
	*/



	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	//捕捉光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//监察按键
		processInput(window);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		//清空缓冲区颜色
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//绑定纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//渲染环节
		shaderProgram.use();
		//glUseProgram(shaderProgram);
		//动态改变颜色
		/*float timeValue = glfwGetTime();
		float greenValue = sin(timeValue)/0.2f + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "vertexColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		*/
		//绘制图形

		//随时间旋转图形
		//glm::mat4 model;
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		//shaderProgram.setMaxtrix4fv("transform", model);
		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 1.0f));
		//shaderProgram.setMaxtrix4fv("transform", trans);



		glBindVertexArray(VAO);
		
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
		
		//观察矩阵
		//float radius = 10.0f;
		//float cameX = sin(glfwGetTime()) * radius;
		//float cameZ = cos(glfwGetTime()) * radius;
		glm::mat4 view;
		//view = glm::lookAt(glm::vec3(cameX, 0.0f, cameZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		view = camera->getView();
		shaderProgram.setMaxtrix4fv("view",view);

		//投影矩阵
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera->Fov), 0.75f, 0.1f, 100.0f);
		shaderProgram.setMaxtrix4fv("projection", projection);

		for (unsigned int i = 0; i < 10; i++) {
			//模型矩阵
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0 * i;
			model = glm::rotate(model, glm::radians(angle),glm::vec3(1.0f,0.3f,0.5f));
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			shaderProgram.setMaxtrix4fv("transform", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//交替缓冲区
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	//glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}
