#pragma once
#ifndef __SIMPLE_MATERIAL_H__
#define __SIMPLE_MATERIAL_H__

#include "material.h"

#include "math/vector.h"

class ShaderDatabase;
class TextureDatabase;

class SimpleMaterial : public Material {
public:
	SimpleMaterial(sht::graphics::Renderer * renderer, ShaderDatabase * shader_db, TextureDatabase * texture_db,
		const sht::math::Vector3 * light_pos_eye, const std::string& texture_name);
	~SimpleMaterial();

	void Bind() override;

	void GroupBind() override;

private:
	ShaderDatabase * shader_db_;
	TextureDatabase * texture_db_;
	const sht::math::Vector3 * light_pos_eye_;
};

#endif