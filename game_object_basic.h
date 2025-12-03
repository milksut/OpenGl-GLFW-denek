#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Headers\Shader.h"

enum TextureType {
	TEXTURE = 0,
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT
};

namespace Textures {
	unsigned int bound_slots[48] = { -1 };//active texture slots like GL_TEXTURE0,for index 0 means GL_TEXTURE0 and var is texture id
	//most moders gpu can supprot more than 48 but 
	//openGl 3.3 garantees minumum 48 texture units and its usualy enough for most cases
	unsigned int slot_age[48] = { 0 };//to track usage age of slots for replacement if needed

	void age_slots()
	{
		for (unsigned int i = 0; i < 48; ++i) {
			if (bound_slots[i] != -1) {
				slot_age[i]++;
			}
		}
	}

	unsigned int get_oldest_slot()
	{
		unsigned int oldest_index = 0;
		unsigned int max_age = 0;
		for (unsigned int i = 0; i < 48; ++i) {
			if (slot_age[i] > max_age) {
				max_age = slot_age[i];
				oldest_index = i;
			}
		}
		return oldest_index;
	}

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

	void unbound_texture(unsigned int slot_index)
	{
		if (slot_index >= 0 && slot_index <48)
		{
			glActiveTexture(GL_TEXTURE0 + slot_index);
			glBindTexture(GL_TEXTURE_2D, 0);
			bound_slots[slot_index] = -1;
			slot_age[slot_index] = 0;
		}
	}

	unsigned int bound_texture(unsigned int texture_id)
	{
		unsigned int slot_index = get_index_of_bound_slot(texture_id);
		if (slot_index != -1) {
			// Texture is already bound, reset its age
			slot_age[slot_index] = 0;
			return slot_index;
		}
		slot_index = get_firs_empty_space();

		if (slot_index == -1) {
			// No available texture slots, replace the oldest one
			slot_index = get_oldest_slot();
			unbound_texture(slot_index);
		}

		glActiveTexture(GL_TEXTURE0 + slot_index);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		bound_slots[slot_index] = texture_id;
		slot_age[slot_index] = 0; // Reset age since it's just been used
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
		
		void draw(Shader& shader)
		{
			std::vector<unsigned int> texture_textures;
			std::vector<unsigned int> texture_diffuses;
			std::vector<unsigned int> texture_speculars;
			std::vector<unsigned int> texture_normals;
			std::vector<unsigned int> texture_heights;
			int counts[5] = {0,0,0,0,0};

			for (unsigned int i = 0; i<48 && i < textures.size(); i++)
			{
				unsigned int slot_index = Textures::bound_texture(textures[i].id);

				switch (textures[i].type)
				{
					case TEXTURE:
						texture_textures.push_back(slot_index);
						counts[TEXTURE]++;
						break;

					case DIFFUSE:
						texture_diffuses.push_back(slot_index);
						counts[DIFFUSE]++;
						break;

					case SPECULAR:
						texture_speculars.push_back(slot_index);
						counts[SPECULAR]++;
						break;

					case NORMAL:
						texture_normals.push_back(slot_index);
						counts[NORMAL]++;
						break;

					case HEIGHT:
						texture_heights.push_back(slot_index);
						counts[HEIGHT]++;
						break;

				default:
					break;
				}

				//TODO: send data to shader
			
			}
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
	};

public:

	std::vector<Mesh> Meshs;

};

