#include <fmod.h>
#include "../includes/ui.h"
#include <stdio.h>

void Load(Ui *appwdgt, char* musique)
{
	FMOD_RESULT result;
	FMOD_SOUND *musi;

	FMOD_CHANNELGROUP *canal;
	FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);

	if (appwdgt->mus.musique != NULL)
	{
		FMOD_Sound_Release(appwdgt->mus.musique);
		if (appwdgt->mus.is_paused)
		{
			appwdgt->mus.is_paused = 0;
			FMOD_ChannelGroup_SetPaused(canal, 0);
		}
	}
	else
		Attach(appwdgt);

	result = FMOD_System_CreateSound(appwdgt->mus.system, musique, FMOD_CREATESTREAM, 0, &(musi));
	if (result != FMOD_OK)
		g_print("Couldn't load the sound\n");
	else
	{
		appwdgt->mus.musique = musi;
		appwdgt->mus.is_paused = 0;
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), TRUE);
        	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), FALSE);
                gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), FALSE);
	}
}

void Play(Ui *appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);

	if (appwdgt->mus.musique != NULL)
	{
		if (appwdgt->mus.is_paused)
		{
			FMOD_ChannelGroup_SetPaused(canal, 0);
                	appwdgt->mus.is_paused = 0;
		}	
		FMOD_System_PlaySound(appwdgt->mus.system, appwdgt->mus.musique, 0, 0, NULL);
	}
}

void Pause(Ui *appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);

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

void RecordStart(Ui *appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);

        if (appwdgt->mus.musique != NULL)
        {
                FMOD_Sound_Release(appwdgt->mus.musique);
                if (appwdgt->mus.is_paused)
                {
                        appwdgt->mus.is_paused = 0;
                        FMOD_ChannelGroup_SetPaused(canal, 0);
                }
        }
	else
		Attach(appwdgt);

	FMOD_CREATESOUNDEXINFO exinfo;
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    	exinfo.numchannels = 1;
    	exinfo.format = FMOD_SOUND_FORMAT_PCM16;
    	exinfo.defaultfrequency = 44100;
    	exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 150;

	FMOD_RESULT result;

	result = FMOD_System_CreateSound(appwdgt->mus.system, 0, FMOD_2D | FMOD_OPENUSER | FMOD_CREATESAMPLE , &exinfo, &(appwdgt->mus.musique));

	if (result != FMOD_OK)
		g_print("cannot create the sound");
	else
	{
		int num;
    		FMOD_System_GetRecordNumDrivers(appwdgt->mus.system, NULL, &num);
    		for (int count = 0; count < num; count++)
    		{
        		char name[256];

        		FMOD_System_GetRecordDriverInfo(appwdgt->mus.system, count, name, 256, NULL, NULL, NULL, NULL, NULL);
        		printf("%d : %s\n", count, name);
    		}
    		FMOD_System_RecordStart(appwdgt->mus.system,0, appwdgt->mus.musique, 0);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), FALSE);
        	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), FALSE);
	}
}

void RecordStop(Ui *appwdgt)
{
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), TRUE);

	FMOD_System_RecordStop(appwdgt->mus.system,0);
    	g_print("%d", appwdgt->mus.musique == NULL);
}

void Attach(Ui *appwdgt)
{
	GtkWidget *new = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	GtkWidget *slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
	gtk_scale_set_draw_value(GTK_SCALE(slider), FALSE);
	g_signal_connect(slider, "value_changed", G_CALLBACK(Volume), appwdgt);

	GtkWidget *draw = gtk_drawing_area_new();

	gtk_box_pack_start(GTK_BOX(new), draw, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(new), slider, TRUE, TRUE, 0);

	gtk_box_pack_start(appwdgt->edit.grille, new, FALSE, FALSE, 0);
	FMOD_CHANNELGROUP *canal;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
        FMOD_ChannelGroup_SetVolume(canal, 0);

	gtk_widget_show_all(GTK_WIDGET(appwdgt->edit.window));
}

void Volume(GtkWidget *slider, Ui *appwdgt)
{
	float value = gtk_range_get_value(GTK_RANGE(slider)); 
	FMOD_CHANNELGROUP *canal;
	FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
	FMOD_ChannelGroup_SetVolume(canal, value);
}

void WriteWavHeader(FILE *fp, Ui *appwdgt, int length)
{
	int             channels, bits;
    	float           rate;

    	fseek(fp, 0, SEEK_SET);

	FMOD_Sound_GetFormat(appwdgt->mus.musique, 0, 0, &channels, &bits);
    	//sound->getFormat  (0, 0, &channels, &bits);
	FMOD_Sound_GetDefaults(appwdgt->mus.musique, &rate, 0);
    	//sound->getDefaults(&rate, 0, 0, 0);

    	//{
       		#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        	#pragma pack(1)
        	#endif

        	/*
            	WAV Structures
        	*/
        	typedef struct
        	{
            		signed char id[4];
            		int 		size;
        	} RiffChunk;

        	struct
        	{
            		RiffChunk       chunk;
            		unsigned short	wFormatTag;    /* format type  */
            		unsigned short	nChannels;    /* number of channels (i.e. mono, stereo...)  */
            		unsigned int	nSamplesPerSec;    /* sample rate  */
            		unsigned int	nAvgBytesPerSec;    /* for buffer estimation  */
            		unsigned short	nBlockAlign;    /* block size of data  */
            		unsigned short	wBitsPerSample;    /* number of bits per sample of mono data */
        	} FmtChunk  = {{{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };

        	struct
        	{
            		RiffChunk   chunk;
        	} DataChunk = {{{'d','a','t','a'}, length }};

        	struct
        	{
            		RiffChunk   chunk;
            		signed char rifftype[4];
        	} WavHeader = {{{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + length }, {'W','A','V','E'} };

        	#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        	#pragma pack()
        	#endif

        	/*
            	Write out the WAV header.
        	*/
        	fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
        	fwrite(&FmtChunk, sizeof(FmtChunk), 1, fp);
        	fwrite(&DataChunk, sizeof(DataChunk), 1, fp);
    	//}
}

