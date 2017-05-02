#pragma once
#ifndef __RESOURCE_CHUNK_H__
#define __RESOURCE_CHUNK_H__

class ResourceChunk {
public:
	ResourceChunk();
	virtual ~ResourceChunk();

	virtual bool Load() = 0;
	virtual void Unload() = 0;

	void SetNextChunk(ResourceChunk * chunk);

	ResourceChunk * next() const;

private:
	ResourceChunk * next_;
};

#endif