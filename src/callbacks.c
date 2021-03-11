#include "../includes/callbacks.h"
#include <fmod.h>

//============================= EDIT SCREEN ==================================//

void on_play_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
    // Allocates the system pointeur and initialize the library
    FMOD_SYSTEM *system;
    FMOD_System_Create(&system);
    FMOD_System_Init(system, 2, FMOD_INIT_NORMAL, NULL);
    FMOD_RESULT result;

    g_print("Pour l'instant tout se passe bien\n"); 
    FMOD_SOUND *door_bell = NULL;
    FMOD_System_CreateSound(system, "door.WAW", FMOD_CREATESAMPLE, 0, &door_bell);
    if (result != FMOD_OK)
    {
	g_print("Couldn't load the sound\n");
    }
    g_print("C'est parti on va jouer le son\n");
    FMOD_System_PlaySound(system, door_bell, FMOD_CHANNELORDER_DEFAULT, 0, NULL);
    sleep(2);
    FMOD_Sound_Release(door_bell);
    
    // Free the system pointeur
    FMOD_System_Close(system);
    FMOD_System_Release(system);
}

//============================= End Function =================================//
void on_window_destroy()
{
    gtk_main_quit();
}

