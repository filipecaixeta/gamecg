#ifndef SFX_H_
#define SFX_H_
#include "SDL.h"
#include "SDL_mixer.h"
#include <iostream>

#define LOOPING -1
#define ONCE 0

using namespace std;

class SFX {
protected:
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