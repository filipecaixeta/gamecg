#include "chunk.h"

Chunk::Chunk() {
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

void Chunk::playSound(int looping, int channel) {
	Mix_PlayChannel(channel, chunk, looping);
}

int Chunk::isPlaying(int channel) {
	return Mix_Playing(channel);
}

void Chunk::stopSound(int channel, bool isFadeOut)
{
	if(isFadeOut)
		Mix_FadeOutChannel(channel, 1000);
	else
		Mix_HaltChannel(channel);
}

void Chunk::changeVolume(int channel, int volume)
{
	SFX::changeVolume(channel, volume);
}