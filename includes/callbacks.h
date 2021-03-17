#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "ui.h"

//=============================== EDIT SCREEN ================================//

void on_play_btn_clicked(GtkButton *btn, Ui *appwdgt);
void on_pause_btn_clicked(GtkButton *btn, Ui *appwdgt);
void on_open_btn_activated(GtkMenuItem *btn, Ui *appwdgt);
void on_new_btn_activated(GtkMenuItem *btn, Ui *appwdgt);
void on_rec_btn_clicked(GtkButton *btn, Ui *appwdgt);
void on_stop_btn_clicked(GtkButton *btn, Ui *appwdgt);

//================================ End Function ==============================//

void on_window_destroy();

#endif
