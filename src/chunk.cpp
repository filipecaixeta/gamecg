#include "chunk.h"

Chunk::Chunk() {
	// TODO Auto-generated constructor stub
	chunk = NULL;
}

Chunk::Chunk(int channel) : SFX(channel) {
	// TODO Auto-generated constructor stub
	chunk = NULL;
}

Chunk::~Chunk() {
	// TODO Auto-generated destructor stub
	Mix_FreeChunk(chunk);
}

void Chunk::loadFile(string path) {
	chunk = Mix_LoadWAV(path.c_str());
}

void Chunk::playSound() {
	Mix_PlayChannel(-1, chunk, 0);
}

void Chunk::changeVolume(int volume)
{
	SFX::changeVolume(volume);
}