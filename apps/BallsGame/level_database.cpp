#include "level_database.h"

LevelDatabase::LevelDatabase()
{
	// TODO: add loading from file here
}
LevelDatabase::~LevelDatabase()
{
	// TODO: add dump to file here
}
const std::size_t LevelDatabase::Size() const
{
	return map_.size();
}
const std::string * LevelDatabase::Get(int number) const
{
	const auto it = map_.find(number);
	if (it != map_.end())
		return &it->second;
	else
		return nullptr;
}
void LevelDatabase::Add(int number, const std::string& commands)
{
	map_[number] = commands;
}
void LevelDatabase::Remove(int number)
{
	auto it = map_.find(number);
	map_.erase(it);
}