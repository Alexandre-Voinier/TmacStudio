#include "../includes/callbacks.h"
#include "../includes/ui.h"
#include <fmod.h>

//============================= EDIT SCREEN ==================================//

void on_new_btn_activated(GtkMenuItem *btn, Ui *appwdgt)
{
	if (appwdgt->mus.musique != NULL)
	{
		FMOD_Sound_Release(appwdgt->mus.musique);
		FMOD_System_Close(appwdgt->mus.system);
    		FMOD_System_Release(appwdgt->mus.system);
		init_musStruct(appwdgt);
	}
}

void on_open_btn_activated(GtkMenuItem *btn, Ui *appwdgt)
{
	GtkWidget *dialog;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
        gint res;

        dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(appwdgt->edit.window),
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

        res = gtk_dialog_run (GTK_DIALOG (dialog));
        if (res == GTK_RESPONSE_ACCEPT)
        {
                char *filename;
                GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
                filename = gtk_file_chooser_get_filename (chooser);
                Load(appwdgt, filename);
        }
        gtk_widget_destroy (dialog);
}

void on_play_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
	Play(appwdgt);
}

void on_pause_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
	Pause(appwdgt);
}

void on_rec_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
	RecordStart(appwdgt);
}

void on_stop_btn_clicked(GtkButton *btn, Ui *appwdgt)
{
	RecordStop(appwdgt);
}

//============================= End Function =================================//
void on_window_destroy()
{
    gtk_main_quit();
}
