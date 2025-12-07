#pragma once

#include "Headers/Globals.h"
#include "Headers/Shader.h"

#include <array>
#include <vector>
#include <string>
#include <glm/mat4x4.hpp>


class game_object_basic_model
{
private:

	struct vertex_data
	{
		float position[3];
		float tex_coords[2];
		float normal[3];
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

		Mesh(std::vector<vertex_data> &vertices, std::vector<unsigned int> &indices, std::vector<Texture>& textures)
			:instance_VBOs(VAO_MAX_ATTRIB_AMOUNT-3, 0), vertices(vertices), indices(indices), textures(textures)
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

		~Mesh()
		{
			glDeleteBuffers(1, &VBO_Mesh);
			glDeleteBuffers(1, &EBO);

			for (auto id : instance_VBOs)
				if (id) glDeleteBuffers(1, &id);
			
			glDeleteVertexArrays(1, &VAO);
		}

		Mesh(const Mesh&) = delete;// Delete copy constructor

		Mesh& operator=(const Mesh&) = delete;// Delete copy assignment operator

		//add vec instance buffer for instanced rendering

		///use this function to add extra per-instance attributes like colors,
		///its size must be float vector of max attribute size 4,
		///most of the time you have max of 16 attrib indexs per vao, 0-1-2 are used by mesh,
		///this function uses 1 attrib index,
		///you should have enough vectors for amoun you wanna draw,
		/// if you have more no problem, if you have less than you get undefined behevior
		void add_vec_instance_buffer(std::vector<float>& extra_data, int attrib_size, int attrib_index, int loop_instance = 1)
		{
			if (attrib_size < 1 || attrib_size >4)
				return; // Invalid attribute size

			if (attrib_index >= VAO_MAX_ATTRIB_AMOUNT || attrib_index <= 2)
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
		///most of the time you have max of 16 attrib indexs per vao, 0-1-2 are used by mesh,
		///it uses 4 attrib index,
		///you should have enough vectors for amoun you wanna draw,
		///if you have more no problem, if you have less than you get undefined behevior
		void add_mat4_instance_buffer(std::vector<glm::mat4>& extra_data, int attrib_start_index, int loop_instance = 1)
		{
			if (attrib_start_index >= VAO_MAX_ATTRIB_AMOUNT-4 || attrib_start_index <= 2)
				return; // Invalid or mesh's attribute index

			if (instance_VBOs[attrib_start_index-3] == 0)
			{
				glGenBuffers(1, &instance_VBOs[attrib_start_index-3]);
				instance_VBOs[attrib_start_index - 2] = instance_VBOs[attrib_start_index-3];
				instance_VBOs[attrib_start_index - 1] = instance_VBOs[attrib_start_index-3];
				instance_VBOs[attrib_start_index] = instance_VBOs[attrib_start_index-3];
			}

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, instance_VBOs[attrib_start_index]);
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
			//these are sended as uniforms to shader as sampler2d arrays like TEXTURE[], DIFFUSE[] etc. What name is defined in globals.h
			//sended data amount is sended as int array named TEX_COUNTS[], they are in order of enum TextureType
			std::array<std::vector<int>,Tex_type_amount> texture_locations;
			for(std::vector<int> &var : texture_locations)
			{
				var.reserve(TEXTURE_SLOTS);
			}
			int counts[Tex_type_amount] = { 0 };

			for (int i = 0; i < TEXTURE_SLOTS && i < textures.size(); i++)
			{
				int slot_index = Texture_slots::bound_texture(textures[i].id);
				counts[textures[i].type]++;

				texture_locations[textures[i].type].push_back(slot_index);
			}
			
			shader.setInt("TEX_COUNTS", counts, Tex_type_amount);
			for(int i = 0; i<Tex_type_amount;i++)
			{
				shader.setInt(Tex_Types_Names[i], texture_locations[i].data(), counts[i]);
			}

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		//TODO: add draw instanced function
	};

public:

	std::vector<std::shared_ptr<Mesh>> Meshes;//you cant use copy constructor or assignment operator because of Mesh class
	//so you need to manage meshes throut pointers becouse vectors copy elements when resized

};

