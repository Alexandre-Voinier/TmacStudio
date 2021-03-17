#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

#include <fmod.h>

// Structures

typedef struct SplashStruct
{
    GtkWidget *window;
}SplashStruct;

typedef struct EditScreen
{
    GtkWidget *window;
    GtkButton *rec_btn;
    GtkButton *stop_btn;
    GtkButton *play_btn;
    GtkButton *pause_btn;
    GtkButton *replay_btn;
}EditScreen;

typedef struct MusStruct
{
        FMOD_SYSTEM *system;
        FMOD_SOUND *musique;
	int is_paused;
}MusStruct;


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

    MusStruct mus;
}Ui;

// Main Function

int ui();

// Display Functions

void *on_start_up(void *arg);
void next_step(Ui *appwdgt);

// Init Screen Functions

void init_splash_screen(Ui *appwdgt);
void init_edit_screen(Ui *appwdgt);

// Init MusStruct

void init_musStruct(Ui *appwdgt);

// End functions

void on_window_destroy();

// musique.h
//Fonctions

void Load(Ui *appwdgt, char* musique);

void Play(Ui *appwdgt);

void Pause(Ui *appwdgt);

void RecordStart(Ui *appwdgt);

void RecordStop(Ui *appwdgt);

#endif
