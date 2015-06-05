#ifndef SFX_H_
#define SFX_H_
#include "SDL.h"
#include "SDL_mixer.h"
#include <iostream>

using namespace std;

class SFX {
private:
	int type;
	int channel;
public:
	SFX();
	SFX(int);
	void changeVolume(int);
	virtual ~SFX();
	virtual void loadFile(string);
	virtual void playSound();
};

#endif /* SFX_H_ */