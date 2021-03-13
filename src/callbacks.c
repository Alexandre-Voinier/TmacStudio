#include "../includes/callbacks.h"
#include "../includes/ui.h"
#include <fmod.h>

//============================= EDIT SCREEN ==================================//


void on_play_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
    Load(appwdgt->mus, "test.mp3");
    g_print("C'est parti on va jouer le son\n");
    Play(appwdgt->mus);
}
s
void on_pause_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
	Pause(appwdgt->mus);
}

//============================= End Function =================================//
void on_window_destroy()
{
    gtk_main_quit();
}

