#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Headers\Shader.h"

enum TextureType {
	TEXTURE,
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT
};

namespace Textures {
	unsigned int bound_slots[16] = { -1 };
	unsigned int get_index_of_bound_slot(unsigned int texture_id) 
	{
		for (unsigned int i = 0; i < 16; ++i) {
			if (bound_slots[i] == texture_id) {
				return i;
			}
		}
		return -1;
	}
	unsigned int get_firs_empty_space()
	{
		for (unsigned int i = 0; i < 16; ++i) {
			if (bound_slots[i] <= 0) {
				return i;
			}
		}
		return -1;
	}
}

class game_object_basic_model
{
private:

	struct vertex_data
	{
		float position[3];
		float tex_coords[2];
		float normal[3];
	};

	struct Texture {
		unsigned int id;
		TextureType type;
	};

	class Mesh
	{
	private:
		std::vector<unsigned int> instance_VBOs;

	public:
		std::vector<vertex_data>  vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture>      textures;

		unsigned int VAO, VBO_Mesh, EBO;

		Mesh(std::vector<vertex_data> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
			:instance_VBOs(13, 0), vertices(vertices), indices(indices), textures(textures)
		{

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO_Mesh);
			glGenBuffers(1, &EBO);


			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Mesh);

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_data), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (void*)0);
			// vertextexture coords
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (void*)offsetof(vertex_data, tex_coords));
			// vertex  normals
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (void*)offsetof(vertex_data, normal));

			glBindVertexArray(0);
		}

		//add vec instance buffer for instanced rendering

		///use this function to add extra per-instance attributes like colors,
		///its size must be float vector of max attribute size 4, it uses 1 attrib index
		///you cant pass total of 16 attrib indexs
		///you should have enough vectors for amoun you wanna draw,
		/// if you have more no problem, if you have less than you get undefined behevior
		void add_vec_instance_buffer(std::vector<float>& extra_data, int attrib_index, int attrib_size, int loop_instance = 1)
		{
			if (attrib_size < 1 || attrib_size >4)
				return; // Invalid attribute size

			if (attrib_index >= 16 || attrib_index <= 2)
				return; // Invalid or mesh's attribute index

			if (instance_VBOs[attrib_index - 3] == 0)
			{
				glGenBuffers(1, &instance_VBOs[attrib_index - 3]);
			}

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, instance_VBOs[attrib_index - 3]);
			glBufferData(GL_ARRAY_BUFFER, extra_data.size() * sizeof(float), &extra_data[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, attrib_size, GL_FLOAT, GL_FALSE, attrib_size * sizeof(float), (void*)0);
			glVertexAttribDivisor(attrib_index, loop_instance); // Update this attribute per instance

			glBindVertexArray(0);
		}

		//add mat4 instance buffer for instanced rendering

		///use this function to add extra per-instance attributes like model matrices,
		///it uses 4 attrib index
		///you cant pass total of 16 attrib indexs so max start index is 12
		///you should have enough vectors for amoun you wanna draw,
		///if you have more no problem, if you have less than you get undefined behevior
		void add_mat4_instance_buffer(std::vector<glm::mat4>& extra_data, int attrib_start_index, int loop_instance = 1)
		{
			if (attrib_start_index >= 13 || attrib_start_index <= 3)
				return; // Invalid or mesh's attribute index

			if (instance_VBOs[attrib_start_index - 3] == 0)
			{
				glGenBuffers(1, &instance_VBOs[attrib_start_index - 3]);
				instance_VBOs[attrib_start_index - 2] = instance_VBOs[attrib_start_index - 3];
				instance_VBOs[attrib_start_index - 1] = instance_VBOs[attrib_start_index - 3];
				instance_VBOs[attrib_start_index] = instance_VBOs[attrib_start_index - 3];
			}

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, instance_VBOs[attrib_start_index - 3]);
			glBufferData(GL_ARRAY_BUFFER, extra_data.size() * sizeof(glm::mat4), &extra_data[0], GL_STATIC_DRAW);
			// A mat4 takes up 4 attribute slots
			for (int i = 0; i < 4; i++) {
				glEnableVertexAttribArray(attrib_start_index + i);
				glVertexAttribPointer(attrib_start_index + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));  // Stride between matrices
				glVertexAttribDivisor(attrib_start_index + i, loop_instance); // Update this attribute per instance
			}
			glBindVertexArray(0);

		};
	};

public:

	std::vector<Mesh> Meshs;

};

