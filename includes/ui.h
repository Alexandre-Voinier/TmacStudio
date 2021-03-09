#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

// Structures

typedef struct SplashStruct
{
    GtkWidget *window;
    GtkWidget *image;
}SplashStruct;

typedef struct HomeScreen
{
    GtkWidget *rec_btn;
}HomeScreen;

typedef struct Ui
{
    SplashStruct splash;
}Ui;

// Main Function

int ui();

// Init Screen Functions

void init_splash_screen(Ui *appwdgt, GtkBuilder *builder);

// End functions

void on_window_destroy();
#endif
