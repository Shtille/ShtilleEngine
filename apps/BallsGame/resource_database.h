#pragma once
#ifndef __RESOURCE_DATABASE_H__
#define __RESOURCE_DATABASE_H__

#include <string>
#include <map>
#include <cassert>

template <class T>
class ResourceDatabase {
public:
	ResourceDatabase() = default;
	virtual ~ResourceDatabase() = default;

	T * Get(const std::string& name);
	void Release(const std::string& name);

	virtual void Load(const std::string& name) = 0;
	virtual void Unload(const std::string& name) = 0;

	virtual void LoadAll() = 0;
	virtual void UnloadAll() = 0;

protected:
	typedef std::pair<int, T*> Pair;
	typedef std::map<std::string, Pair> Container;

	Container container_;
};

template <class T>
T * ResourceDatabase<T>::Get(const std::string& name)
{
	typename Container::iterator it = container_.find(name);
	if (it != container_.end())
	{
		// Object exists in database, thus we just increase the counter
		auto& pair = it->second;
		++pair.first;
		return pair.second;
	}
	else
	{
		// Object doesn't exist in database, thus we load it
		Load(name);
		it = container_.find(name);
		assert(it != container_.end());
		auto& pair = it->second;
		++pair.first;
		return pair.second;
	}
}

template <class T>
void ResourceDatabase<T>::Release(const std::string& name)
{
	typename Container::iterator it = container_.find(name);
	assert(it != container_.end());
	auto& pair = it->second;
	--pair.first;
	if (pair.first == 0) // counter is zero
	{
		Unload(name);
	}
}

#endif