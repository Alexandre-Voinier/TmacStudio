#include <fmod.h>
#include "../includes/ui.h"
#include "../includes/callbacks.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void Load(Ui *appwdgt, char* musique, int s)
{
    FMOD_RESULT result;
    FMOD_SOUND *musi;

    int first = 0;
    if (appwdgt->mus.musique != NULL)
    {
	g_source_remove(appwdgt->wave.cursor);
	FMOD_Sound_Release(appwdgt->mus.musique);
	appwdgt->mus.soundlength = 0;
	appwdgt->mus.datalength = 0;
	if (!s)
		appwdgt->mus.fd = NULL;
	appwdgt->mus.save = 0;
	appwdgt->mus.isloop = 0;
	appwdgt->mus.coeff = 1;

	if (appwdgt->mus.is_paused)
	{
	    appwdgt->mus.is_paused = 0;
	    FMOD_ChannelGroup_SetPaused(appwdgt->mus.master, 0);
	}

	FMOD_BOOL mute;
	FMOD_ChannelGroup_GetMute(appwdgt->mus.master, &mute);
	if (mute)
	    FMOD_ChannelGroup_SetMute(appwdgt->mus.master, 0);

	if (appwdgt->spectre.created)
	{
	    g_source_remove(appwdgt->spectre.timeout);
	    FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.dspFFT);
	    FMOD_DSP_Release(appwdgt->mus.dspFFT);
	    appwdgt->spectre.created = 0;

	    if (appwdgt->spectre.hasheight)
            {
                 FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.height);
                 appwdgt->mus.height = NULL;
            }

	}
	if (appwdgt->spectre.hasheight)
        {
             FMOD_ChannelGroup_RemoveDSP(appwdgt->mus.master, appwdgt->mus.height);
             FMOD_DSP_Release(appwdgt->mus.height);
             appwdgt->spectre.hasheight = 0;
        }
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
	if (!s)
		gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), FALSE);
	appwdgt->wave.timer = 0;
	read_data(appwdgt);
	gtk_widget_queue_draw(appwdgt->wave.drawW);
	appwdgt->mus.play1 = 1;
    }
}

void Play(Ui *appwdgt)
{

	if (appwdgt->mus.musique != NULL)
	{
	    if (appwdgt->mus.is_paused)
	    {
		FMOD_ChannelGroup_SetPaused(appwdgt->mus.master, 0);
		FMOD_Channel_SetPaused(appwdgt->mus.channel, 0);
		appwdgt->mus.is_paused = 0;
	    }

	    if (appwdgt->spectre.created)
	    {
		g_source_remove(appwdgt->spectre.timeout);
		appwdgt->spectre.created = 0;

		FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.dspFFT);
		FMOD_DSP_Release(appwdgt->mus.dspFFT);

		if (appwdgt->spectre.hasheight)
		{
		    FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.height);
		    appwdgt->mus.height = NULL;
		}
	    }

	    if (appwdgt->spectre.hasheight)
	    {
		    FMOD_ChannelGroup_RemoveDSP(appwdgt->mus.master, appwdgt->mus.height);
		    FMOD_DSP_Release(appwdgt->mus.height);
		    appwdgt->spectre.hasheight = 0;
	    }

	    gtk_widget_queue_draw(appwdgt->spectre.visuSpectre);
		
	    appwdgt->wave.timer = 0;
	    FMOD_System_PlaySound(appwdgt->mus.system, appwdgt->mus.musique, appwdgt->mus.master, 0,
		    &(appwdgt->mus.channel));
	    float volume;
	    FMOD_ChannelGroup_GetVolume(appwdgt->mus.master, &volume);
	    FMOD_Channel_SetVolume(appwdgt->mus.channel, volume);
	    if(!appwdgt->spectre.created)
	    {
		appwdgt->spectre.created = 1;
		FMOD_System_CreateDSPByType(appwdgt->mus.system, FMOD_DSP_TYPE_FFT, &(appwdgt->mus.dspFFT));
		FMOD_Channel_AddDSP(appwdgt->mus.channel, FMOD_CHANNELCONTROL_DSP_HEAD, appwdgt->mus.dspFFT);
		FMOD_DSP_SetParameterInt(appwdgt->mus.dspFFT, FMOD_DSP_FFT_WINDOWSIZE, 1024);
		appwdgt->spectre.timeout = g_timeout_add(100, G_SOURCE_FUNC(get_spectre), appwdgt);
	    }
	    FMOD_BOOL mute;
	    FMOD_ChannelGroup_GetMute(appwdgt->mus.master, &mute);
	    FMOD_Channel_SetMute(appwdgt->mus.channel, mute);
	    Height(appwdgt, appwdgt->mus.coeff);
	    appwdgt->wave.cursor = g_timeout_add_seconds(1, G_SOURCE_FUNC(draw), appwdgt);
	    if (appwdgt->mus.play1)
	    {
		appwdgt->mus.play1 = 0;
		usleep(500000);
		Play(appwdgt);
	    }
	}
}

void Pause(Ui *appwdgt)
{

	if (appwdgt->mus.is_paused)
	{
		FMOD_ChannelGroup_SetPaused(appwdgt->mus.master, 0);
		if (appwdgt->spectre.created)
		    FMOD_Channel_SetPaused(appwdgt->mus.channel, 0);
		appwdgt->mus.is_paused = 0;
	}
	else
	{
        	FMOD_ChannelGroup_SetPaused(appwdgt->mus.master, 1);
		if (appwdgt->spectre.created)
		    FMOD_Channel_SetPaused(appwdgt->mus.channel, 1);
		
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
			appwdgt->mus.isloop = 0;
        		appwdgt->mus.coeff = 1;

                	if (appwdgt->mus.is_paused)
                	{
                        	appwdgt->mus.is_paused = 0;
                        	FMOD_ChannelGroup_SetPaused(canal, 0);
                	}

			if (appwdgt->spectre.created)
            		{
                		g_source_remove(appwdgt->spectre.timeout);
                		appwdgt->spectre.created = 0;

                		FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.dspFFT);
                		FMOD_DSP_Release(appwdgt->mus.dspFFT);

                		if (appwdgt->spectre.hasheight)
                		{
                    			FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.height);
                    			appwdgt->mus.height = NULL;
                		}
            		}

            		if (appwdgt->spectre.hasheight)
            		{
                    		FMOD_ChannelGroup_RemoveDSP(appwdgt->mus.master, appwdgt->mus.height);
                    		FMOD_DSP_Release(appwdgt->mus.height);
                    		appwdgt->spectre.hasheight = 0;
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
    		exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 3;
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

			appwdgt->wave.record = 1;
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
		appwdgt->wave.record = 0;
		Load(appwdgt, ".record.wav", 1);

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

void Message(Ui *appwdgt)
{

	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(appwdgt->edit.TextS));
	gtk_text_buffer_set_text(buffer, "Choose a command between:\n    -play\n    -pause\n    -rec\n    -recstop\n    - mute\n    - loop\n    - height\n    - clear\n    - exit\n", 127);
	g_source_remove(appwdgt->mus.save);
}

void on_entry_activated(GtkWidget *entry, Ui *appwdgt)
{
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(appwdgt->edit.TextS));
	// tu peux utiliser cette chaine comme la chaine globale entrée :)
	const gchar* chaine = gtk_entry_get_text(GTK_ENTRY(entry));
	if (Compare((char*)(chaine),"recstop",7) == 0)
		RecordStop(appwdgt);
	else
	{
		if (Compare((char*)(chaine),"rec",3) == 0)
			RecordStart(appwdgt);
		else
		{
			if (Compare((char*)(chaine),"pause",5) == 0)
				Pause(appwdgt);
			else
			{
				if (Compare((char*)(chaine),"play",4) == 0)
					Play(appwdgt);
				else
				{
					if (Compare((char*)(chaine),"mute",4) == 0)
					{
							Mute(appwdgt);
						gtk_text_buffer_set_text(buffer, "The sound has been muted", 26);
					}
					else
					{
						if (Compare((char*)(chaine),"exit",4) == 0)
							gtk_main_quit();

						else
						{
							if (Compare((char*)(chaine),"loop",4) == 0)
							{
								Loop(appwdgt,(int)(strtol((char*)(chaine+5),NULL,10)));
								gtk_text_buffer_set_text(buffer, "The loop effect has been modified", 33);
							}
							else
							{
								if (Compare((char*)(chaine),"height",6) == 0)
								{
									Height(appwdgt,strtof((char*)(chaine+7),NULL));
									gtk_text_buffer_set_text(buffer, "The height has been modified", 28);
								}
							}
						}
					}
				}
			}
		}
	}
	
	gtk_editable_delete_text(GTK_EDITABLE(entry), 0, -1); // ça ça clean le texte tapé dans l'entré
       appwdgt->mus.save = g_timeout_add_seconds(3, G_SOURCE_FUNC(Message), appwdgt);	
}

void draw(Ui *appwdgt)
{
	g_source_remove(appwdgt->wave.cursor);
	unsigned int ip;
        FMOD_Channel_IsPlaying(appwdgt->mus.channel, &ip);
	if (ip)
	{
	    appwdgt->wave.cursor = g_timeout_add_seconds(1, G_SOURCE_FUNC(draw), appwdgt);
	    if (!appwdgt->mus.is_paused)
	    {
		appwdgt->wave.timer += 1;
		gtk_widget_queue_draw(appwdgt->wave.drawW);
	    }
	}
}

void Attach(Ui *appwdgt)
{
	//On s'occupe de la zone du slider
	GtkWidget *new1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	GtkWidget *slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
	gtk_scale_set_draw_value(GTK_SCALE(slider), FALSE);
	g_signal_connect(slider, "value_changed", G_CALLBACK(Volume), appwdgt);

	appwdgt->wave.drawW = gtk_drawing_area_new();
	appwdgt->wave.tab = NULL;
	appwdgt->wave.sound_length_pcm_bytes = 0;
	appwdgt->wave.sound_length_s = 0;
	appwdgt->wave.record = 0;
	appwdgt->wave.timer = 0;
	appwdgt->wave.r = 0;
	g_signal_connect(appwdgt->wave.drawW, "draw", G_CALLBACK(on_draw_wave), appwdgt);

	gtk_box_pack_start(GTK_BOX(new1), appwdgt->wave.drawW, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(new1), slider, TRUE, TRUE, 0);	
		
	//On s'occupe du shell et du spectre
	GtkWidget *new2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *shell = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	for (int i = 0; i < 512; i++)
	{
	    appwdgt->spectre.rects[i].x = 0;
	    appwdgt->spectre.rects[i].y = 0;
	    appwdgt->spectre.rects[i].width = 0;
	    appwdgt->spectre.rects[i].height = 0;
	}
	appwdgt->spectre.visuSpectre = gtk_drawing_area_new();
	g_signal_connect(appwdgt->spectre.visuSpectre, "draw", G_CALLBACK(on_draw_spectrum), appwdgt);
	GtkWidget *TextS = gtk_text_view_new();
	appwdgt->edit.TextS = TextS;
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(appwdgt->edit.TextS));
        gtk_text_buffer_set_text(buffer, "Choose a command between:\n    - mute\n    - loop\n    - height\n    - clear\n    - exit\n et bientot d'autres\n", 104);
	GtkWidget *entry = gtk_entry_new();
	g_signal_connect(entry, "activate", G_CALLBACK(on_entry_activated), appwdgt);
	
	gtk_box_pack_start(GTK_BOX(shell), TextS, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(shell), entry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(new2), appwdgt->spectre.visuSpectre, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(new2), shell, TRUE, TRUE, 0);

	//ici on ajouter les deux nouvelles zones sous les boutons
	gtk_box_pack_start(appwdgt->edit.grille, new1, TRUE, TRUE, 0);
	gtk_box_pack_start(appwdgt->edit.grille, new2, TRUE, TRUE, 0);
	
        FMOD_ChannelGroup_SetVolume(appwdgt->mus.master, 0);

	gtk_widget_show_all(GTK_WIDGET(appwdgt->edit.window));
}


void Volume(GtkWidget *slider, Ui *appwdgt)
{
	float value = gtk_range_get_value(GTK_RANGE(slider)); 
	FMOD_ChannelGroup_SetVolume(appwdgt->mus.master, value);
	if (appwdgt->spectre.created)
	    FMOD_Channel_SetVolume(appwdgt->mus.channel, value);
}

void Mute(Ui* appwdgt)
{
    FMOD_RESULT result;
    FMOD_BOOL mute;

    result = FMOD_ChannelGroup_GetMute(appwdgt->mus.master, &mute);
    if (result != FMOD_OK)
	g_print("error while getting the mute's mode\n");
    else
    {
	if (mute)
	{
	    FMOD_ChannelGroup_SetMute(appwdgt->mus.master, 0);
	    if (appwdgt->spectre.created)
		FMOD_Channel_SetMute(appwdgt->mus.channel, 0);
	}
	else
	{
	    FMOD_ChannelGroup_SetMute(appwdgt->mus.master, 1);
	    if (appwdgt->spectre.created)
		FMOD_Channel_SetMute(appwdgt->mus.channel, 1);
	}
    }
}
void Loop(Ui *appwdgt, int booleen)
{
    if (booleen)
	appwdgt->mus.isloop = 1;
    else
	appwdgt->mus.isloop = 0;
}
//à mettre dans le shell
void Height(Ui *appwdgt, float coef)
{
    if (appwdgt->mus.height == NULL)
    {
	FMOD_DSP *height;
	FMOD_System_CreateDSPByType(appwdgt->mus.system, FMOD_DSP_TYPE_PITCHSHIFT, &height);
	appwdgt->mus.height = height;
	FMOD_ChannelGroup_AddDSP(appwdgt->mus.master, FMOD_CHANNELCONTROL_DSP_TAIL, appwdgt->mus.height);

	if (appwdgt->spectre.hasheight == 0 && appwdgt->spectre.created)
	{
	    FMOD_Channel_AddDSP(appwdgt->mus.channel, FMOD_CHANNELCONTROL_DSP_TAIL, appwdgt->mus.height);
	    appwdgt->spectre.hasheight = 1;
	}
    }
    FMOD_DSP_SetParameterFloat(appwdgt->mus.height, 0, coef);
    appwdgt->mus.coeff = coef;
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

void get_spectre(Ui *appwdgt)
{
	unsigned int ip;
	FMOD_Channel_IsPlaying(appwdgt->mus.channel, &ip);
	if (!ip)
		clean_spectre(appwdgt);
	else
	{
		g_source_remove(appwdgt->spectre.timeout);
		appwdgt->spectre.timeout= g_timeout_add(100, G_SOURCE_FUNC(get_spectre), appwdgt);
		FMOD_DSP_GetParameterData(appwdgt->mus.dspFFT, FMOD_DSP_FFT_SPECTRUMDATA,
				(void **)&(appwdgt->mus.paramFFT), 0, 0, 0);
		int width, height, xs;
		width = gtk_widget_get_allocated_width(appwdgt->spectre.visuSpectre);
		height = gtk_widget_get_allocated_height(appwdgt->spectre.visuSpectre);
		xs = width / 512;
		for (int i = 0; i < 512; i++)
		{
			
			if (appwdgt->mus.paramFFT->spectrum[0] != NULL)
			{
				float value = appwdgt->mus.paramFFT->spectrum[0][i] * 20;

				appwdgt->spectre.rects[i].x = i * xs;
				appwdgt->spectre.rects[i].y = height;
				appwdgt->spectre.rects[i].width = xs;
				appwdgt->spectre.rects[i].height = (int)(value * (height / 5)) * 2;

				if (appwdgt->spectre.rects[i].height > height)
					appwdgt->spectre.rects[i].height = height;

				appwdgt->spectre.rects[i].height *= (-1);
			}
		}
		gtk_widget_queue_draw(appwdgt->spectre.visuSpectre);
	}

}
void clean_spectre(Ui *appwdgt)
{
    if (appwdgt->mus.isloop)
	Play(appwdgt);
    else
    {
	g_source_remove(appwdgt->spectre.timeout);
	FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.dspFFT);
	FMOD_DSP_Release(appwdgt->mus.dspFFT);
	appwdgt->spectre.created = 0;
	gtk_widget_queue_draw(appwdgt->spectre.visuSpectre);
	if (appwdgt->spectre.hasheight)
	{
	    FMOD_ChannelGroup_RemoveDSP(appwdgt->mus.master, appwdgt->mus.height);
	    FMOD_Channel_RemoveDSP(appwdgt->mus.channel, appwdgt->mus.height);
	    FMOD_DSP_Release(appwdgt->mus.height);
	    appwdgt->mus.height = NULL;
    }
	appwdgt->spectre.hasheight = 0;
    }
}

void read_data(Ui *appwdgt)
{
	FMOD_RESULT result;
	result = FMOD_Sound_GetLength(appwdgt->mus.musique, &appwdgt->wave.sound_length_pcm_bytes, FMOD_TIMEUNIT_PCMBYTES);
	result = FMOD_Sound_GetLength(appwdgt->mus.musique, &appwdgt->wave.sound_length_s, FMOD_TIMEUNIT_MS);
	if (result != FMOD_OK)
		g_print("problème sur Getlength\n");

	appwdgt->wave.sound_length_s = appwdgt->wave.sound_length_s*0.001f;

	appwdgt->wave.tab = malloc(appwdgt->wave.sound_length_pcm_bytes*sizeof(char));
	if (appwdgt->wave.tab == NULL)
		g_print("ça a pas marché le malloc\n");

	FMOD_Sound_SeekData(appwdgt->mus.musique, 0);
	result = FMOD_Sound_ReadData(appwdgt->mus.musique, appwdgt->wave.tab, appwdgt->wave.sound_length_pcm_bytes, &appwdgt->wave.r);
	
        if (result!=FMOD_OK)
                g_print("problème sur le readdata\n");
	
	for (int i = 0; i<appwdgt->wave.sound_length_pcm_bytes; i++)
        {
                if (i<appwdgt->wave.r)
                {
                        if (appwdgt->wave.tab[i] == 0)
                                appwdgt->wave.tab[i] = 1;
                        if (appwdgt->wave.tab[i] < 0)
                                appwdgt->wave.tab[i] = -(appwdgt->wave.tab[i]);
                }
                else
                        appwdgt->wave.tab[i] = 0;
        }
}

