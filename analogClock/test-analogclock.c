#include<gtk/gtk.h>
#include"analogClock.h"




int
main (int argc, char **argv)
{

  gtk_init (&argc, &argv);


  GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  //gtk_widget_set_size_request (win, 300, 300);


  GtkWidget *clock0 =
    g_object_new (TYPE_ANALOG_CLOCK,  "sensitive", TRUE,"show_sec",TRUE, NULL);

  GtkWidget * clock1 =
      g_object_new (TYPE_ANALOG_CLOCK,"show_sec",FALSE, "sensitive", TRUE,"show_num",TRUE,NULL);

  GtkWidget * clock2 =
      g_object_new (TYPE_ANALOG_CLOCK,"show_sec",TRUE, "sensitive", FALSE, "show_num",TRUE,NULL);

  GtkWidget * clock3 =
      g_object_new (TYPE_ANALOG_CLOCK,"show_sec",TRUE, "sensitive", TRUE,"show_num",TRUE, NULL);

  gtk_widget_set_hexpand(clock0,TRUE);
  gtk_widget_set_vexpand(clock0,TRUE);
  gtk_widget_set_hexpand(clock1,TRUE);
  gtk_widget_set_vexpand(clock1,TRUE);
  gtk_widget_set_hexpand(clock2,TRUE);
  gtk_widget_set_vexpand(clock2,TRUE);
  gtk_widget_set_hexpand(clock3,TRUE);
  gtk_widget_set_vexpand(clock3,TRUE);

  GtkWidget*grid=gtk_grid_new();

  gtk_grid_attach(GTK_GRID(grid),clock0,0,0,1,1);
  gtk_grid_attach(GTK_GRID(grid),clock1,0,1,1,1);
  gtk_grid_attach(GTK_GRID(grid),clock2,1,0,1,1);
  gtk_grid_attach(GTK_GRID(grid),clock3,1,1,1,1);

  gtk_container_add (GTK_CONTAINER (win), grid);


  gtk_widget_show_all (win);
  gtk_main ();

  return 0;

}
