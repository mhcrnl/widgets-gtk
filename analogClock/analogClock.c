#include"analogClock.h"
#include<string.h>
#include<math.h>


G_DEFINE_TYPE (AnalogClock, analog_clock, GTK_TYPE_DRAWING_AREA)


static guint TimeOutVal = 200;

static int SecOfMin = (int) 60;
static int SecOfHour = (int) 3600;
static int SecOfDaytime = (int) 3600 * 12;

static struct clockVal
{
  int h;
  int m;
  int s;
  int ts;
} ClockVal;

static gboolean
clockval_is_null (struct clockVal *val)
{

  return (val->h == 0 && val->m == 0 && val->s == 0);

}

//static void analog_clock_realize(GtkWidget*);
//static void analog_clock_unrealize(GtkWidget*);
static void analog_clock_get_preferred_width (GtkWidget * w, gint * min,
					      gint * nat);
static void analog_clock_get_preferred_height_for_width (GtkWidget * w,
							 gint width,
							 gint * min,
							 gint * nat);
static void analog_clock_get_preferred_height (GtkWidget * w,
					       gint * min, gint * nat);
static void analog_clock_get_preferred_width_for_height (GtkWidget * w,
							 gint width,
							 gint * min,
							 gint * nat);
static void analog_clock_size_allocate (GtkWidget * widget,
					GtkAllocation * allocation);



static void analog_clock_init (AnalogClock * obj);

static void analog_clock_class_init (AnalogClockClass * klass);


static void analog_clock_get_property (GObject * obj, guint prop_id,
				       GValue * value, GParamSpec * pspec);
static void analog_clock_set_property (GObject * obj, guint prop_id,
				       const GValue * value,
				       GParamSpec * pspec);



static gboolean
flushtime_cb (AnalogClock * aclock)
{

  gtk_widget_queue_draw (GTK_WIDGET (aclock));
  return TRUE;

}


static gboolean
//_draw (GtkWidget * w, cairo_t * cr, AnalogClock * aclock)
_draw (GtkWidget * w, cairo_t * cr)
{
    
    AnalogClock * aclock=ANALOG_CLOCK(w);
  struct timeval tv;
  if (gettimeofday (&tv, NULL) == -1)
    {
      g_critical ("gettimeofday() failed!");
    }

  time_t now = tv.tv_sec;
  struct tm *now_tm = localtime (&now);


  double width, height, fontsiz;
  width = (double) aclock->priv->width;
  height = (double) aclock->priv->height;
  AnalogClockPriv*priv=aclock->priv;

//  double edge=width<=height?width:height;

  double side;
  if (width < height)
    {
      cairo_translate (cr, 0, (height - width) / 2);
      side = width;
    }
  else
    {
      cairo_translate (cr, (width - height) / 2, 0);
      side = height;
    }
  cairo_scale (cr, side, side);
  cairo_translate (cr, 0.5, 0.5);

  cairo_set_source_rgb (cr, 0.95, 0.95, 0.95);
  cairo_paint (cr);


  if (aclock->priv->sensitive || clockval_is_null (&ClockVal))
    {
      ClockVal.h = now_tm->tm_hour;
      ClockVal.m = now_tm->tm_min;
      ClockVal.s = now_tm->tm_sec;
      ClockVal.ts = ClockVal.h * 3600 + ClockVal.m * 60 + ClockVal.s;
    }

  printf ("Time:%d:[%d:%d:%d].%03u %06u\n", ClockVal.ts, ClockVal.h,
	  ClockVal.m, ClockVal.s, (int) (tv.tv_usec / 1000.0 + 0.5),
	  tv.tv_usec);

  double x = 0;			// width / 2 - extents.width / 2 - extents.x_bearing;
  double y = 0;			// height / 2 - extents.height / 2 - extents.y_bearing;

  cairo_set_source_rgb (cr, 0.1, 0.1, 0.1);


  double ux = 1., uy = 1.;

  cairo_device_to_user_distance (cr, &ux, &uy);
  double px = ux;

  double radius = 0.4;
  double scaleDash[2] = { 0.06, 1.1 };
  double pointerDash[2] = { 0.35, 1.1 };

  cairo_set_line_width (cr, 3 * px);
  cairo_arc (cr, x, y, radius, 0, M_PI * 2);

  cairo_stroke (cr);

  double hscale, qscale;	//length of scale

  int i, k = 60;
  double pangle = 2 * M_PI / k;

  cairo_select_font_face(cr,"Serif" ,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr,0.07);
  gchar* num[12]={"3","4","5","6","7","8","9","10","11","12","1","2"};
  int t=0;

  cairo_text_extents_t te;
  

  for (i = 0; i < k; i++)
    {
      double sx, sy;

      sx = cos (i * pangle) * radius;	// + 0.5;
      sy = sin (i * pangle) * radius;	// + 0.5;

      cairo_move_to (cr, sx, sy);
      cairo_line_to (cr, x, y);
      if (i % 5 == 0)
	{
        if(priv->show_num){
        cairo_text_extents(cr,num[t],&te);
        cairo_move_to(cr,(sx-te.width/2-te.x_bearing)*3/4,(sy-te.height/2-te.y_bearing)*3/4);
        cairo_show_text(cr,num[t]);
        t++;
        }
	  cairo_set_line_width (cr, 2 * px);
	  cairo_set_dash (cr, scaleDash, 2, 0);
	}
      else
	{
	  cairo_set_line_width (cr, px);
	  cairo_set_dash (cr, scaleDash, 2, 0.02);
	}

      cairo_stroke (cr);

    }

/*draw pointers...below*/
  double ptx, pty;
  gboolean isDaytime;
  if (ClockVal.ts / SecOfDaytime != 0)
    isDaytime = FALSE;
  else
    isDaytime = TRUE;
  int ts = ClockVal.ts % SecOfDaytime;

  double hpointer = ((double) ts / SecOfDaytime) * 2 * M_PI - M_PI / 2;
//  printf("min:%g\n",spointer);
  ptx = cos (hpointer) * radius;	// + 0.5;
  pty = sin (hpointer) * radius;	// + 0.5;
  cairo_move_to (cr, x, y);
  cairo_line_to (cr, ptx, pty);
  cairo_set_dash (cr, pointerDash, 2, 0.2);
  cairo_set_line_width (cr, 0.01);
  cairo_set_source_rgb (cr, 0.6, 0.0, 0.0);
  // cairo_rotate(cr,-M_PI/2);
  cairo_stroke (cr);

  cairo_arc(cr,x,y,0.013,0,3*M_PI);
  cairo_fill(cr);


  double mpointer = ((double) ts / SecOfHour) * 2 * M_PI - M_PI / 2;
  ptx = cos (mpointer) * radius;	// + 0.5;
  pty = sin (mpointer) * radius;	// + 0.5;
  cairo_move_to (cr, x, y);
  cairo_line_to (cr, ptx, pty);
  cairo_set_dash (cr, pointerDash, 2, 0.13);
  cairo_set_line_width (cr, 0.005);
  cairo_set_source_rgb (cr, 0.1, 0.2, 0.0);
  // cairo_rotate(cr,-M_PI/2);
  cairo_stroke (cr);

  cairo_arc(cr,x,y,0.007,0,3*M_PI);
  cairo_fill(cr);



  double spointer = (double) (ts % SecOfMin) / 60 * 2 * M_PI - M_PI / 2;
  ptx = cos (spointer) * radius;	// + 0.5;
  pty = sin (spointer) * radius;	// + 0.5;
  if(aclock->priv->show_sec){
  cairo_move_to (cr, x, y);
  cairo_line_to (cr, ptx, pty);
  cairo_set_dash (cr, pointerDash, 2, 0.06);
  cairo_set_line_width (cr, 1 * px);
  cairo_set_source_rgb (cr, 0.1, 0.1, 0.3);
  cairo_stroke (cr);
//tail of sec-pointer
  cairo_move_to (cr, x, y);
  cairo_line_to (cr, -ptx, -pty);
  cairo_set_dash (cr, pointerDash, 2, 0.30);
  cairo_stroke (cr);
  cairo_arc (cr, x, y, 0.007, 0, M_PI * 2);
  cairo_fill (cr);
  }
//seconds point.
  cairo_set_source_rgb (cr, 0.1, 0.1, 0.1);
  cairo_arc (cr, ptx, pty, 3 * px, 0, M_PI * 2);
  cairo_fill (cr);



  return TRUE;
}






static void
analog_clock_init (AnalogClock * aclock)
{

  AnalogClockPriv *priv;

  aclock->priv = ANALOG_CLOCK_GET_PRIV (aclock);
  priv = aclock->priv;
  priv->x = priv->y = 0;
  priv->width = priv->height = -1;


//  g_signal_connect (G_OBJECT (aclock), "draw", G_CALLBACK (_draw), aclock);
  aclock->priv->timeoutid =
    g_timeout_add (TimeOutVal, (GSourceFunc) flushtime_cb, aclock);

//  gtk_widget_register_window(GTK_WIDGET(aclock),gtk_widget_get_window(GTK_WIDGET(aclock)));
    gtk_widget_add_events(GTK_WIDGET(aclock),GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK);

}






static void
analog_clock_get_preferred_width (GtkWidget * w, gint * min, gint * nat)
{
  g_return_if_fail (w != NULL);
  g_return_if_fail (IS_ANALOG_CLOCK (w));
  if (min)
    *min = 150;
  if (nat)
    *nat = 150;

}



static void
analog_clock_get_preferred_height (GtkWidget * w, gint * min, gint * nat)
{
  g_return_if_fail (w != NULL);
  g_return_if_fail (IS_ANALOG_CLOCK (w));
  if (min)
    *min = 150;
  if (nat)
    *nat = 150;

}






static void
analog_clock_get_preferred_height_for_width (GtkWidget * w, gint width,
					     gint * min, gint * nat)
{
  g_return_if_fail (w != NULL);
  g_return_if_fail (IS_ANALOG_CLOCK (w));
  if (min)
    *min = width;
  if (nat)
    *nat = width;

}


static void
analog_clock_get_preferred_width_for_height (GtkWidget * w, gint height,
					     gint * min, gint * nat)
{
  g_return_if_fail (w != NULL);
  g_return_if_fail (IS_ANALOG_CLOCK (w));
  if (min)
    *min = height;
  if (nat)
    *nat = height;

}




static void
analog_clock_size_allocate (GtkWidget * w, GtkAllocation * allocation)
{

  g_return_if_fail (w != NULL || allocation != NULL);
  g_return_if_fail (IS_ANALOG_CLOCK (w));

  AnalogClockPriv *priv = ANALOG_CLOCK_GET_PRIV (w);
  gtk_widget_set_allocation (w, allocation);

  priv->width = allocation->width;
  priv->height = allocation->height;

  priv->x = allocation->x;
  priv->y = allocation->y;


  /*
     GtkWidgetClass*pclass=g_type_class_peek_parent(ANALOG_CLOCK_GET_CLASS(w));
     pclass->size_allocate(w,allocation);
   */
  GTK_WIDGET_CLASS (analog_clock_parent_class)->size_allocate (w, allocation);



}




static void
analog_clock_get_property (GObject * obj, guint prop_id, GValue * value,
			   GParamSpec * pspec)
{

  AnalogClock *aclock = ANALOG_CLOCK (obj);

  AnalogClockPriv *priv = aclock->priv;

  switch (prop_id)
    {
    case PROP_SHOWSEC:
      g_value_set_boolean (value, priv->show_sec);
      break;
    case PROP_SHOWNUM:
      g_value_set_boolean (value, priv->show_num);
      break;
    case PROP_SENSITIVE:
      g_value_set_boolean (value, priv->sensitive);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }

}




static void
analog_clock_set_property (GObject * obj, guint prop_id,
			   const GValue * value, GParamSpec * pspec)
{

  AnalogClock *aclock = ANALOG_CLOCK (obj);

  AnalogClockPriv *priv = aclock->priv;

  switch (prop_id)
    {
    case PROP_SHOWSEC:
      priv->show_sec = g_value_get_boolean (value);
      break;
    case PROP_SHOWNUM:
      priv->show_num= g_value_get_boolean (value);
      break;
    case PROP_SENSITIVE:
      priv->sensitive = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
  gtk_widget_queue_draw (GTK_WIDGET (obj));

}



/*
static gint
analog_clock_enter_notify(GtkWidget*w,GdkEventCrossing*event)
{

    AnalogClock*aclock=ANALOG_CLOCK(w);
    g_object_set(w,"show_sec",TRUE,NULL);
    g_message("enter clock...");
    return FALSE;


}


static gint
analog_clock_leave_notify(GtkWidget*w,GdkEventCrossing*event)
{

    AnalogClock*aclock=ANALOG_CLOCK(w);
    g_object_set(w,"show_sec",FALSE,NULL);
    g_message("leave clock...");
    return FALSE;


}


*/


static void
analog_clock_class_init (AnalogClockClass * klass)
{

  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->get_preferred_width = analog_clock_get_preferred_width;
  widget_class->get_preferred_height = analog_clock_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    analog_clock_get_preferred_height_for_width;
  widget_class->get_preferred_width_for_height =
    analog_clock_get_preferred_width_for_height;

  widget_class->size_allocate = analog_clock_size_allocate;

  widget_class->draw=_draw;
//  widget_class->enter_notify_event=analog_clock_enter_notify;
//  widget_class->leave_notify_event=analog_clock_leave_notify;

  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->set_property = analog_clock_set_property;
  gobject_class->get_property = analog_clock_get_property;



  g_object_class_install_property (gobject_class, PROP_SHOWNUM,
				   g_param_spec_boolean ("show-num",
							 "show number field",
							 "Dispaly time with number",
							 FALSE,
							 G_PARAM_READWRITE));



  g_object_class_install_property (gobject_class, PROP_SHOWSEC,
				   g_param_spec_boolean ("show-sec",
							 "show second field",
							 "Dispaly time with second field",
							 FALSE,
							 G_PARAM_READWRITE));


  g_object_class_install_property (gobject_class, PROP_SENSITIVE,
				   g_param_spec_boolean ("sensitive",
							 "sensitivity of flush",
							 "Whether flush time",
							 TRUE,
							 G_PARAM_READWRITE));

  g_type_class_add_private (gobject_class, sizeof (AnalogClockPriv));

}

void
analog_clock_flush (AnalogClock * aclock)
{
  gtk_widget_queue_draw (GTK_WIDGET (aclock));


}

void
analog_clock_show_sec (AnalogClock * aclock, gboolean set)
{

  g_object_set (aclock, "show-sec", set, NULL);

}

void analog_clock_show_num (AnalogClock *aclock, gboolean set)
{
  g_object_set (aclock, "show-num", set, NULL);


}


void
analog_clock_set_sensitive (AnalogClock * aclock, gboolean sensitive)
{
  g_object_set (aclock, "sensitive", sensitive, NULL);

}


AnalogClock *
analog_clock_new ()
{
  return ANALOG_CLOCK (g_object_new (TYPE_ANALOG_CLOCK, NULL));

}
