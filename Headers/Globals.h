#pragma once

//defines----------------------------------------------------------------------------------------
#define TEXTURE_SLOTS 48 //most modern gpu can supprot more than 48 but 
//openGl 3.3 garantees minumum 48 texture units and its usualy enough for most cases

#define VAO_MAX_ATTRIB_AMOUNT 16 //max attrib indexs per vaos

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3
//end of defines---------------------------------------------------------------------------------



//Includes----------------------------------------------------------------------------------------
#include <glad/glad.h> 
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
//end of includes---------------------------------------------------------------------------------



//X-Macros----------------------------------------------------------------------------------------
	//TEX_TYPE MACRO------------------------------------------------------------------------------
	#define TEX_TYPES \
		X(DIFFUSE , aiTextureType_DIFFUSE, aiTextureType_BASE_COLOR) \
		X(NORMAL , aiTextureType_NORMALS, aiTextureType_NONE) \
		X(SPECULAR , aiTextureType_SPECULAR, aiTextureType_NONE) \

	enum TextureType
	{
		#define X(name, assimp_name, second_assimp_name) name,
		TEX_TYPES
		#undef X

		Tex_type_amount//always leave in bottom. Used to measure how many texture types are there
	};

	static const std::string Tex_Types_Names[] = 
	{
		#define X(name, assimp_name, second_assimp_name) #name,
		TEX_TYPES
		#undef X
	};

	//end of TEX_TYPE MACRO-----------------------------------------------------------------------
//end of X-Macros---------------------------------------------------------------------------------



//Structs----------------------------------------------------------------------------------------
struct Texture {
	unsigned int id;
	TextureType type;
	std::string path;
};

struct Light {
	bool has_a_source;
	glm::vec3 light_pos;
	glm::vec3 light_target;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float cos_soft_cut_off_angle;
	float cos_hard_cut_off_angle;

	float constant;
	float linear;
	float quadratic;
};

//end of structs---------------------------------------------------------------------------------



//Namespaces--------------------------------------------------------------------------------------
namespace Texture_slots {
	std::vector<Texture> loaded_textures; //to avoid loading duplicated textures, inclues all textures bound or unbound.

	unsigned int bound_slots[TEXTURE_SLOTS] = { 0 };//active texture slots like GL_TEXTURE0,for index 0 means GL_TEXTURE0 and var is texture id

	unsigned int slot_age[TEXTURE_SLOTS] = { 0 };//to track usage age of slots for replacement if needed

	void new_texture_loaded(Texture texture)
	{
		if(loaded_textures.capacity()<100)
			loaded_textures.reserve(100);


		loaded_textures.push_back(texture);
	}

	Texture* get_loaded_texture(const std::string& path)
	{
		for (Texture& tex : loaded_textures)
		{
			if (tex.path == path)
			{
				return &tex;
			}
		}
		return nullptr;
		
	}

	void age_slots()
	{
		slot_age[0] += bound_slots[0] <= 0 ? 0 : 2; //becouse slot 0 is mostly used during other texture bindings we try to free it as much as possible
		for (int i = 1; i < TEXTURE_SLOTS; ++i) {
			if (bound_slots[i] != 0) {
				slot_age[i]++;
			}
		}
	}

	int get_oldest_slot()
	{
		int oldest_index = 0;
		unsigned int max_age = 0;
		for (int i = 0; i < TEXTURE_SLOTS; ++i) {
			if (slot_age[i] > max_age) {
				max_age = slot_age[i];
				oldest_index = i;
			}
		}
		return oldest_index;
	}

	int get_index_of_bound_slot(unsigned int texture_id)
	{
		for (int i = 0; i < TEXTURE_SLOTS; ++i) {
			if (bound_slots[i] == texture_id) {
				return i;
			}
		}
		return -1;
	}

	int get_last_empty_space()//becouse slot 0 is mostly used during other texture bindings we start searching from last slot
	{
		for (int i = TEXTURE_SLOTS - 1; i >= 0; --i) {
			if (bound_slots[i] <= 0) {
				return i;
			}
		}
		return -1;
	}

	void unbound_texture(int slot_index)
	{
		if (slot_index >= 0 && slot_index < TEXTURE_SLOTS)
		{
			glActiveTexture(GL_TEXTURE0 + slot_index);
			glBindTexture(GL_TEXTURE_2D, 0);
			bound_slots[slot_index] = 0;
			slot_age[slot_index] = 0;
		}
	}

	int bound_texture(unsigned int texture_id)
	{
		int slot_index = get_index_of_bound_slot(texture_id);
		if (slot_index != -1) {
			// Texture is already bound, reset its age
			slot_age[slot_index] = 0;
			return slot_index;
		}
		slot_index = get_last_empty_space();

		if (slot_index == -1)
		{
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
//end of namespaces-------------------------------------------------------------------------------