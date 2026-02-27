#include "Globals.h"
#include "Shader.h"
#include "Camera_test.h"
#include "Some_functions.h"
#include "TextRenderer.h"
#include "game_object_basic.h"


const double Target_fps = 144;
const double Target_frame_time = 1.0 / Target_fps;
const bool enable_vSync = false;

const unsigned int width = 800, height = 600;
const float aspect_ratio = (float)width / (float)height;

float mouse_lastX = width / 2, mouse_lastY = height / 2;

const float mouse_sensitivity = 0.3f;


camera_test camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

TextRenderer* printer;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//// make sure the viewport matches the new window dimensions; note that width and 
	//// height will be significantly larger than specified on retina displays.
	const float new_aspect_ratio = (float)width / (float)height;
	//if (new_aspect_ratio < aspect_ratio)
	//{
	//	//if the new aspect ratio is smaller than the original one, we need to adjust the viewport
	//	int new_height = (int)(width / aspect_ratio);
	//	glViewport(0, (height - new_height) / 2, width, new_height);
	//}
	//else
	//{
	//	//if the new aspect ratio is larger than the original one, we need to adjust the viewport
	//	int new_width = (int)(height * aspect_ratio);
	//	glViewport((width - new_width) / 2, 0, new_width, height);
	//}
	glViewport(0, 0, width, height);

	camera.update_projection(45.0f, new_aspect_ratio, 0.1f, 1000.0f);
	printer->change_screen_size(width, height);
}

bool pressable = true;
void processInput(GLFWwindow* window, float camera_speed, camera_test& camera)
{

	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && pressable)
	{
		static bool is_fullscreen = false;
		static int windowed_xpos = 100, windowed_ypos = 100, windowed_width = 800, windowed_height = 600;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (!is_fullscreen)
		{
			glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
			glfwGetWindowSize(window, &windowed_width, &windowed_height);
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			is_fullscreen = true;
		}
		else
		{
			glfwSetWindowMonitor(window, nullptr, windowed_xpos, windowed_ypos, windowed_width, windowed_height, 0);
			is_fullscreen = false;
		}
		pressable = false;
	}
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE)
	{
		pressable = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera_speed *= 2;

	bool changes[6] = { false, false, false, false, false, false };

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		changes[0] = true; // front change
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		changes[1] = true; // back change
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		changes[2] = true; // left change
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		changes[3] = true; // right change
	}


	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		changes[4] = true; // up change
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		changes[5] = true; // down change
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.camera_tilt(-camera_speed * 5);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.camera_tilt(camera_speed * 5);
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

	glfwSwapInterval(enable_vSync);

	camera.update_projection(45.0f, aspect_ratio, 0.1f, 1000.0f);

	printer = new TextRenderer("Textures/Font_texture_Atlas/DejaVu Sans Mono_512X256_16x32.png",
		"Textures/Font_texture_Atlas/DejaVu Sans Mono_512X256_16x32.txt",
		width, height, 16, 32,
		"Shaders/Vertex_shaders/Text_render_vertex.vert",
		"Shaders/Fragment_shaders/Text_render_fragment.frag",
		0.005f);

	printer->change_deleted_colors(0, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.5f, glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
	printer->change_deleted_colors(1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	printer->push_deleted_colors();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	Shader shader("Shaders/Vertex_shaders/Loaded_model_vertex.vert",
		"Shaders/Fragment_shaders/Loaded_model_fragment.frag");

	game_object_basic_model backpack;
	//backpack.import_model_from_file("C:\\Users\\altay\\Desktop\\pull_from_this_easy\\Backpack.obj");

	backpack.import_model_from_file("/home/altay2510tr/Desktop/opengl-asset/Tree1.obj");
	
	//-*-*-*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*
	int grid_amount = 60;
	//-*-*-*-*-*-*-*-**-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*

	std::shared_ptr<class_region> grid_region = backpack.reserve_class_region(grid_amount * grid_amount);

	backpack.add_instance_buffer(16, 3); //attrib size-mat4-16floats, attrib index, for model

	backpack.add_instance_buffer(9, 7); //attrib size-mat3-12floats, attrib index, for transpose_inverse_viewXmodel


	std::vector<glm::mat4> model_matrices_grid;
	model_matrices_grid.reserve(grid_amount * grid_amount);
	for (int i = 0; i < grid_amount; i++)
	{
		for (int j = 0; j < grid_amount; j++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(i * 5.0f, 5.0f, j * 5.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model_matrices_grid.push_back(model);
		}

	}
		
	backpack.load_instance_buffer((float*)model_matrices_grid.data(), model_matrices_grid.size(), 3, grid_region);
	checkGLError("After loading models");

	std::vector<glm::mat3> transpose_inverse_model_matrices_grid;
	transpose_inverse_model_matrices_grid.reserve(grid_amount * grid_amount);
	for (int i = 0; i < grid_amount; i++)
	{
		for (int j = 0; j < grid_amount; j++)
		{
			glm::mat4 model = model_matrices_grid[i * grid_amount + j];
			glm::mat3 transpose_inverse_model = glm::transpose(glm::inverse(glm::mat3(model)));
			transpose_inverse_model_matrices_grid.push_back(transpose_inverse_model);
		}
	}

	backpack.load_instance_buffer((float*)transpose_inverse_model_matrices_grid.data(), transpose_inverse_model_matrices_grid.size(), 7, grid_region);
	checkGLError("After loading transpose_inverse");

	Light sun = {false, glm::vec3(0.0), glm::vec3(0.0,-1.0,0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(5.0,5.0,5.0), glm::vec3(0.5f,0.5f,0.5f),0,0,0,0,0};
	
	shader.use();
	shader.setInt("num_of_lights", 1);
	checkGLError("After changing light amount");

	shader.setBool("lights[0].has_a_source", sun.has_a_source);
	shader.setVec3("lights[0].light_pos", sun.light_pos);
	shader.setVec3("lights[0].light_target", sun.light_target);

	shader.setVec3("lights[0].ambient", sun.ambient);
	shader.setVec3("lights[0].diffuse", sun.diffuse);
	shader.setVec3("lights[0].specular", sun.specular);
	
	shader.setFloat("lights[0].cos_soft_cut_off_angle", sun.cos_soft_cut_off_angle);
	shader.setFloat("lights[0].cos_hard_cut_off_angle", sun.cos_hard_cut_off_angle);

	shader.setFloat("lights[0].constant", sun.constant);
	shader.setFloat("lights[0].linear", sun.linear);
	shader.setFloat("lights[0].quadratic", sun.quadratic);

	checkGLError("After loading light");


	glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	int x = 0, y = 0, z = 0;
	double time_of_last_frame = 1;
	std::string fps_text = "";
	glfwSetTime(0.0);

	while (!glfwWindowShouldClose(window))
	{

		processInput(window, 0.1 * ((glfwGetTime() - time_of_last_frame) / Target_frame_time), camera);
		time_of_last_frame = glfwGetTime();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setVec3("viewPos", camera.camera_position);
		shader.setMatrix4fv("view", glm::value_ptr(camera.get_view_matrix()));
		shader.setMatrix4fv("projection", glm::value_ptr(camera.projection));



		backpack.draw(shader, grid_region, grid_amount * grid_amount);
		checkGLError("After drawing grid backpack");

		x++;
		if (glfwGetTime() - z >= 1.0f)
		{
			y = x;
			x = 0;
			z = glfwGetTime();
			fps_text = "FPS: " + std::to_string(y);
			printf("Draw calls per second : %d\n",draw_call_count);
			draw_call_count = 0;
		}
		printer->render_text(fps_text, -1, 0.9, 2.0f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete printer;
}
