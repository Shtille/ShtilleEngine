#pragma once
#ifndef __LEVEL_DATABASE_H__
#define __LEVEL_DATABASE_H__

#include <string>
#include <map>

//! Stores map level name (or number) to commands list (string)
// also should load levels from file on construction
// and write down to file on destruction
class LevelDatabase {
	typedef std::map<int, std::string> Map;
public:
	LevelDatabase();
	~LevelDatabase();

	const std::size_t Size() const;
	const std::string * Get(int number) const;

	void Add(int number, const std::string& commands);
	void Remove(int number);

private:
	Map map_;
};

#endif