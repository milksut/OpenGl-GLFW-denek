#include <glad/glad.h> 
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Headers\stb_image.h"

#include "Headers\Shader.h"
#include "Headers\Camera_test.h"
#include "Headers\Some_functions.h"
#include "Headers\TextRenderer.h"

const unsigned int width = 800, height = 600;
const float aspect_ratio = (float)width / (float)height;

float mouse_lastX = width/2, mouse_lastY = height/2;

const float mouse_sensitivity = 0.1f;

glm::vec3 camera_rotation(0.0f, 0.0f, 0.0f);
glm::vec3 camera_direction = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_ang = glm::vec3(0.0f, 0.0f, 0.0f);


camera_test camera(camera_direction, camera_ang);



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.

	const float new_aspect_ratio = (float)width / (float)height;
	if (new_aspect_ratio < aspect_ratio)
	{
		//if the new aspect ratio is smaller than the original one, we need to adjust the viewport
		int new_height = (int)(width / aspect_ratio);
		glViewport(0, (height - new_height) / 2, width, new_height);
	}
	else
	{
		//if the new aspect ratio is larger than the original one, we need to adjust the viewport
		int new_width = (int)(height * aspect_ratio);
		glViewport((width - new_width) / 2, 0, new_width, height);
	}
}

void processInput(GLFWwindow* window, float camera_speed, glm::vec3 &camera_direction, camera_test &camera)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera_speed *= 2;

	bool changes[6] = { false, false, false, false, false, false };

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera_direction.z += camera_speed * cos(glm::radians(camera_rotation.x));
		camera_direction.x -= camera_speed * sin(glm::radians(camera_rotation.x));
		changes[0] = true; // front change
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera_direction.z -= camera_speed * cos(glm::radians(camera_rotation.x));
		camera_direction.x += camera_speed * sin(glm::radians(camera_rotation.x));
		changes[1] = true; // back change
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera_direction.x += camera_speed * cos(glm::radians(camera_rotation.x));
		camera_direction.z += camera_speed * sin(glm::radians(camera_rotation.x));
		changes[2] = true; // left change
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera_direction.x -= camera_speed * cos(glm::radians(camera_rotation.x));
		camera_direction.z -= camera_speed * sin(glm::radians(camera_rotation.x));
		changes[3] = true; // right change
	}


	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera_direction.y -= camera_speed;
		changes[4] = true; // up change
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera_direction.y += camera_speed;
		changes[5] = true; // down change
	}

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.camera_tilt(-camera_speed*5);
	}
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.camera_tilt(camera_speed*5);
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	camera.camera_move(
		1, camera_speed,
		changes[0], changes[1],
		changes[2], changes[3],
		changes[4], changes[5]);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
	const float xoffset = (x_pos - mouse_lastX) * mouse_sensitivity;
	const float yoffset = (y_pos - mouse_lastY) * mouse_sensitivity;

	mouse_lastX = x_pos;
	mouse_lastY = y_pos;

	camera.process_mouse_movement(xoffset, yoffset, mouse_sensitivity);

}




int main()
{
	GLFWwindow* window = init_window(width, height, "Shader Tester");
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	if (!window) {
		return -1;
	}
	
	TextRenderer printer("Textures\\Font_texture_Atlas\\DejaVu Sans Mono_512X256_16x32.png",
	"Textures\\Font_texture_Atlas\\DejaVu Sans Mono_512X256_16x32.txt", width, height, 16, 32,
	"Shaders\\Vertex_shaders\\Text_render_vertex.vert", "Shaders\\Fragment_shaders\\Text_render_fragment.frag");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	Shader shader("Shaders\\Vertex_shaders\\MVP_texture_vertex.vert",
		"Shaders\\Fragment_shaders\\Basic_texture_fragment.frag");

	Shader shader_black("Shaders\\Vertex_shaders\\MVP_texture_vertex.vert",
		"Shaders\\Fragment_shaders\\Black_fragment.frag");

	int width, height, nrChannels;
	unsigned int texture = load_image("Textures\\random\\Tile_17-512x512.png",
		width, height, nrChannels);

	unsigned int sky_texture = load_image("Textures\\random\\skybox_example.png",
		width, height, nrChannels);

	float skybox[] =
	{
		// ===== FRONT (z = +1) =====
		-1.0f, -1.0f, +1.0f,   0.50f, 0.333f,
		+1.0f, -1.0f, +1.0f,   0.75f, 0.333f,
		+1.0f, +1.0f, +1.0f,   0.75f, 0.666f,

		+1.0f, +1.0f, +1.0f,   0.75f, 0.666f,
		-1.0f, +1.0f, +1.0f,   0.50f, 0.666f,
		-1.0f, -1.0f, +1.0f,   0.50f, 0.333f,

		// ===== BACK (z = -1) =====
		+1.0f, -1.0f, -1.0f,   0.00f, 0.333f,
		-1.0f, -1.0f, -1.0f,   0.25f, 0.333f,
		-1.0f, +1.0f, -1.0f,   0.25f, 0.666f,

		-1.0f, +1.0f, -1.0f,   0.25f, 0.666f,
		+1.0f, +1.0f, -1.0f,   0.00f, 0.666f,
		+1.0f, -1.0f, -1.0f,   0.00f, 0.333f,

		// ===== LEFT (x = -1) =====
		-1.0f, -1.0f, -1.0f,   0.25f, 0.333f,
		-1.0f, -1.0f, +1.0f,   0.50f, 0.333f,
		-1.0f, +1.0f, +1.0f,   0.50f, 0.666f,

		-1.0f, +1.0f, +1.0f,   0.50f, 0.666f,
		-1.0f, +1.0f, -1.0f,   0.25f, 0.666f,
		-1.0f, -1.0f, -1.0f,   0.25f, 0.333f,

		// ===== RIGHT (x = +1) =====
		+1.0f, -1.0f, +1.0f,   0.75f, 0.333f,
		+1.0f, -1.0f, -1.0f,   1.00f, 0.333f,
		+1.0f, +1.0f, -1.0f,   1.00f, 0.666f,

		+1.0f, +1.0f, -1.0f,   1.00f, 0.666f,
		+1.0f, +1.0f, +1.0f,   0.75f, 0.666f,
		+1.0f, -1.0f, +1.0f,   0.75f, 0.333f,

		// ===== UP (y = +1) =====
		-1.0f, +1.0f, +1.0f,   0.25f, 0.666f,
		+1.0f, +1.0f, +1.0f,   0.50f, 0.666f,
		+1.0f, +1.0f, -1.0f,   0.50f, 1.000f,

		+1.0f, +1.0f, -1.0f,   0.50f, 1.000f,
		-1.0f, +1.0f, -1.0f,   0.25f, 1.000f,
		-1.0f, +1.0f, +1.0f,   0.25f, 0.666f,

		// ===== DOWN (y = -1) =====
		-1.0f, -1.0f, -1.0f,   0.25f, 0.000f,
		+1.0f, -1.0f, -1.0f,   0.50f, 0.000f,
		+1.0f, -1.0f, +1.0f,   0.50f, 0.333f,

		+1.0f, -1.0f, +1.0f,   0.50f, 0.333f,
		-1.0f, -1.0f, +1.0f,   0.25f, 0.333f,
		-1.0f, -1.0f, -1.0f,   0.25f, 0.000f,
	};

	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.setInt("Texture_1", 0);


	float cube_vertices[] = {
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

	unsigned int VBO_sky_box, VAO_sky_box;
	glGenVertexArrays(1, &VAO_sky_box);		glBindVertexArray(VAO_sky_box);
	glGenBuffers(1, &VBO_sky_box);			glBindBuffer(GL_ARRAY_BUFFER, VBO_sky_box);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), skybox, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);// Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO

	

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);		glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);			glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);// Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO

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



	//The model matrix consists of translations, scaling and/or rotations
	//we'd like to apply to transform all object's vertices to the global world space
	/*glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/
	

	//glm::mat4 view = glm::mat4(1.0f);
	//// note that we're translating the scene in the reverse direction of where we want to move
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 projection;
	//radians(45.0f) - FOV / aspect_ratio - aspect ratio
	//0.1f - distace with near plane(wiew start plane) / 100.0f - distance with far plane
	projection = glm::perspective(glm::radians(85.0f), aspect_ratio, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	int x= 0,y =0,z=0;
	glfwSetTime(0.0);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, 0.1, camera_direction,camera);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));

		/*glm::mat4 view = glm::mat4(1.0f);
		view = glm::rotate(view, glm::radians(camera_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(camera_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, camera_direction);*/

		shader.use();
		shader.setMatrix4fv("model", glm::value_ptr(model));
		shader.setMatrix4fv("view", glm::value_ptr(camera.get_view_matrix()));
		shader.setMatrix4fv("projection", glm::value_ptr(projection));
		

		glBindVertexArray(VAO_sky_box);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sky_texture);
		shader.setInt("Texture_1", 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(VAO);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		shader.setInt("Texture_1", 0);

		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			shader.setMatrix4fv("model", glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		x++;
		if(glfwGetTime() -z > 1.0f)
		{
			y = x;
			x = 0;
			z = glfwGetTime();
		}
		printer.render_text("FPS: " + std::to_string(y), -1, 0.9);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}