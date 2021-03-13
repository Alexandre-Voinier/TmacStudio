#ifndef MUSIQUE_H
#define MUSIQUE_H

#include <fmod.h>
#include "ui.h"

//Structure

typedef struct MusStruct
{
        FMOD_SYSTEM *system;
        FMOD_SOUND *musique;

}MusStruct;

//Fonctions

void Load(Ui *appwdgt, char* musique);

void Play(Ui *appwdgt);

void Pause(Ui *appwdgt);

//void Volume(float scale, MusStruct mus);

#endif
