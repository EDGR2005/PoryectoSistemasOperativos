#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <gtk/gtk.h>

// ----------------------
// Estructuras
// ----------------------
typedef struct {
    GtkStack *stack;
    const gchar **names;
    gint count;
} CarouselData;

// ----------------------
// Prototipos de funciones
// ----------------------

GtkWidget *create_nav_button(GtkStack *stack, const gchar *label, const gchar *page_name);
GtkWidget *create_icon_nav_button(GtkStack *stack, const gchar *icon_name, const gchar *page_name);
GtkWidget *create_menu_button(GtkStack *stack,
                              const gchar *main_label,
                              gint num_options,
                              const gchar *option_labels[],
                              const gchar *option_page_names[]);
GtkWidget *create_content_label(const gchar *text, const gchar *css_class);
GtkWidget *create_cell(const char *ruta, const char *texto);
GtkWidget *create_grid_scrolleable(int filas, int columnas);

// Carrusel
void setup_carrusel(GtkWidget **out_grid,
                    GtkWidget **out_stack_img,
                    CarouselData **out_carousel_data,
                    const gchar *paths[],
                    const gchar *names[],
                    int count,
                    int width,
                    int height);

#endif
