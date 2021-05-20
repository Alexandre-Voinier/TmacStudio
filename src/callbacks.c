#include "../includes/callbacks.h"
#include "../includes/ui.h"
#include <fmod.h>
#include <unistd.h>
#include <wait.h>

//============================= EDIT SCREEN ==================================//

void on_new_btn_activated(GtkMenuItem *btn, Ui *appwdgt)
{
	if (appwdgt->mus.musique != NULL)
	{
		appwdgt->mus.isloop = 0;
		clean_spectre(appwdgt);
		FMOD_Sound_Release(appwdgt->mus.musique);
		FMOD_System_Close(appwdgt->mus.system);
    		FMOD_System_Release(appwdgt->mus.system);

		gtk_widget_destroy(GTK_WIDGET(appwdgt->edit.grille));
		GtkWidget *new1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_box_pack_start(appwdgt->edit.grilleG, new1, TRUE, TRUE, 0);
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
	if (appwdgt->mus.musique != NULL && appwdgt->mus.fd != NULL)
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
                                     ("record.wav"));

                res = gtk_dialog_run (GTK_DIALOG (dialog));
                if (res == GTK_RESPONSE_ACCEPT)
                {
                        char *filename;
                        filename = gtk_file_chooser_get_filename (chooser);
                        
			
    			if (fork())
        			wait(NULL);
			else
			{
				char *arg[4];
				arg[0] = "cp";
				arg[1] = ".record.wav";
				arg[2] = filename;
				arg[3] = NULL;

				execvp(arg[0], arg);
			}
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

void on_draw_spectrum(GtkWidget *drawarea, cairo_t *cr, Ui *appwdgt)
{
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    if (appwdgt->spectre.created)
    {
	cairo_set_source_rgb(cr, 1, 0, 0);
	for (int i = 0; i < 512; i++)
	{
	    GdkRectangle rect = appwdgt->spectre.rects[i];
	    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	    cairo_fill(cr);
	}
    }
}

void on_draw_wave(GtkWidget *drawarea, cairo_t *cr, Ui *appwdgt)
{
	cairo_set_source_rgb(cr, 1, 1, 1);
    	cairo_paint(cr);
	
	int width = gtk_widget_get_allocated_width(appwdgt->wave.window);
        int height = (gtk_widget_get_allocated_height(appwdgt->wave.window))/2;
	float heightscale = (float)height * 0.75f;
	float *wave = malloc(width*sizeof(float));
	int r;
	FMOD_RESULT result;
	result = FMOD_Sound_ReadData(appwdgt->mus.musique, wave, width, &r);
	if (result != FMOD_OK)
		g_print("couldn't load the music data with readdata\n");
	for (int i = 0; i<width; i++)
	{
		if (i<r && wave[i] < 0)
			wave[i] = -(wave[i]);
		else
		{
			if (i>=r)
				wave[i] = 0;
		}
	}

	cairo_set_source_rgb(cr, 0, 0, 0);

	for (int x = 0; x<width; x++)
	{
		for (int y = 0; y < wave[x]*heightscale; y++)
		{
			cairo_move_to(cr, x, y);
			cairo_close_path(cr);
		}
	}
	cairo_stroke(cr);
}

//============================= End Function =================================//
void on_window_destroy()
{
    gtk_main_quit();
}
