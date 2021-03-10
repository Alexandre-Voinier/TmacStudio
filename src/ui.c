#include "../includes/ui.h"

//=============================== Main Function ==============================// 

int ui()
{
    gtk_init(NULL, NULL);

    Ui *appwdgt = malloc(sizeof(Ui));
    appwdgt->default_width = 800;
    appwdgt->default_height = 800;
    
    appwdgt->builder = gtk_builder_new();
    gtk_builder_add_from_file(appwdgt->builder, "UI/ui.glade", NULL);

    on_start_up(appwdgt);

    gtk_main();


    // When the program stops we free the structures
    free(appwdgt);

    return 0;
}

//============================= Display Functions ============================//

void *on_start_up(void *arg)
{
    Ui *appwdgt = arg;
    init_splash_screen(appwdgt);
    gtk_widget_show_all(appwdgt->splash.window);
    appwdgt->loading = g_timeout_add_seconds(3, G_SOURCE_FUNC(next_step),
	    appwdgt);
}

void next_step(Ui *appwdgt)
{
    g_source_remove(appwdgt->loading);
    init_edit_screen(appwdgt);
    gtk_widget_show_all(appwdgt->edit.window);
    gtk_widget_hide(appwdgt->splash.window);
}

//=========================== Init Screen Functions ==========================// 

void init_splash_screen(Ui *appwdgt)
{
    GtkWidget *window;
    window = GTK_WIDGET(gtk_builder_get_object(appwdgt->builder, 
		"SplashScreen"));
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), appwdgt->default_width,
	    appwdgt->default_height); 
    appwdgt->splash.window = window;
}

void init_edit_screen(Ui *appwdgt)
{
    GtkWidget *window;
    window = GTK_WIDGET(gtk_builder_get_object(appwdgt->builder, "EditScreen"));
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), appwdgt->default_width,
	    appwdgt->default_height);
    appwdgt->edit.window = window;
}

//============================== End Functions ===============================//

void on_window_destroy()
{
    gtk_main_quit();
}

