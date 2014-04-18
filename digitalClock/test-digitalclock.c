#include<gtk/gtk.h>
#include"digitalClock.h"


static int val;



int
main (int argc, char **argv)
{

  gtk_init (&argc, &argv);


  GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);


  GtkWidget*clock0 =
    g_object_new (TYPE_DIGITAL_CLOCK, "show-sec", FALSE, "sensitive", TRUE,
		  "font", "DejaVu Sans", "fsiz", 30, NULL);
  
  GtkWidget*clock1 =
    g_object_new (TYPE_DIGITAL_CLOCK, "show-sec", TRUE, "sensitive", TRUE,
		  "font", "DejaVu Serif", "fsiz", 30, NULL);

  GtkWidget*clock2 =
    g_object_new (TYPE_DIGITAL_CLOCK, "show-sec", FALSE, "sensitive", FALSE,
		  "font", "DejaVu Sans", "fsiz", 25, NULL);
  
  GtkWidget*clock3 =
    g_object_new (TYPE_DIGITAL_CLOCK, "show-msec", TRUE, "sensitive", TRUE,
		  "font", "DejaVu Sans", "fsiz", 30, NULL);
  g_object_set(clock0,"blink",TRUE,NULL);
  g_object_set(clock3,"blink",TRUE,NULL);


  GtkWidget*grid=gtk_grid_new();
  gtk_grid_attach(GTK_GRID(grid),clock0,0,0,5,1);
  gtk_grid_attach_next_to(GTK_GRID(grid),clock1,clock0,GTK_POS_RIGHT,1,1);
  gtk_grid_attach_next_to(GTK_GRID(grid),clock2,clock0,GTK_POS_BOTTOM,1,1);
  gtk_grid_attach_next_to(GTK_GRID(grid),clock3,clock1,GTK_POS_BOTTOM,1,1);
  
  gtk_container_add (GTK_CONTAINER (win), grid);

  g_signal_connect(G_OBJECT(win),"delete-event",G_CALLBACK(gtk_main_quit),NULL);

  gtk_widget_show_all (win);
  gtk_main ();

  return 0;

}
