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
    GtkBox *grilleG;
    GtkBox *grille;
    GtkWidget *TextS;
    GtkWidget *drawW;
}EditScreen;

typedef struct Spectre
{
    GtkWidget *visuSpectre;
    GdkRectangle rects[512];
    int created;
    guint timeout;
    int hasheight;
}Spectre;

typedef struct MusStruct
{
        FMOD_SYSTEM *system;
        FMOD_SOUND *musique;

	FMOD_CHANNEL *channel;
	FMOD_CHANNELGROUP *master;
	FMOD_DSP *dspFFT;
	FMOD_DSP_PARAMETER_FFT *paramFFT;
	int is_paused;
	int is_recording;
	int isloop;

	FMOD_CREATESOUNDEXINFO exinfo;
	unsigned int datalength;
	unsigned int soundlength;
	FILE* fd;
	guint save;

	guint check;

	FMOD_DSP *height;
	float coeff;
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
    Spectre spectre;

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

void Load(Ui *appwdgt, char* musique, int s);

void Play(Ui *appwdgt);

void Pause(Ui *appwdgt);

void Save(Ui *appwdgt);

void RecordStart(Ui *appwdgt);

void RecordStop(Ui *appwdgt);

void on_entry_activated(GtkWidget *entry, Ui *appwdgt);

void Attach(Ui *appwdgt);

void Volume(GtkWidget *slider, Ui *appwdgt);

void Mute(Ui* appwdgt);

void Loop(Ui *appwdgt, int booleen);

void Height(Ui *appwdgt, float coef);

void WriteWavHeader(FILE *fp, Ui *appwdgt, int length);

void get_spectre(Ui *appwdgt);

void clean_spectre(Ui *appwdgt);

#endif
