#pragma once
#include "Headers\Shader.h"
#include "Headers/Globals.h"

#include <glm/gtc/matrix_transform.hpp>


#include <fstream>
#include <unordered_map>

#include "Headers\Some_functions.h"
class TextRenderer
{
private:
	unsigned int VAO, VBO, EBO;
	Shader shader;
	unsigned int texture_atlas;
	int char_width, char_height;
	float normalized_char_width, normalized_char_height;
	int screen_width, screen_height;
	float screen_char_width, screen_char_height, add_advance_per_char;

	
	int nrChannels, width, height;

	float tex_coords[8] = { 0 };

	std::vector<float> vertices;
	std::unordered_map<uint32_t, std::pair<float, float>> char_pos;
	std::vector<unsigned int> indices;
	int indices_count = 10;

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

		screen_char_width = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		screen_char_height = static_cast<float>(char_height) / screen_height;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);


		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		indices.reserve(indices_count * 6);// 6 indices per char
				for (size_t i = 0; i < indices_count; i++)
				{
					unsigned int base = i * 4;
					indices.push_back(base);
					indices.push_back(base + 1);
					indices.push_back(base + 2);
					indices.push_back(base + 2);
					indices.push_back(base + 3);
					indices.push_back(base);
				}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int), indices.data(), GL_STREAM_DRAW);

		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_atlas);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
			char_pos[ascii_code] = { (float)(x*char_width) / width, (float)(y*char_height) / height };
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
		glDeleteBuffers(1, &VBO);
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
		screen_char_width = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		screen_char_height = static_cast<float>(char_height) / screen_height;
	}

	void change_add_advance_per_char(float new_value)
	{
		add_advance_per_char = new_value;
		screen_char_width = (static_cast<float>(char_width) / screen_width) + add_advance_per_char;
		screen_char_height = static_cast<float>(char_height) / screen_height;
	}

	void render_text(const std::string& text, float starting_x, float starting_y, float scale_factor)
	{
		if (text.empty())
			return;
		int text_size = text.size();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		shader.use();
		
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_atlas);
		shader.setInt("Texture_1", 0);
		
		// Build vertex data for all characters at once
		
		vertices.clear();
		vertices.reserve(text_size * 20); // 5 floats per vertex * 4 vertices per char
		bool flag_indices_size_change = false;
		if (indices_count < text_size)
		{
			// Regenerate indices if needed
			indices.reserve(text_size * 6);
			for (size_t i = indices_count; i < text_size; i++)
			{
				unsigned int base = i * 4;
				indices.push_back(base);
				indices.push_back(base + 1);
				indices.push_back(base + 2);
				indices.push_back(base + 2);
				indices.push_back(base + 3);
				indices.push_back(base);
			}
			indices_count = static_cast<int>(text_size);
			flag_indices_size_change = true;
		}


		const float add_advance_per_char_temp = add_advance_per_char <0 ? -1 * add_advance_per_char/2 : 0;

		for (uint32_t c : text)
		{
			auto map_pos = char_pos.find(c);
			if (map_pos == char_pos.end())
				continue; // Skip characters not in the map

			float tex_x = static_cast<float>(map_pos->second.first);
			float tex_y = static_cast<float>(map_pos->second.second);


			tex_coords[0] = tex_x + add_advance_per_char_temp;								tex_coords[1] = tex_y + normalized_char_height; // Bottom-left
			tex_coords[2] = tex_x + normalized_char_width - add_advance_per_char_temp;		tex_coords[3] = tex_y + normalized_char_height; // Bottom-right
			tex_coords[4] = tex_x + normalized_char_width - add_advance_per_char_temp;		tex_coords[5] = tex_y; // Top-right
			tex_coords[6] = tex_x + add_advance_per_char_temp;								tex_coords[7] = tex_y; // Top-left


			
			float ending_x = starting_x + screen_char_width * scale_factor;
			float ending_y = starting_y + screen_char_height * scale_factor;
			vertices.insert(vertices.end(),
			{
				//	x			y		z		u			   v
				starting_x, starting_y, 00, tex_coords[0], tex_coords[1],
				ending_x,   starting_y, 00, tex_coords[2], tex_coords[3],
				ending_x,   ending_y,   00, tex_coords[4], tex_coords[5],
				starting_x, ending_y,   00, tex_coords[6], tex_coords[7]
			});
			
			starting_x += screen_char_width * scale_factor;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), nullptr, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,0, vertices.size() * sizeof(float), vertices.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		if(flag_indices_size_change)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STREAM_DRAW);

		glDrawElements(GL_TRIANGLES, text_size*6, GL_UNSIGNED_INT, 0);

		glEnable(GL_DEPTH_TEST);
	}
};

