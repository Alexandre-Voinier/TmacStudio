#include <fmod.h>
#include "../includes/musique.h"
#include "../includes/ui.h"
#include <stdio.h>

void Load(Ui *appwdgt, char* musique)
{
	FMOD_RESULT result;
	FMOD_SOUND *musi;
	result = FMOD_System_CreateSound(appwdgt->mus.system, "test.mp3", FMOD_CREATESTREAM, 0, &(musi));
	if (result != FMOD_OK)
		g_print("Couldn't load the sound\n");
	appwdgt->mus.musique = musi;
}

void Play(Ui *appwdgt)
{
	FMOD_System_PlaySound(appwdgt->mus.system, appwdgt->mus.musique, 0, 0, NULL);
	g_print("ça joue bien\n");
}

void Pause(Ui *appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_BOOL etat;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
        FMOD_ChannelGroup_GetPaused(canal, &etat);

        if (etat) // Si la chanson est en pause
                FMOD_ChannelGroup_SetPaused(canal, 0); // On enlève la pause
        else // Sinon, elle est en cours de lecture
                FMOD_ChannelGroup_SetPaused(canal, 1); // On active la pause
}

/*void Volume(float scale, MusStruct mus)
{
	FMOD_CHANNEL *canal;
	FMOD_System_GetMasterChannelGroup(mus.system, &canal);
	float result = scale*255;
	FMOD_Channel_SetVolume(canal, result);
}
*/
