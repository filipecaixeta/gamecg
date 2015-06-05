#ifndef MUSIC_H_
#define MUSIC_H_

#include "SFX.h"

class Music: public SFX {
private:
	Mix_Music* music;
public:
	Music();
	virtual ~Music();
	void loadFile(string);
	void playSound();
	void pauseSound();
	void stopSound();
	int isPlaying();
	int isPaused();
	void changeVolume(int);
};

#endif /* MUSIC_H_ */