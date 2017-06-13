#pragma once
#ifndef __MATERIAL_BINDER_H__
#define __MATERIAL_BINDER_H__

#include "graphics/include/renderer/shader.h"
#include "graphics/include/material.h"

class MaterialBinder : public sht::graphics::MaterialBinderInterface {
public:
	MaterialBinder();

	void Bind(sht::graphics::Material * material) override;

	void SetShader(sht::graphics::Shader * shader);

private:
	sht::graphics::Shader * shader_;
};

#endif