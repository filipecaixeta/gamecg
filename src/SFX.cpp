#include "SFX.h"

SFX::SFX() {
	// TODO Auto-generated constructor stub
	SDL_Init( SDL_INIT_AUDIO );
	type = 0;
	Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
}

SFX::~SFX() {
	// TODO Auto-generated destructor stub
    Mix_Quit();
    SDL_Quit();
}

void SFX::loadFile(string path) {

}

void SFX::playSound() {

}