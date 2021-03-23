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

		gtk_widget_destroy(GTK_WIDGET(appwdgt->edit.grille));
		GtkWidget *new1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_box_pack_start(appwdgt->edit.grilleG, new1, FALSE, FALSE, 0);
		appwdgt->edit.grille = GTK_BOX(new1);

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

void on_save_btn_activated(GtkMenuItem *btn, Ui *appwdgt)
{
	if (appwdgt->mus.musique != NULL)
        {
                GtkWidget *dialog;
                GtkFileChooser *chooser;
                GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
                gint res;

                dialog = gtk_file_chooser_dialog_new ("Save File",
                                      NULL,
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Save"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
                chooser = GTK_FILE_CHOOSER (dialog);

                gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

                gtk_file_chooser_set_current_name (chooser,
                                     ("record.WAV"));

                res = gtk_dialog_run (GTK_DIALOG (dialog));
                if (res == GTK_RESPONSE_ACCEPT)
                {
                        char *filename;
                        filename = gtk_file_chooser_get_filename (chooser);
                        
			FILE *fp = fopen(filename, "wb");
			
    			if (!fp)
    			{
        			printf("ERROR : could not open the file for writing.\n");
    			}
			else
				printf("%s\n", filename);
				//WriteWavHeader(fp, appwdgt->mus.musique, 0);

                        g_free (filename);
                }

                gtk_widget_destroy (dialog);
        }

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
