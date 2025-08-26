//#include <glad/glad.h> 
//#include <glfw/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include "stb_image.h"
//
//#include "Shader.h"
//
//const unsigned int width = 800, height = 600;
//const float aspect_ratio = (float)width / (float)height;
//
//GLFWwindow* init_window(int width, int height, const char* window_name)
//{
//	//glfwInit();
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
//unsigned int load_image(const char* image_path, int &width, int &height, int &nrChannels)
//{
//	unsigned int texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	// set the texture wrapping/filtering options (on the currently bound texture object)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	//this is used to flip the image vertically when loading it becouse on OpenGL the origin is at the
//	//bottom left corner and increases upwards, while in most image formats the origin is at the top left corner
//	//and increases downwards
//	stbi_set_flip_vertically_on_load(true);
//	// load and generate the texture
//	unsigned char* data = stbi_load(image_path, &width, &height, &nrChannels, 0);
//	printf("Image loaded: %s, width: %d, height: %d, channels: %d\n", image_path, width, height, nrChannels);
//	if (data)
//	{
//		//set the pixel storage mode to 1 byte alignment(default is 4 bytes)
//		//we do this becosue the if image data is not aligned to 4 bytes it throw an error
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//		//GL_TEXTURE_2D - specifies the target texture / 0 - minimap level / GL_RGB - the type we store the texture 
//		//width, height - dimensions of the texture / 0 - legacy, always 0 / GL_RGB - format of the pixel data
//		//GL_UNSIGNED_BYTE - data type of the pixel data / data - pointer to the pixel data
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//
//		glGenerateMipmap(GL_TEXTURE_2D);
//		stbi_image_free(data);
//		return texture;
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//		stbi_image_free(data);
//		return 0;
//	}
//
//}
//
//constexpr float PI = 3.14159265358979323846f;
//
//void get_coordinates_from_angle(float &x, float &z, const float angle)
//{
//	x = std::cos(angle);
//	z = std::sin(angle);
//}
//
//void framebuffer_size_callback(GLFWwindow * window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//
//	const float new_aspect_ratio = (float)width / (float)height;
//	if(new_aspect_ratio < aspect_ratio)
//	{
//		//if the new aspect ratio is smaller than the original one, we need to adjust the viewport
//		int new_height = (int)(width / aspect_ratio);
//		glViewport(0, (height - new_height) / 2, width, new_height);
//	}
//	else
//	{
//		//if the new aspect ratio is larger than the original one, we need to adjust the viewport
//		int new_width = (int)(height * aspect_ratio);
//		glViewport((width - new_width) / 2, 0, new_width, height);
//	}
//}
//
//int main()
//{
//	GLFWwindow* window = init_window(width, height, "Shader Tester");
//	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//
//	if (!window) {
//		return -1;
//	}
//
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	Shader shader("Shaders\\Vertex_shaders\\Basic_texture_vertex.vert",
//		"Shaders\\Fragment_shaders\\Basic_texture_fragment.frag");
//
//	Shader shader_black("Shaders\\Vertex_shaders\\Basic_texture_vertex.vert",
//		"Shaders\\Fragment_shaders\\Black_fragment.frag");
//
//	//const float halflenght = sqrt(2.0f) / 2.0f;
//	const float halflenght = 0.3f;
//	float vertices[] = {
//		// positions					// colors           // texture coords
//		 0.3f,  -halflenght, 0.3f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
//		-0.3f,  -halflenght, 0.3f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
//		-0.3f,  -halflenght,-0.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // up left
//		 0.3f,  -halflenght,-0.3f,   0.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // up right
//		 0.0f,   halflenght, 0.0f,   1.0f, 1.0f, 1.0f,   0.5f, 0.5f,   // middle
//	};
//
//	/*float angles[4] = {
//	PI / 2.0f,
//	PI,
//	3.0f * PI / 2.0f,
//	2.0f * PI,
//	};*/
//
//	unsigned int indices[] = {
//		4, 1, 2,
//		4, 2, 3,
//		4, 3, 0,
//		4, 0, 1
//	};
//
//
//	float rod_vertices[] = {
//		// positions			// colors           // texture coords
//		 0.01f,  -1.0, 0.0f,   1.0f, 1.0f, 1.0f,   0.5f, 0.5f,   // bottom right
//		-0.01f,  -1.0, 0.0f,   1.0f, 1.0f, 1.0f,   0.5f, 0.5f,   // bottom left
//		 0.01f,   1.0, 0.0f,   1.0f, 1.0f, 1.0f,   0.5f, 0.5f,   // top right
//		-0.01f,   1.0, 0.0f,   1.0f, 1.0f, 1.0f,   0.5f, 0.5f    // top left
//	};
//
//	unsigned int rod_indices[] = {
//		0, 1, 2,
//		1, 3, 2
//	};
//
//
//	unsigned int VBO, VBO_rod, VAO, VAO_rod, EBO, EBO_rod;
//	glGenBuffers(1, &VBO);			glGenBuffers(1, &VBO_rod);
//	glGenVertexArrays(1, &VAO);		glGenVertexArrays(1, &VAO_rod);
//
//	glBindVertexArray(VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	
//	glGenBuffers(1, &EBO);		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	glBindVertexArray(VAO);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);// position attribute
//
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);// color attribute
//
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glEnableVertexAttribArray(2);// texture coordinate attribute
//
//
//	glBindVertexArray(VAO_rod);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO_rod);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(rod_vertices), rod_vertices, GL_STATIC_DRAW);
//
//	glGenBuffers(1, &EBO_rod);	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rod);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rod_indices), rod_indices, GL_STATIC_DRAW);
//
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);// position attribute
//
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);// color attribute
//
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glEnableVertexAttribArray(2);// texture coordinate attribute
//
//
//	int width, height, nrChannels;
//	unsigned int obama_texture = load_image("Textures\\random\\obam4.jpg", width, height, nrChannels);
//	unsigned int piramit_texture = load_image("Textures\\random\\piramit.png", width, height, nrChannels);
//
//	shader.use();
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, obama_texture);
//	shader.setInt("Texture_1", 0);
//
//	glActiveTexture(GL_TEXTURE0 + 1);
//	glBindTexture(GL_TEXTURE_2D, piramit_texture);
//	shader.setInt("Texture_2", 1);
//
//	glEnable(GL_DEPTH_TEST);
//	float angle_x = 0.0f;
//	glm::mat4 model_rod = glm::mat4(1.0f);
//	while (!glfwWindowShouldClose(window))
//	{
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		float scale = 0.8f + (0.3f * std::sin(angle_x));
//
//		glm::mat4 model = glm::mat4(1.0f);
//
//		//model = glm::scale(model, glm::vec3(scale, scale, scale));
//		model = glm::rotate(model, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
//		model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0.3f));
//		model = glm::rotate(model, angle_x * 2, glm::vec3(0.0f, 1.0f, 0.0f));
//
//
//		glm::mat4 model_2 = glm::mat4(1.0f);
//
//		//model_2 = glm::scale(model_2, glm::vec3(scale, scale, scale));
//		model_2 = glm::rotate(model_2, -angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
//		model_2 = glm::translate(model_2, glm::vec3(-0.5f, 0.5f, -0.3f));
//		model_2 = glm::rotate(model_2, -angle_x * 2, glm::vec3(0.0f, 1.0f, 0.0f));
//
//
//		angle_x += 0.01f;
//
//		shader.use();
//		shader.setMatrix4fv("model", glm::value_ptr(model));
//		glBindVertexArray(VAO);
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//
//		shader.setMatrix4fv("model", glm::value_ptr(model_2));
//		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//
//
//
//		shader_black.use();
//		shader_black.setMatrix4fv("model", glm::value_ptr(model_rod));
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glBindVertexArray(VAO_rod);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//
//
//		shader_black.setMatrix4fv("model", glm::value_ptr(model));
//		glBindVertexArray(VAO);
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//
//		shader_black.setMatrix4fv("model", glm::value_ptr(model_2));
//		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//
//
//		/*for (int i = 0; i < 4; i++)
//		{
//			get_coordinates_from_angle(vertices[0 +(i*8)], vertices[2 + (i*8)], angles[i]);
//			angles[i] += 0.01f;
//		}
//
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);*/
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//}
//
