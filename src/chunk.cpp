#include "chunk.h"

Chunk::Chunk() {
	// TODO Auto-generated constructor stub
	chunk = NULL;
	channel = 2;
}

Chunk::Chunk(int channel) : SFX(channel) {
	// TODO Auto-generated constructor stub
	chunk = NULL;
	channel = 2;
}

Chunk::~Chunk() {
	// TODO Auto-generated destructor stub
	Mix_FreeChunk(chunk);
}

void Chunk::loadFile(string path) {
	chunk = Mix_LoadWAV(path.c_str());
}

void Chunk::playSound(int looping) {
	Mix_PlayChannel(channel, chunk, looping);
}

int Chunk::isPlaying() {
	return Mix_Playing(channel);
}

void Chunk::stopSound()
{
	Mix_HaltChannel(channel);
}

void Chunk::changeVolume(int volume)
{
	SFX::changeVolume(volume);
}