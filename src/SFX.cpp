#include "SFX.h"

SFX::SFX() {
	// TODO Auto-generated constructor stub
	SDL_Init( SDL_INIT_AUDIO );
	type = 0;
	Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 6, 4096 );
}
SFX::~SFX() {
	// TODO Auto-generated destructor stub
    Mix_Quit();
    SDL_Quit();
}

void SFX::changeVolume(int channel, int volume)
{
	Mix_Volume(channel, volume);
}

void SFX::loadFile(string path) {

}

void SFX::playSound() {

}