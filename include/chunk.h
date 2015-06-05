#ifndef CHUNK_H_
#define CHUNK_H_

#include "SFX.h"

class Chunk: public SFX {
private:
	Mix_Chunk* chunk;
public:
	Chunk();
	Chunk(int);
	virtual ~Chunk();
	void loadFile(string);
	void playSound();
	void changeVolume(int);
};

#endif /* CHUNK_H_ */