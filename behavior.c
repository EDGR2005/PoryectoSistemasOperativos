#include "behavior.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

// ===============================================================
// CALLBACKS
// ===============================================================

static void on_nav_item_clicked(GtkButton *button, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    const gchar *page_name = gtk_widget_get_name(GTK_WIDGET(button));
    gtk_stack_set_visible_child_name(stack, page_name);

    // cerrar popover
    GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(button));
    while (parent && !GTK_IS_POPOVER(parent))
        parent = gtk_widget_get_parent(parent);
    if (GTK_IS_POPOVER(parent))
        gtk_popover_popdown(GTK_POPOVER(parent));
}

static void on_carrusel_button_clicked(GtkButton *button, gpointer user_data) {
    CarouselData *data = user_data;

    const gchar *current_name =
        gtk_stack_get_visible_child_name(data->stack);

    int current = -1;
    for (int i = 0; i < data->count; i++)
        if (g_strcmp0(current_name, data->names[i]) == 0)
            current = i;

    int next = current;
    if (g_strcmp0(gtk_button_get_label(button), ">") == 0)
        next = (current + 1) % data->count;
    else
        next = (current - 1 + data->count) % data->count;

    gtk_stack_set_visible_child_name(data->stack, data->names[next]);
}

// ===============================================================
// CONSTRUCCIÓN DE WIDGETS
// ===============================================================

GtkWidget *create_nav_button(GtkStack *stack,
                             const gchar *label,
                             const gchar *page_name)
{
    GtkWidget *btn = gtk_button_new_with_label(label);
    gtk_widget_set_name(btn, page_name);
    gtk_style_context_add_class(gtk_widget_get_style_context(btn), "nav-button");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_item_clicked), stack);
    return btn;
}

GtkWidget *create_icon_nav_button(GtkStack *stack,
                                  const gchar *icon_name,
                                  const gchar *page_name)
{
    GtkWidget *btn = gtk_button_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
    gtk_widget_set_name(btn, page_name);
    gtk_style_context_add_class(gtk_widget_get_style_context(btn), "nav-button");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_item_clicked), stack);

    gtk_widget_set_margin_start(btn, 5);
    gtk_widget_set_margin_end(btn, 5);

    return btn;
}

GtkWidget *create_menu_button(GtkStack *stack,
                              const gchar *main_label,
                              gint num_options,
                              const gchar *option_labels[],
                              const gchar *option_page_names[])
{
    GtkWidget *menu = gtk_menu_button_new();
    gtk_button_set_label(GTK_BUTTON(menu), main_label);
    gtk_style_context_add_class(gtk_widget_get_style_context(menu), "menu-nav-button");

    GtkWidget *popover = gtk_popover_new(menu);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    for (int i = 0; i < num_options; i++) {
        GtkWidget *btn = gtk_button_new_with_label(option_labels[i]);
        gtk_widget_set_name(btn, option_page_names[i]);
        gtk_style_context_add_class(gtk_widget_get_style_context(btn), "menu-item");
        g_signal_connect(btn, "clicked", G_CALLBACK(on_nav_item_clicked), stack);
        gtk_box_pack_start(GTK_BOX(box), btn, TRUE, TRUE, 0);
    }

    gtk_container_add(GTK_CONTAINER(popover), box);
    gtk_widget_show_all(box);

    gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu), popover);
    return menu;
}

GtkWidget *create_content_label(const gchar *text, const gchar *css_class) {
    GtkWidget *lbl = gtk_label_new(text);
    GtkStyleContext *ctx = gtk_widget_get_style_context(lbl);

    if (css_class && *css_class)
        gtk_style_context_add_class(ctx, css_class);

    gtk_widget_set_halign(lbl, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(lbl, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(lbl, TRUE);
    return lbl;
}

GtkWidget *create_cell(const char *ruta, const char *texto) {
    GtkWidget *event = gtk_event_box_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(event), box);

    GdkPixbuf *pix = gdk_pixbuf_new_from_file_at_scale(ruta, 120, 120, TRUE, NULL);
    GtkWidget *img = gtk_image_new_from_pixbuf(pix);
    gtk_box_pack_start(GTK_BOX(box), img, FALSE, FALSE, 0);

    GtkWidget *lbl = gtk_label_new(texto);
    gtk_box_pack_start(GTK_BOX(box), lbl, FALSE, FALSE, 0);

    gtk_widget_set_name(event, "cell");
    return event;
}

GtkWidget *create_grid_scrolleable(int filas, int columnas)
{
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *align = gtk_alignment_new(0.5, 0, 0, 0);
    GtkWidget *grid = gtk_grid_new();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    for (int r = 0; r < filas; r++)
        for (int c = 0; c < columnas; c++)
            gtk_grid_attach(GTK_GRID(grid),
                            create_cell("/home/img.jpg", "Producto X"),
                            c, r, 1, 1);

    gtk_container_add(GTK_CONTAINER(align), grid);
    gtk_container_add(GTK_CONTAINER(scroll), align);
    return scroll;
}

// ===============================================================
// CARRUSEL COMPLETO
// ===============================================================

void setup_carrusel(GtkWidget **out_grid,
                    GtkWidget **out_stack_img,
                    CarouselData **out_data,
                    const gchar *paths[],
                    const gchar *names[],
                    int count,
                    int width,
                    int height)
{
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *stack = gtk_stack_new();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *btn_prev = gtk_button_new_with_label("<");
    GtkWidget *btn_next = gtk_button_new_with_label(">");

    gtk_widget_set_name(btn_prev, "carrusel_control");
    gtk_widget_set_name(btn_next, "carrusel_control");

    // Añadir imágenes al stack
    for (int i = 0; i < count; i++) {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *img = gtk_image_new();

        GdkPixbuf *orig = gdk_pixbuf_new_from_file(paths[i], NULL);
        GdkPixbuf *scaled =
            gdk_pixbuf_scale_simple(orig, width, height, GDK_INTERP_BILINEAR);

        gtk_image_set_from_pixbuf(GTK_IMAGE(img), scaled);

        gtk_container_add(GTK_CONTAINER(box), img);
        gtk_stack_add_named(GTK_STACK(stack), box, names[i]);

        g_object_unref(orig);
        g_object_unref(scaled);
    }

    CarouselData *data = g_new(CarouselData, 1);
    data->stack = GTK_STACK(stack);
    data->names = names;
    data->count = count;

    g_signal_connect(btn_prev, "clicked",
                     G_CALLBACK(on_carrusel_button_clicked), data);
    g_signal_connect(btn_next, "clicked",
                     G_CALLBACK(on_carrusel_button_clicked), data);

    gtk_grid_attach(GTK_GRID(grid), btn_prev, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stack,    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_next, 2, 0, 1, 1);

    *out_grid = grid;
    *out_stack_img = stack;
    *out_data = data;
}
