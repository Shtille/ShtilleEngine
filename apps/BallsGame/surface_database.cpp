#include "surface_database.h"

SurfaceDatabase::SurfaceDatabase()
{
	FillTable();
}
SurfaceDatabase::~SurfaceDatabase()
{
}
Surface * SurfaceDatabase::Get(const std::string& name)
{
	auto it = container_.find(name);
	if (it != container_.end())
		return &it->second;
	else
		return nullptr;
}
void SurfaceDatabase::FillTable()
{
	container_.insert(std::make_pair("metal", Surface(0.5f, 0.1f, 0.1f, 0.8f)));
}