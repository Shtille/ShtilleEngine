#pragma once
#ifndef __ALBEDO_MATERIAL_BINDER_H__
#define __ALBEDO_MATERIAL_BINDER_H__

#include "graphics/include/renderer/shader.h"
#include "graphics/include/material.h"

class AlbedoMaterialBinder : public sht::graphics::MaterialBinderInterface {
public:
	AlbedoMaterialBinder();

	void Bind(sht::graphics::Material * material) override;

	void SetShader(sht::graphics::Shader * shader);

private:
	sht::graphics::Shader * shader_;
};

#endif