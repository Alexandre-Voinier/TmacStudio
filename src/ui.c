#include "../includes/ui.h"
#include "../includes/callbacks.h"

#include <fmod.h>

//=============================== Main Function ==============================// 

int ui()
{
    gtk_init(NULL, NULL);

    Ui *appwdgt = malloc(sizeof(Ui));
    
    appwdgt->builder = gtk_builder_new();
    gtk_builder_add_from_file(appwdgt->builder, "UI/ui.glade", NULL);

    on_start_up(appwdgt);
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "UI/ui.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
	    GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_main();

    // When the program stops we free the structures

    FMOD_Sound_Release(appwdgt->mus.musique);
    FMOD_System_Close(appwdgt->mus.system);
    FMOD_System_Release(appwdgt->mus.system);

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
    init_musStruct(appwdgt);
    gtk_widget_show_all(appwdgt->edit.window);
    gtk_widget_hide(appwdgt->splash.window);
}

//=========================== Init Screen Functions ==========================// 

void init_splash_screen(Ui *appwdgt)
{
    GtkWidget *window;
    GtkWidget *image;

    // First we get objects
    window = GTK_WIDGET(gtk_builder_get_object(appwdgt->builder, 
		"SplashScreen"));
    image = GTK_WIDGET(gtk_builder_get_object(appwdgt->builder,
		"SplashImage"));

    // Then we connect the signals
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // We set the options for each
    gtk_window_set_default_size(GTK_WINDOW(window), 589, 589);
    gtk_window_get_size(GTK_WINDOW(window), 589, 589);
    GdkPixbuf *pic = gdk_pixbuf_new_from_file_at_scale("UI/logo.png",
	    589, 589, 0, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pic);
    // Save the objects in the struct
    appwdgt->splash.window = window;
}

void init_edit_screen(Ui *appwdgt)
{
    // Get the window
    GtkWidget *window;
    window = GTK_WIDGET(gtk_builder_get_object(appwdgt->builder, "EditScreen"));
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 589,
	    589);
    appwdgt->edit.window = window;
    
    // Get the btns
    GtkButton *btn;

    // REC BTN
    btn = GTK_BUTTON(gtk_builder_get_object(appwdgt->builder, "rec_btn"));
    appwdgt->edit.rec_btn = btn;

    // STOP BTN
    btn = GTK_BUTTON(gtk_builder_get_object(appwdgt->builder, "stop_btn"));
    appwdgt->edit.stop_btn = btn;

    // PLAY BTN
    btn = GTK_BUTTON(gtk_builder_get_object(appwdgt->builder, "play_btn"));
    g_signal_connect(btn, "clicked", G_CALLBACK(on_play_btn_clicked), appwdgt);
    appwdgt->edit.play_btn = btn;

    // PAUSE BTN
    btn = GTK_BUTTON(gtk_builder_get_object(appwdgt->builder, "pause_btn"));
    appwdgt->edit.pause_btn = btn;
    g_signal_connect(btn, "clicked", G_CALLBACK(on_pause_btn_clicked), appwdgt);

    // REC BTN
    btn = GTK_BUTTON(gtk_builder_get_object(appwdgt->builder, "rec_btn"));
    g_signal_connect(btn, "clicked", G_CALLBACK(on_rec_btn_clicked), appwdgt);
    appwdgt->edit.rec_btn = btn;

    // STOP BTN
    btn = GTK_BUTTON(gtk_builder_get_object(appwdgt->builder, "stop_btn"));
    g_signal_connect(btn, "clicked", G_CALLBACK(on_stop_btn_clicked), appwdgt);
    appwdgt->edit.stop_btn = btn;

    // Get the menuitem
    GtkMenuItem *menuitm;

    //OPEN MENUITEM
    menuitm = GTK_MENU_ITEM(gtk_builder_get_object(appwdgt->builder,
                "open_menuitm2"));
    g_signal_connect(menuitm, "activate", G_CALLBACK(on_open_btn_activated), appwdgt);

    //NEW MENUITEM
    menuitm = GTK_MENU_ITEM(gtk_builder_get_object(appwdgt->builder,
                "new_menuitm2"));
    g_signal_connect(menuitm, "activate", G_CALLBACK(on_new_btn_activated), appwdgt);

    // QUIT MENUITEM
    menuitm = GTK_MENU_ITEM(gtk_builder_get_object(appwdgt->builder,
		"quit_menuitm2"));
    g_signal_connect(menuitm, "activate", G_CALLBACK(on_window_destroy), NULL);

    menuitm = GTK_MENU_ITEM(gtk_builder_get_object(appwdgt->builder,
                "save_menuitm2"));
    g_signal_connect(menuitm, "activate", G_CALLBACK(on_save_btn_activated), appwdgt);

    //GETTING THE BOXS
    GtkBox *box;

    box = GTK_BOX(gtk_builder_get_object(appwdgt->builder,
                "grilleG"));
    appwdgt->edit.grilleG = box;

    box = GTK_BOX(gtk_builder_get_object(appwdgt->builder,
                "grille"));
    appwdgt->edit.grille = box;
}

void init_musStruct(Ui *appwdgt)
{
	FMOD_System_Create(&(appwdgt->mus.system));
	FMOD_System_Init(appwdgt->mus.system, 2, FMOD_INIT_NORMAL, NULL);
	FMOD_System_GetMasterChannelGroup(appwdgt->mus.system,
		&(appwdgt->mus.master));

	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.play_btn), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.pause_btn), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.rec_btn), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(appwdgt->edit.stop_btn), FALSE);
  	appwdgt->mus.musique = NULL;
	appwdgt->mus.is_recording = 0;
	appwdgt->mus.is_paused = 0;
	appwdgt->mus.isloop = 0;
	appwdgt->mus.fd = NULL;
	appwdgt->mus.datalength = 0;
	appwdgt->mus.soundlength = 0;
	appwdgt->spectre.created = 0;
	appwdgt->mus.play1 = 1;
  appwdgt->mus.ismute = 0;

  // Variables pour la HAUTEUR
  appwdgt->spectre.hasheight = 0;
  appwdgt->mus.coeff = 1;
  appwdgt->mus.height = NULL;

  // Variables pour l'ECHO
  appwdgt->spectre.has_echo = 0;
  appwdgt->mus.has_echo = 0;
  appwdgt->mus.echo = NULL;

  // Variables pour la REVERB
  appwdgt->spectre.has_reverb = 0;
  appwdgt->mus.has_reverb = 0;
  appwdgt->mus.reverb = NULL;
}

