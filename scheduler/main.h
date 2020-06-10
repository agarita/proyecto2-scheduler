#ifndef MAIN_H_
#define MAIN_H_

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>
#include <gmp.h>

static void activate(GtkApplication* app, gpointer user_data);

void arcsin(unsigned int start, unsigned int finish);

#endif
