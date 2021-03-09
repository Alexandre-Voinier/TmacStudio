#include <fmod.h> 
#include <unistd.h>
#include "../includes/ui.h"
int main()
{
   /* // Allocates the system pointeur and initialize the library
    FMOD_SYSTEM *system;
    FMOD_System_Create(&system);
    FMOD_System_Init(system, 2, FMOD_INIT_NORMAL, NULL);
    FMOD_RESULT result;

    
    FMOD_SOUND *door_bell = NULL;
    FMOD_System_CreateSound(system, "door.WAW", FMOD_CREATESAMPLE, 0, &door_bell);
    if (result != FMOD_OK)
    {
	printf("CA MARHCE PAS\n");
	return 1;
    }
    FMOD_System_PlaySound(system, door_bell, FMOD_CHANNELORDER_DEFAULT, 0, NULL);
    sleep(2);
    FMOD_Sound_Release(door_bell);
    
    // Free the system pointeur
    FMOD_System_Close(system);
    FMOD_System_Release(system);*/
    return ui();
}
