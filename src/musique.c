#include <fmod.h>
#include "../includes/ui.h"
#include <stdio.h>

void Load(Ui *appwdgt, char* musique)
{
	FMOD_RESULT result;
	FMOD_SOUND *musi;

	FMOD_CHANNELGROUP *canal;
	FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);

	int first = 0;
	if (appwdgt->mus.musique != NULL)
	{
		FMOD_Sound_Release(appwdgt->mus.musique);
		appwdgt->mus.soundlength = 0;
                appwdgt->mus.datalength = 0;
                appwdgt->mus.fd = NULL;
               	appwdgt->mus.save = 0;

		if (appwdgt->mus.is_paused)
		{
			appwdgt->mus.is_paused = 0;
			FMOD_ChannelGroup_SetPaused(canal, 0);
		}
		
		FMOD_BOOL mute;
		FMOD_ChannelGroup_GetMute(canal, &mute);
		if (mute)
			FMOD_ChannelGroup_SetMute(canal, 0);
	}
	else
		first = 1;

	result = FMOD_System_CreateSound(appwdgt->mus.system, musique, FMOD_CREATESTREAM, 0, &(musi));
	if (result != FMOD_OK)
		g_print("Couldn't load the sound\n");
	else
	{
		if (first)
			Attach(appwdgt);
			
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

void Save(Ui *appwdgt)
{
	 static unsigned int lastrecordpos = 0;
         unsigned int recordpos = 0;

         FMOD_RESULT result = FMOD_System_GetRecordPosition(appwdgt->mus.system, 0, &recordpos);
         if (result != FMOD_OK)
		 g_print("cannot get recordpos\n");
	 else
	 {
	 	if (recordpos != lastrecordpos)
	 	{
			void *ptr1, *ptr2;
         	        int blocklength;
                	unsigned int len1, len2;

                 	blocklength = (int)recordpos - (int)lastrecordpos;
                 	if (blocklength < 0)
				 blocklength += appwdgt->mus.soundlength;

                 	FMOD_Sound_Lock(appwdgt->mus.musique, lastrecordpos * appwdgt->mus.exinfo.numchannels * 2, blocklength * appwdgt->mus.exinfo.numchannels * 2, &ptr1, &ptr2, &len1, &len2);

                 	if (ptr1 && len1)
				 appwdgt->mus.datalength += fwrite(ptr1, 1, len1, appwdgt->mus.fd);
                 	if (ptr2 && len2)
                        	 appwdgt->mus.datalength += fwrite(ptr2, 1, len2, appwdgt->mus.fd);

                 	FMOD_Sound_Unlock(appwdgt->mus.musique, ptr1, ptr2, len1, len2);
		}
         }
         FMOD_System_Update(appwdgt->mus.system);
}

void RecordStart(Ui *appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
	
	if (appwdgt->mus.is_recording == 0)
	{

        	if (appwdgt->mus.musique != NULL)
        	{
                	FMOD_Sound_Release(appwdgt->mus.musique);

			appwdgt->mus.soundlength = 0;
			appwdgt->mus.datalength = 0;
			appwdgt->mus.fd = NULL;
			appwdgt->mus.save = 0;

                	if (appwdgt->mus.is_paused)
                	{
                        	appwdgt->mus.is_paused = 0;
                        	FMOD_ChannelGroup_SetPaused(canal, 0);
                	}
			
			FMOD_BOOL mute;
                	FMOD_ChannelGroup_GetMute(canal, &mute);
                	if (mute)
                        	FMOD_ChannelGroup_SetMute(canal, 0);

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
		appwdgt->mus.exinfo = exinfo;

		FMOD_RESULT result;

		result = FMOD_System_CreateSound(appwdgt->mus.system, 0, FMOD_2D | FMOD_OPENUSER | FMOD_CREATESAMPLE , &exinfo, &(appwdgt->mus.musique));

		if (result != FMOD_OK)
			g_print("cannot create the sound\n");
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
			gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), FALSE);
        		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), FALSE);
                        gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), TRUE);

			appwdgt->mus.is_recording = 1;
			
			appwdgt->mus.fd = fopen(".record.wav", "wb");
			if (!appwdgt->mus.fd)
        			printf("ERROR : could not open record.wav for writing.\n");
			else
			{

				WriteWavHeader(appwdgt->mus.fd, appwdgt, appwdgt->mus.datalength);	
				result = FMOD_Sound_GetLength(appwdgt->mus.musique, &(appwdgt->mus.soundlength), FMOD_TIMEUNIT_PCM);
				if (result != FMOD_OK)
					g_print("cannot get the length\n");
				else
					appwdgt->mus.save = g_timeout_add_seconds(10, G_SOURCE_FUNC(Save), appwdgt);
			}
		}
	}
}

void RecordStop(Ui *appwdgt)
{
	if (appwdgt->mus.is_recording == 1)
	{
		Save(appwdgt);
		g_source_remove(appwdgt->mus.save);
		appwdgt->mus.is_recording = 0;
		WriteWavHeader(appwdgt->mus.fd, appwdgt, appwdgt->mus.datalength);
		fclose(appwdgt->mus.fd);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), TRUE);
                gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), FALSE);

		FMOD_System_RecordStop(appwdgt->mus.system,0);

    		g_print("%d", appwdgt->mus.musique == NULL);
	}
}

int Compare(char* str1, char* str2, size_t n)
{
    for(size_t k = 0;k<n;k++)
    {
        if (*(str1+k)!=*(str2+k))
            return -1;
    }
    return 0;
}

void on_entry_activated(GtkWidget *entry, Ui *appwdgt)
{
	// tu peux utiliser cette chaine comme la chaine globale entrée :)
	const gchar* chaine = gtk_entry_get_text(GTK_ENTRY(entry));

	if (Compare((char*)(chaine),"reverb",6) == 0)
        printf("You have called the reverb function wolla\n");
            
    if(Compare((char*)(chaine),"clear",5) == 0)
        gtk_editable_delete_text(GTK_EDITABLE(entry), 0, -1);

    if (Compare((char*)(chaine),"exit",4) == 0)
        gtk_main_quit();

    if (Compare((char*)(chaine),"cut",3) == 0)
        printf("You have called the cut function wolla\n");

	//ci-dessous, pour modifier le text du shell 
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(appwdgt->edit.TextS));
	gtk_text_buffer_set_text(buffer, "chaine à saisir", 15); //où 15 est la longueur de la chaineà set.	
	gtk_text_buffer_set_text(buffer, "", 0);//et si tu veux clean le texte, tu mets ça
	
	//et pour ton instruction exit, tu peux utiliser gtk_main_quit(). ça fermera tout.
	
	gtk_editable_delete_text(GTK_EDITABLE(entry), 0, -1); // ça ça clean le texte tapé dans l'entré 
}

void Attach(Ui *appwdgt)
{
	//On s'occupe de la zone du slider
	GtkWidget *new1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	GtkWidget *slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
	gtk_scale_set_draw_value(GTK_SCALE(slider), FALSE);
	g_signal_connect(slider, "value_changed", G_CALLBACK(Volume), appwdgt);

	GtkWidget *draw = gtk_drawing_area_new();
	
	gtk_box_pack_start(GTK_BOX(new1), draw, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(new1), slider, TRUE, TRUE, 0);	
		
	//On s'occupe du shell et du spectre
	GtkWidget *new2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *shell = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	GtkWidget *zoneSpectre = gtk_drawing_area_new();
	GtkWidget *TextS = gtk_text_view_new();
	appwdgt->edit.TextS = TextS;
	GtkWidget *entry = gtk_entry_new();
	g_signal_connect(entry, "activate", G_CALLBACK(on_entry_activated), appwdgt);
	
	gtk_box_pack_start(GTK_BOX(shell), TextS, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(shell), entry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(new2), zoneSpectre, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(new2), shell, TRUE, TRUE, 0);

	//ici on ajouter les deux nouvelles zones sous les boutons
	gtk_box_pack_start(appwdgt->edit.grille, new1, TRUE, TRUE, 0);
	gtk_box_pack_start(appwdgt->edit.grille, new2, TRUE, TRUE, 0);
	
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
//à mettre dans le shell
void Mute(Ui* appwdgt)
{
	FMOD_CHANNELGROUP *canal;
        FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
        FMOD_RESULT result;
	FMOD_BOOL mute;

	result = FMOD_ChannelGroup_GetMute(canal, &mute);
	if (result != FMOD_OK)
                g_print("error while getting the mute's mode\n");
	else
	{
		if (mute)
			FMOD_ChannelGroup_SetMute(canal, 0);
		else
			FMOD_ChannelGroup_SetMute(canal, 1);
	}
}
//à mettre dans le shell
void Loop(Ui *appwdgt, int booleen)
{
	if (booleen)
		FMOD_Sound_SetMode(appwdgt->mus.musique, FMOD_LOOP_NORMAL);
	else
		FMOD_Sound_SetMode(appwdgt->mus.musique, FMOD_LOOP_OFF);
}
//à mettre dans le shell
void Height(Ui *appwdgt, float coef)
{
	if (appwdgt->mus.height == NULL)
	{
		FMOD_DSP *height;
		FMOD_System_CreateDSPByType(appwdgt->mus.system, FMOD_DSP_TYPE_PITCHSHIFT, &height);
		appwdgt->mus.height = height;
		FMOD_CHANNELGROUP *canal;
        	FMOD_System_GetMasterChannelGroup(appwdgt->mus.system, &canal);
		FMOD_ChannelGroup_AddDSP(canal, FMOD_CHANNELCONTROL_DSP_TAIL, appwdgt->mus.height);
	}
	FMOD_DSP_SetParameterFloat(appwdgt->mus.height, 0, coef);
}

void WriteWavHeader(FILE *fp, Ui *appwdgt, int length)
{
	int             channels, bits;
    	float           rate;

    	fseek(fp, 0, SEEK_SET);

	FMOD_Sound_GetFormat(appwdgt->mus.musique, 0, 0, &channels, &bits);
	FMOD_Sound_GetDefaults(appwdgt->mus.musique, &rate, 0);

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

