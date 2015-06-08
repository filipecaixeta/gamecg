#ifndef CHUNK_H_
#define CHUNK_H_

#include "SFX.h"

class Chunk: public SFX {
private:
	Mix_Chunk* chunk;
public:
	Chunk();
	virtual ~Chunk();
	void loadFile(string);
	void playSound(int looping = ONCE, int channel = -1);
	void stopSound(int, bool isFadeOut = false);
	int isPlaying(int);
	void changeVolume(int, int);
};

#endif /* CHUNK_H_ */