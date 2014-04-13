#include<gtk/gtk.h>
#include"digitalClock.h"


static int val;




gboolean
tmout_cb (DigitalClock * dclock)
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
//    digital_clock_set_sensitive(dclock,set);

  if(val%3==0)
  digital_clock_show_sec (dclock, set);
  digital_clock_show_msec (dclock, set);

  return TRUE;
}

int
main (int argc, char **argv)
{

  gtk_init (&argc, &argv);


  GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//  gtk_widget_set_size_request (win, 300, 300);


  DigitalClock *clock =
    g_object_new (TYPE_DIGITAL_CLOCK, "show-sec", FALSE, "sensitive", TRUE,
		  "font", "DejaVu Sans", "fsiz", 30, NULL);
  val = 30;

  g_object_set(clock,"blink",TRUE,NULL);


  gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (clock));

  g_timeout_add (3000, (GSourceFunc) tmout_cb, clock);
//    g_idle_add((GSourceFunc)tmout_cb,clock);

  gtk_widget_show_all (win);
  gtk_main ();

  return 0;

}
