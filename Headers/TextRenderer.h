#pragma once
#include "Headers\Shader.h"
#include <glad/glad.h> 
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <unordered_map>

#include "Headers\Some_functions.h"
class TextRenderer
{
private:
	unsigned int VAO, VBO_pos,VBO_tex, EBO;
	Shader shader;
	unsigned int texture_atlas;
	int char_width, char_height;
	float normalized_char_width, normalized_char_height;
	int screen_width, screen_height;
	float add_advance_per_char;
	
	int nrChannels, width, height;

	float pos_coords[12] =
	{
		0.0f, 0.0f, 0.0f,	// Bottom-left
		1.0f, 0.0f, 0.0f,	// Bottom-right
		1.0f, 1.0f, 0.0f,	// Top-right
		0.0f, 1.0f, 0.0f	// Top-left
	};

	float tex_coords[8] = 
	{
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f
	};

	const int ebo[6] = 
	{
		0,1,2,
		0,2,3
	};

	std::unordered_map<uint32_t, std::pair<int, int>> char_pos;

	int get_ascii_code(const std::string& str)
	{
		size_t start = str.find(' '); // find first space
		if (start == std::string::npos) return -1; // no space found
		start++; // move past first space

		size_t fin = str.find(' ', start); // find next space after start
		if (fin == std::string::npos) return -1; // no second space found

		return std::stoi(str.substr(start, fin - start));
	}

	

public:
	glm::vec4 deleted_colors[8];
	float tolerances[8];
	glm::vec4 replace_colors[8];
	int num_color = 0;

	TextRenderer(
		const char* texture_path, const char* char_set_path,
		const int screen_width, const int screen_height,
		const int char_width, const int char_height, 
		const char* vertex_shader_path, const char* fragment_shader_path,
		const float add_advance_per_char = 0.0f,const int image_packing = 4)
		: shader(vertex_shader_path, fragment_shader_path), char_width(char_width), char_height(char_height)
		, screen_height(screen_height), screen_width(screen_width), add_advance_per_char(add_advance_per_char)
	{
		// Load texture atlas
		texture_atlas = load_image(texture_path, width, height, nrChannels, image_packing, false);

		normalized_char_width = static_cast<float>(char_width) / width;
		normalized_char_height = static_cast<float>(char_height) / height;

		pos_coords[3] = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		pos_coords[6] = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;

		pos_coords[7] = static_cast<float>(char_height) / screen_height;
		pos_coords[10] = static_cast<float>(char_height) / screen_height;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO_pos);
		glGenBuffers(1, &VBO_tex);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_atlas);

		// Upload positions
		glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pos_coords), pos_coords, GL_STATIC_DRAW);

		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo), ebo, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		// Upload texCoords
		glBindBuffer(GL_ARRAY_BUFFER, VBO_tex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);



		int char_per_row = width / char_width;
		int char_per_col = height / char_height;

		char_pos.reserve(char_per_row * char_per_col);


		std::ifstream file(char_set_path); // Open the file
		if (!file.is_open())
		{
			std::cerr << "Failed to open file\n";
			throw std::runtime_error("Could not open character set file");
		}

		int x = 0,y=0;
		std::string line;
		while (std::getline(file, line)) // Read line by line
		{
			int ascii_code = get_ascii_code(line);

			if (ascii_code == -1)
			{
				std::cerr << "error on line: " << x;
				throw std::runtime_error("Invalid line format");
			}
			char_pos[ascii_code] = { x * char_width, y*char_height};
			if (x >= char_per_row - 1)
			{
				x = 0;
				y++;
			}
			else
			x++;
		}

		file.close();
	}

	~TextRenderer()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO_pos);
		glDeleteBuffers(1, &VBO_tex);
		glDeleteBuffers(1, &EBO);
		glDeleteTextures(1, &texture_atlas);
	}

	int change_deleted_colors(int index_of_color, const glm::vec4& new_deleted_color, float tolerance = 0.5f, const glm::vec4& new_replace_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f))
	{
		if (index_of_color < 0 || index_of_color > 7)
			return -1;
		if (index_of_color >= num_color)
			index_of_color = num_color++;

		deleted_colors[index_of_color] = new_deleted_color;
		tolerances[index_of_color] = tolerance;
		replace_colors[index_of_color] = new_replace_color;

		return index_of_color;
	}

	void push_deleted_colors()
	{
		shader.use();
		shader.setVec4("delete_colors", deleted_colors,num_color);
		shader.setFloat("tolerances", tolerances, num_color);
		shader.setVec4("replace_colors", replace_colors, num_color);
		shader.setInt("num_colors", num_color);
	}

	void change_screen_size(int new_width, int new_height)
	{
		screen_width = new_width;
		screen_height = new_height;
		pos_coords[3] = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		pos_coords[6] = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		pos_coords[7] = static_cast<float>(char_height) / screen_height;
		pos_coords[10] = static_cast<float>(char_height) / screen_height;
		glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos_coords), pos_coords);
	}

	void change_add_advance_per_char(float new_value)
	{
		add_advance_per_char = new_value;
		pos_coords[3] = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		pos_coords[6] = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos_coords), pos_coords);
	}

	void render_text(const std::string& text, float starting_x, float starting_y, float scale_factor)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader.use();
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_atlas);
		shader.setInt("Texture_1", 0);
		
		const float add_advance_per_char_temp = add_advance_per_char <0 ? -1 * add_advance_per_char/2 : 0;

		for (uint32_t c : text)
		{
			if (char_pos.find(c) == char_pos.end())
				continue; // Skip characters not in the map

			float tex_x = static_cast<float>(char_pos[c].first) / width;
			float tex_y = static_cast<float>(char_pos[c].second) / height;

			tex_coords[0] = tex_x + add_advance_per_char_temp;								tex_coords[1] = tex_y + normalized_char_height; // Bottom-left
			tex_coords[2] = tex_x + normalized_char_width - add_advance_per_char_temp;		tex_coords[3] = tex_y + normalized_char_height; // Bottom-right
			tex_coords[4] = tex_x + normalized_char_width - add_advance_per_char_temp;		tex_coords[5] = tex_y; // Top-right
			tex_coords[6] = tex_x + add_advance_per_char_temp;								tex_coords[7] = tex_y; // Top-left

			glBindBuffer(GL_ARRAY_BUFFER, VBO_tex);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex_coords), tex_coords);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(starting_x, starting_y, 0.0f));
			model = glm::scale(model, glm::vec3(scale_factor, scale_factor, 1.0f));

			shader.setMatrix4fv("model", glm::value_ptr(model));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			starting_x += pos_coords[3]* scale_factor;
		}
		glEnable(GL_DEPTH_TEST);
	}
};

