//#include <glad/glad.h> 
//#include <iostream>
//#include <glfw/glfw3.h>
////In order for OpenGL to use the shader it has to dynamically compile it at run-time from its source code. 
//const char* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 1) in vec3 aColor;\n"
//"out vec3 ourColor;\n"
//"void main()\n"
//"{\n"
//"   gl_Position = vec4(aPos, 1.0);\n"
//"   ourColor = aColor;\n"
//"}\0";
//
//const char* fragmentShaderSource = "#version 330 core\n"
//"in vec3 ourColor;\n"
//"out vec4 FragColor;\n"
//"void main()\n"
//"{\n"
//	"FragColor = vec4(ourColor, 1.0f);\n"
//"}\n";
//
//constexpr float PI = 3.14159265358979323846f;
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
//void init_shaders(float vertices[], int vertices_size, unsigned int &shaderProgram, unsigned int& VAO ,unsigned int& VBO)
//{
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glGenBuffers(1, &VBO);//create buffer in GPU
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);//bind buffer to gl_array_buffer
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_size, vertices, GL_DYNAMIC_DRAW);//push data to buffer
//
//	//GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
//	//GL_STATIC_DRAW : the data is set only once and used many times.
//	//GL_DYNAMIC_DRAW : the data is changed a lot and used many times.
//
//	
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glCompileShader(vertexShader);
//
//	int  success;
//	char infoLog[512];
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//
//	if (!success)
//	{
//		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//		return;
//	}
//
//
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//	glCompileShader(fragmentShader);
//
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//	if (!success)
//	{
//		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//		return;
//	}
//
//	//linking shader togather
//	shaderProgram = glCreateProgram();
//
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//	glLinkProgram(shaderProgram);
//
//	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//	if (!success) {
//		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::LINKING::COMPILATION_FAILED\n" << infoLog << std::endl;
//		return;
//	}
//
//	//we no longer need them anymore
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//
//	//0-attribute we wanto configure / 3-vertex size(3 floats, vec3) / GL_FLOAT - data type
//	//GL_FALSE - do we want to normalize data? / 6 * sizeof(float) - stride, space between vertex's 
//	// (void*)0 - This is the offset of where the position data begins in the buffer
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//
//	//Now that we specified how OpenGL should interpret the vertex data we should also enable the vertex attribute
//	//giving the vertex attribute location as its argument;
//	glEnableVertexAttribArray(0);
//
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//}
//
//void turn_around_y_depricated(float &x, float &z,const float stepsize, bool clocwise = true)
//{
//	if (z < 0.0f || (z == 0 && x >= 0))
//	{
//		x = x - (clocwise ? stepsize : -stepsize);
//		
//		if (x <= -1.0f)
//		{
//			x = -1.0f;
//			z = 0;
//		}
//		else
//			z = -std::sqrt(1.0f - (x * x));
//	}
//	else if (z >= 0.0f)
//	{
//		x = x + (clocwise ? stepsize : -stepsize);
//		
//		if (x >= 1.0f)
//		{
//			x = 1.0f;
//			z = 0;
//		}
//		else
//			z = std::sqrt(1.0f - (x * x));
//	}
//
//	
//	
//}
//
//void get_coordinates_from_angle(float &x, float &z, const float angle)
//{
//	x = std::cos(angle);
//	z = std::sin(angle);
//}
//
//int main() {
//	std::cout << "Hello, World!" << std::endl;
//
//	GLFWwindow* window = init_window(800, 600, "GLFW My first triangle");
//
//	if (!window) {
//		return -1;
//	}
//
//	glEnable(GL_DEPTH_TEST);
//
//	const float starting_coordinate = std::sin(3.14f / 4.0f); // 0.7071f
//
//	float angles[4] = {
//	PI / 2.0f,
//	PI,
//	3.0f * PI / 2.0f,
//	2.0f * PI,
//	};
//
//	float my_vertices[] = {
//		// positions         // colors
//		//bottom points
//		 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
//		 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
//		 0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
//		 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
//
//		 //top vetrex
//		 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
//	};
//
//	unsigned int indices[] = {
//	0, 1, 4,   
//	1, 2, 4,
//	2, 3, 4,
//	3, 0, 4,
//	};
//
//	unsigned int VAO;
//	unsigned int VBO;
//	unsigned int shaderProgram;
//	init_shaders(my_vertices, sizeof(my_vertices), shaderProgram, VAO, VBO);
//	
//	unsigned int EBO;
//	glGenBuffers(1, &EBO);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	while (!glfwWindowShouldClose(window)) {
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		//Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
//		glUseProgram(shaderProgram);
//		glBindVertexArray(VAO);
//		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//		for (int i = 0; i < 4; i++)
//		{
//			get_coordinates_from_angle(my_vertices[0 +(i*6)], my_vertices[2 + (i*6)], angles[i]);
//			angles[i] -= 0.01f; // Increment the angle for the next frame
//		}
//					
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(my_vertices), my_vertices);
//	}
//	return 0;
//}