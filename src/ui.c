#include "../includes/ui.h"

//=============================== Main Function ==============================// 

int ui()
{
    gtk_init(NULL, NULL);

    GtkBuilder *builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "UI/ui.glade", NULL);

    Ui *appwdgt = malloc(sizeof(Ui));
    init_splash_screen(appwdgt, builder);
    gtk_widget_show_all(appwdgt->splash.window);

    g_object_unref(builder);

    gtk_main();

    // When the program stops we free the structures
    free(appwdgt);

    return 0;
}


//=========================== Init Screen Functions ==========================// 

void init_splash_screen(Ui *appwdgt, GtkBuilder *builder)
{
    GtkWidget *window;
    window =  GTK_WIDGET(gtk_builder_get_object(builder,
		"SplashScreen"));
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    appwdgt->splash.window = window;
}

//============================== End Functions ===============================//

void on_window_destroy()
{
    gtk_main_quit();
}

