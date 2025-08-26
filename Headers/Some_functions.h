#pragma once
#include <iostream>
#include <glad/glad.h> 
#include <glfw/glfw3.h>
#include "stb_image.h"

unsigned int load_image(const char* image_path, int& width, int& height, int& nrChannels, const int packing = 4, const bool flip_vertically = true)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//this is used to flip the image vertically when loading it becouse on OpenGL the origin is at the
	//bottom left corner and increases upwards, while in most image formats the origin is at the top left corner
	//and increases downwards
	stbi_set_flip_vertically_on_load(flip_vertically);
	// load and generate the texture
	unsigned char* data = stbi_load(image_path, &width, &height, &nrChannels, 0);
	printf("Image loaded: %s, width: %d, height: %d, channels: %d\n", image_path, width, height, nrChannels);
	if (data)
	{
		//set the pixel storage mode "packing" byte alignment(default is 4 bytes)
		//we do this becosue the if image data is not aligned to 4 bytes it throw an error
		glPixelStorei(GL_UNPACK_ALIGNMENT, packing);

		GLenum format;
		switch (nrChannels)
		{
		case 1:  format = GL_RED;   break;
		case 2:  format = GL_RG;    break;
		case 3:  format = GL_RGB;   break;
		case 4:  format = GL_RGBA;  break;
		default: /* error */        break;
		}

		//GL_TEXTURE_2D - specifies the target texture / 0 - minimap level / format - the type we store the texture 
		//width, height - dimensions of the texture / 0 - legacy, always 0 / format - format of the pixel data
		//GL_UNSIGNED_BYTE - data type of the pixel data / data - pointer to the pixel data
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		return texture;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
		return 0;
	}

}

GLFWwindow* init_window(int width, int height, const char* window_name)
{
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	return window;
}