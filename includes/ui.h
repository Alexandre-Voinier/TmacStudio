#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

// Structures

typedef struct SplashStruct
{
    GtkWidget *window;
    GtkWidget *image;
}SplashStruct;

typedef struct EditScreen
{
    GtkWidget *window;
    GtkWidget *rec_btn;
    GtkWidget *stop_btn;
    GtkWidget *play_btn;
    GtkWidget *pause_btn;
}EditScreen;

typedef struct Ui
{
    // Default parameters
    size_t default_width;
    size_t default_height;
    
    // Builder
    GtkBuilder *builder;
    guint loading;
    // Screens
    SplashStruct splash;
    EditScreen edit;
}Ui;

// Main Function

int ui();

// Display Functions

void *on_start_up(void *arg);
void next_step(Ui *appwdgt);

// Init Screen Functions

void init_splash_screen(Ui *appwdgt);
void init_edit_screen(Ui *appwdgt);

// End functions

void on_window_destroy();
#endif
