#pragma once
#ifndef __SURFACE_DATABASE_H__
#define __SURFACE_DATABASE_H__

#include "surface.h"

#include <string>
#include <map>

class SurfaceDatabase {
	typedef std::map<std::string, Surface> Container;
public:
	SurfaceDatabase();
	~SurfaceDatabase();

	Surface * Get(const std::string& name);

private:
	void FillTable();

	Container container_;
};

#endif