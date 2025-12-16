#pragma once

#include "Shader.h"
#include "Globals.h"
#include "Some_functions.h"

#include <array>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static const aiTextureType Assimp_Tex_Types[] =
{
	#define X(name,assimp_name, second_assimp_name) assimp_name,
	TEX_TYPES
	#undef X
};

static const aiTextureType Assimp_Tex_Types_2[] =
{
	#define X(name,assimp_name, second_assimp_name) second_assimp_name,
	TEX_TYPES
	#undef X
};

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

		void bind_textures(Shader shader)
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

			glActiveTexture(GL_TEXTURE0);

			//for (int i = 0; i < Tex_type_amount; i++)
			//{
			//	printf(Tex_Types_Names[i].c_str());
			//	printf(" count: %d\n", counts[i]);
			//}
			

		}

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
			// vertex texture coords
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

			textures.clear();
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
			bind_textures(shader);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		void draw_instanced(Shader& shader, int amount)
		{
			bind_textures(shader);

			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}
	};

	struct Mesh_Childs
	{
		std::vector<std::shared_ptr<Mesh>> Meshes;
		std::vector<Mesh_Childs> Childs;
	};


	void process_node(aiNode* node, const aiScene* scene, Mesh_Childs& parent_mesh,const std::string& path)
	{
		//process meshes of node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			//process mesh
			Meshes.push_back(process_mesh(mesh, scene, path));
			printf("Processed mesh: %s\n", mesh->mName.C_Str());
		}
		//process childs
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			Mesh_Childs child_mesh;
			parent_mesh.Childs.push_back(child_mesh);
			process_node(node->mChildren[i], scene, child_mesh, path);
		}
	}

	std::shared_ptr<Mesh> process_mesh(aiMesh* mesh, const aiScene* scene,const std::string& path)
	{
		std::vector<vertex_data> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		//process vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertex_data vertex;
			//positions
			vertex.position[0] = mesh->mVertices[i].x;
			vertex.position[1] = mesh->mVertices[i].y;
			vertex.position[2] = mesh->mVertices[i].z;
			//normals
			if (mesh->HasNormals())
			{
				vertex.normal[0] = mesh->mNormals[i].x;
				vertex.normal[1] = mesh->mNormals[i].y;
				vertex.normal[2] = mesh->mNormals[i].z;
			}
			else
			{
				vertex.normal[0] = 0.0f;
				vertex.normal[1] = 0.0f;
				vertex.normal[2] = 0.0f;
			}
			//texture coords
			if (mesh->mTextureCoords[0]) //does the mesh contain texture coordinates?
			{
				vertex.tex_coords[0] = mesh->mTextureCoords[0][i].x;
				vertex.tex_coords[1] = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.tex_coords[0] = 0.0f;
				vertex.tex_coords[1] = 0.0f;
			}
			vertices.push_back(vertex);
		}
		//process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		//process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			bool loop = true;
			for (int i = 0; i < Tex_type_amount; loop = !loop)
			{
				aiTextureType assimp_type = loop ? Assimp_Tex_Types[i] : Assimp_Tex_Types_2[i];
				
				for(int j = 0; j < material->GetTextureCount(assimp_type); j++)
				{
					Texture* texture;
					aiString str;
					int unused_data1 = 0, unused_data2 = 0, unused_data3 = 0; // data returned by load_image but unused here

					//assimp_type - Texture type, j - Index of the texture to get, &str - output path
					material->GetTexture(assimp_type, j, &str);

					std::string directory = path.substr(0, path.find_last_of('\\'));
					str.Set((directory + '\\' + str.C_Str()).c_str());

					texture = Texture_slots::get_loaded_texture(str.C_Str());//check if texture was loaded before
					if(texture != nullptr)
					{
						textures.push_back(*texture);
					}
					else
					{
						texture = new Texture();

						texture->id = load_image(str.C_Str(),unused_data1,unused_data2,unused_data3);
						texture->type = static_cast<TextureType>(i);
						texture->path = str.C_Str();

						textures.push_back(*texture);
						Texture_slots::loaded_textures.push_back(*texture);
					}
				}
				
				if (!loop) i++;
			}

		}
		return std::make_shared<Mesh>(vertices,indices,textures);
	}

public:

	std::vector<std::shared_ptr<Mesh>> Meshes;//you cant use copy constructor or assignment operator because of Mesh class
	//so you need to manage meshes throut pointers becouse vectors copy elements when resized

	Mesh_Childs root;

	void draw(Shader shader)
	{
		for(std::shared_ptr<Mesh> pointer : Meshes)
		{
			pointer->draw(shader);
		}
	}

	void draw_instanced(Shader shader, int amount)
	{
		for (std::shared_ptr<Mesh> pointer : Meshes)
		{
			pointer->draw_instanced(shader, amount);
		}
	}


	///use this function to add extra per-instance attributes like colors,
	///its size must be float vector of max attribute size 4,
	///most of the time you have max of 16 attrib indexs per vao, 0-1-2 are used by mesh,
	///this function uses 1 attrib index,
	///you should have enough vectors for amoun you wanna draw,
	/// if you have more no problem, if you have less than you get undefined behevior
	void add_vec_instance_buffer(std::vector<float>& extra_data, int attrib_size, int attrib_index, int loop_instance = 1)
	{
		for (std::shared_ptr<Mesh> pointer : Meshes)
		{
			pointer->add_vec_instance_buffer(extra_data, attrib_size, attrib_index, loop_instance);
		}
	}


	///use this function to add extra per-instance attributes like model matrices,
	///most of the time you have max of 16 attrib indexs per vao, 0-1-2 are used by mesh,
	///it uses 4 attrib index,
	///you should have enough vectors for amoun you wanna draw,
	///if you have more no problem, if you have less than you get undefined behevior
	void add_mat4_instance_buffer(std::vector<glm::mat4>& extra_data, int attrib_start_index, int loop_instance = 1)
	{
		for (std::shared_ptr<Mesh> pointer : Meshes)
		{
			pointer->add_mat4_instance_buffer(extra_data, attrib_start_index, loop_instance);
		}
	}

	void import_model_from_file(std::string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		}
		else
		{
			process_node(scene->mRootNode, scene, root,path);
		}

	}
};

