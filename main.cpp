//#include "Headers/Globals.h"
//#include <iostream>

//
//GLFWwindow* init_window(int width, int height, const char* window_name)
//{
//	glfwInit();
//	if (!glfwInit()) {
//		std::cerr << "Failed to initialize GLFW" << std::endl;
//		return nullptr;
//	}
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//
//	GLFWwindow* window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);
//	if (!window) {
//		std::cerr << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return nullptr;
//	}
//	
//	glfwMakeContextCurrent(window);
//	
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return nullptr;
//	}
//
//	return window;
//}
//
//void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
//    int i = int(h * 6);
//    float f = h * 6 - i;
//    float p = v * (1 - s);
//    float q = v * (1 - f * s);
//    float t = v * (1 - (1 - f) * s);
//
//    switch (i % 6) {
//    case 0: r = v, g = t, b = p; break;
//    case 1: r = q, g = v, b = p; break;
//    case 2: r = p, g = v, b = t; break;
//    case 3: r = p, g = q, b = v; break;
//    case 4: r = t, g = p, b = v; break;
//    case 5: r = v, g = p, b = q; break;
//    }
//}
//
//void glfw_error_callback(int error, const char* description) {
//	std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
//}
//
//void processInput(GLFWwindow* window)
//{
//	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//	{
//		glfwSetWindowMonitor(
//			window,
//			nullptr,
//			0, 0,                        // xpos, ypos (top-left corner)
//			800,600,  // fullscreen resolution
//			0           // refresh rate
//		);
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
//	{
//		glfwSetWindowMonitor(
//			window,
//			monitor,
//			0, 0,                        // xpos, ypos (top-left corner)
//			mode->width, mode->height,  // fullscreen resolution
//			mode->refreshRate           // refresh rate
//		);
//	}
//
//	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
//		glClearColor(0, 0, 1, 1.0f);
//
//}
//
//int main() {
//	std::cout << "Hello, World!" << std::endl;
//	
//	GLFWwindow* window = init_window(800, 600, "GLFW Window Opacity Example");
//
//	if (!window) {
//		return -1;
//	}
//	
//	float y = 0.0;
//	bool way = false;
//	float hue_speed = 0.002f;
//	float hue = 0.0f;
//	float r, g, b;
//
//
//	while (!glfwWindowShouldClose(window)) {
//		float x = glfwGetWindowOpacity(window) + (way ? y : -y);
//		if(x>= 1.0)
//		{
//			x = 1.0;
//			way = false;
//		}
//		else if(x<=0.0)
//		{
//			x = 0.0;
//			way = true;
//		}
//
//		processInput(window);
//
//		glfwSetWindowOpacity(window, x);
//
//		hue += hue_speed;
//		if (hue > 1.0f) hue -= 1.0f;
//
//		
//		HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
//
//		glClearColor(r, g, b, 1.0f);
//		
//		glClear(GL_COLOR_BUFFER_BIT);
//		
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	return 0;
//}