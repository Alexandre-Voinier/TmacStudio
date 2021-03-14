#include <fmod.h>
#include "../includes/ui.h"
#include <stdio.h>

void Load(Ui *appwdgt, char* musique)
{
	FMOD_RESULT result;
	FMOD_SOUND *musi;

	FMOD_CHANNELGROUP *canal;
        FMOD_BOOL etat;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
        FMOD_ChannelGroup_GetPaused(canal, &etat);

	if (appwdgt->mus.musique != NULL)
	{
		FMOD_Sound_Release(appwdgt->mus.musique);
		if (appwdgt->mus.is_paused)
		{
			appwdgt->mus.is_paused = 0;
			FMOD_ChannelGroup_SetPaused(canal, 0);
		}
	}

	result = FMOD_System_CreateSound(appwdgt->mus.system, musique, FMOD_CREATESTREAM, 0, &(musi));
	if (result != FMOD_OK)
		g_print("Couldn't load the sound\n");
	else
	{
		appwdgt->mus.musique = musi;
		appwdgt->mus.is_paused = 0;
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), TRUE);
        	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), TRUE);
	}
}

void Play(Ui *appwdgt)
{
	if (appwdgt->mus.musique != NULL)
	{	
		FMOD_System_PlaySound(appwdgt->mus.system, appwdgt->mus.musique, 0, 0, NULL);
	}
}

void Pause(Ui *appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_BOOL etat;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
        FMOD_ChannelGroup_GetPaused(canal, &etat);

	if (appwdgt->mus.is_paused)
	{
		FMOD_ChannelGroup_SetPaused(canal, 0);
		appwdgt->mus.is_paused = 0;
	}
	else
	{
        	FMOD_ChannelGroup_SetPaused(canal, 1);
		appwdgt->mus.is_paused = 1;
	}
}

/*void Volume(float scale, MusStruct mus)
{
	FMOD_CHANNEL *canal;
	FMOD_System_GetMasterChannelGroup(mus.system, &canal);
	float result = scale*255;
	FMOD_Channel_SetVolume(canal, result);
}
*/
