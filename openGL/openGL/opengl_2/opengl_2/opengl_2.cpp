#include<iostream>

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
	
	//着色器及模型
	MyShader shader("C:\\Users\\fzb\\Desktop\\openGL\\opengl_2\\opengl_2\\shaderSource\\model.vs", "C:\\Users\\fzb\\Desktop\\openGL\\opengl_2\\opengl_2\\shaderSource\\model.fs");
	MyModel myModel("C:\\Users\\fzb\\Desktop\\openGL\\opengl_2\\opengl_2\\object\\nanosuit\\nanosuit.obj");


	//渲染
	while (!glfwWindowShouldClose(window)) {
		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		shader.use();
		mat4 projection = perspective(radians(camera.Fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.getView();
		shader.setMaxtrix4fv("projection", projection);
		shader.setMaxtrix4fv("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	
		shader.setMaxtrix4fv("model", model);

		myModel.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}