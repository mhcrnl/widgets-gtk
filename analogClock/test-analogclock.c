#include<gtk/gtk.h>
#include"analogClock.h"


static int val;




gboolean
tmout_cb (AnalogClock * dclock)
{

//    g_object_set(dclock,"num",val++,NULL);
/*
    if(val<105)
        return TRUE;
    else
        return FALSE;

*/
val++;
  static gboolean set;

  if (set)
    set = FALSE;
  else
    set = TRUE;

//    digital_clock_set_font_size(dclock,val++);
//    analog_clock_set_sensitive(dclock,set);

  if(val%3==0)
//  analog_clock_show_sec (dclock, set);
    g_object_set(dclock,"show_num",TRUE,NULL);
  else
    g_object_set(dclock,"show_num",FALSE,NULL);


  return TRUE;
}

int
main (int argc, char **argv)
{

  gtk_init (&argc, &argv);


  GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  //gtk_widget_set_size_request (win, 300, 300);


  AnalogClock *clock =
    g_object_new (TYPE_ANALOG_CLOCK,  "sensitive", TRUE,"show_sec",TRUE,
		  NULL);
  val = 30;

  gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (clock));

  g_timeout_add (3000, (GSourceFunc) tmout_cb, clock);
//    g_idle_add((GSourceFunc)tmout_cb,clock);

  gtk_widget_show_all (win);
  gtk_main ();

  return 0;

}
