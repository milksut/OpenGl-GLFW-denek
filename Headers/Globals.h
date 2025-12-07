#pragma once

//defines----------------------------------------------------------------------------------------
#define TEXTURE_SLOTS 48 //most modern gpu can supprot more than 48 but 
//openGl 3.3 garantees minumum 48 texture units and its usualy enough for most cases

#define VAO_MAX_ATTRIB_AMOUNT 16 //max attrib indexs per vaos
//end of defines---------------------------------------------------------------------------------



//Includes----------------------------------------------------------------------------------------
#include <glad/glad.h> 
#include <glfw/glfw3.h>
#include <string>
//end of includes---------------------------------------------------------------------------------



//X-Macros----------------------------------------------------------------------------------------
	//TEX_TYPE MACRO------------------------------------------------------------------------------
	#define TEX_TYPES \
		X(TEXTURE) \
		X(DIFFUSE) \
		X(SPECULAR) \
		X(NORMAL) \
		X(HEIGHT)

	enum TextureType
	{
		#define X(name) name,
		TEX_TYPES
		#undef X

		Tex_type_amount//always leave in bottom. Used to measure how many texture types are there
	};

	static const std::string Tex_Types_Names[] = 
	{
		#define X(name) #name,
		TEX_TYPES
		#undef X
	};
	//end of TEX_TYPE MACRO-----------------------------------------------------------------------
//end of X-Macros---------------------------------------------------------------------------------



//Namespaces--------------------------------------------------------------------------------------
namespace Texture_slots {
	unsigned int bound_slots[TEXTURE_SLOTS] = { 0 };//active texture slots like GL_TEXTURE0,for index 0 means GL_TEXTURE0 and var is texture id

	unsigned int slot_age[TEXTURE_SLOTS] = { 0 };//to track usage age of slots for replacement if needed

	void age_slots()
	{
		for (int i = 0; i < TEXTURE_SLOTS; ++i) {
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

	int get_firs_empty_space()
	{
		for (int i = 0; i < TEXTURE_SLOTS; ++i) {
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
//end of namespaces-------------------------------------------------------------------------------



//Structs----------------------------------------------------------------------------------------
struct Texture {
	unsigned int id;
	TextureType type;
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