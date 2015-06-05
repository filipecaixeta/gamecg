#include "SFX.h"

SFX::SFX(int channel) {
	// TODO Auto-generated constructor stub
	SDL_Init( SDL_INIT_AUDIO );
	type = 0;
	this->channel = channel;
	Mix_OpenAudio( this->channel, MIX_DEFAULT_FORMAT, 2, 4096 );
}

SFX::SFX() {
	// TODO Auto-generated constructor stub
	SDL_Init( SDL_INIT_AUDIO );
	type = 0;
	this->channel = 44100;
	Mix_OpenAudio( this->channel, MIX_DEFAULT_FORMAT, 2, 4096 );
}

SFX::~SFX() {
	// TODO Auto-generated destructor stub
    Mix_Quit();
    SDL_Quit();
}

void SFX::changeVolume(int volume)
{
	Mix_Volume(channel, volume);
}

void SFX::loadFile(string path) {

}

void SFX::playSound() {

}