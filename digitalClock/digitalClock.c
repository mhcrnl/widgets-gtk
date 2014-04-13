#include"digitalClock.h"
#include<string.h>


G_DEFINE_TYPE(DigitalClock, digital_clock, GTK_TYPE_DRAWING_AREA)


static char TimeStr[13];
static guint TimeOutVal = 200;


//static void digital_clock_realize(GtkWidget*);
//static void digital_clock_unrealize(GtkWidget*);
static void digital_clock_get_preferred_width(GtkWidget * w, gint * min,
					      gint * nat);
static void digital_clock_get_preferred_height_for_width(GtkWidget * w,
							 gint width,
							 gint * min,
							 gint * nat);
static void digital_clock_get_preferred_height(GtkWidget * w,
					       gint * min, gint * nat);
static void digital_clock_get_preferred_width_for_height(GtkWidget * w,
							 gint width,
							 gint * min,
							 gint * nat);
static void digital_clock_size_allocate(GtkWidget * widget,
					GtkAllocation * allocation);



static void digital_clock_init(DigitalClock * obj);

static void digital_clock_class_init(DigitalClockClass * klass);


static void digital_clock_get_property(GObject * obj, guint prop_id,
				       GValue * value, GParamSpec * pspec);
static void digital_clock_set_property(GObject * obj, guint prop_id,
				       const GValue * value,
				       GParamSpec * pspec);



static gboolean flushtime_cb(DigitalClock * dclock)
{

    DigitalClockPriv *priv = dclock->priv;

    if (priv->show_msec) {

	priv->width = priv->fsiz * 8;
    } else if (priv->show_sec) {
	priv->width = priv->fsiz * 5;

    } else {
	priv->width = priv->fsiz * 3;

    }
/*
  GtkAllocation alloc;
  gtk_widget_get_allocation(GTK_WIDGET(dclock),&alloc);
  alloc.width=priv->width;
  gtk_widget_set_allocation(GTK_WIDGET(dclock),&alloc);
*/

    //gtk_widget_queue_draw (GTK_WIDGET (dclock));
    gtk_widget_queue_resize(GTK_WIDGET(dclock));
    return TRUE;

}


static gboolean digital_clock_draw(GtkWidget * w, cairo_t * cr)
{
    DigitalClock *dclock = DIGITAL_CLOCK(w);

    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
	g_critical("gettimeofday() failed!");
    }

    time_t now = tv.tv_sec;
//  time_t now = time (NULL);
    struct tm *now_tm = localtime(&now);


    double width, height, fontsiz;
    width = (double) dclock->priv->width;
    height = (double) dclock->priv->height;
    fontsiz = (double) dclock->priv->fsiz;

    static gboolean draw_colon;
    static int now_sec = -1;
    static int last_msec=-1;


//    cairo_scale(cr,width,height);
//  cairo_set_source_rgb (cr, 1, 1, 1);
//  cairo_paint (cr);

    gtk_render_background(gtk_widget_get_style_context(w), cr, 0, 0,
			  dclock->priv->width, dclock->priv->height);

    if (dclock->priv->sensitive || 0 == strlen(TimeStr)) {

        if(!dclock->priv->blink||last_msec<=500||now_sec!=now_tm->tm_sec)
            draw_colon=TRUE;
        else
            draw_colon=FALSE;

	if (dclock->priv->show_sec) {
	    if (draw_colon)
		sprintf(TimeStr, "%02d:%02d:%02d", now_tm->tm_hour,
			now_tm->tm_min, now_tm->tm_sec);
	    else
		sprintf(TimeStr, "%02d %02d %02d", now_tm->tm_hour,
			now_tm->tm_min, now_tm->tm_sec);

//    timestr=g_strdup_printf("%02d:%02d:%02d",now_tm->tm_hour,now_tm->tm_min,now_tm->tm_sec);
	    if (dclock->priv->show_msec) {
		sprintf(TimeStr + 8, ".%d",
			(int) (tv.tv_usec / 1000.0 + 0.5));
	    }
	} else {
	    if (draw_colon)
		sprintf(TimeStr, "%02d:%02d", now_tm->tm_hour,
			now_tm->tm_min);
	    else
		sprintf(TimeStr, "%02d %02d", now_tm->tm_hour,
			now_tm->tm_min);
	}
//    timestr=g_strdup_printf("%02d:%02d",now_tm->tm_hour,now_tm->tm_min);
	now_sec = now_tm->tm_sec;
    }

	last_msec=(int) (tv.tv_usec / 1000.0 + 0.5);
    printf("Time::[%s].%03u %06u\n", TimeStr,last_msec,tv.tv_usec);

    cairo_select_font_face(cr, dclock->priv->font, CAIRO_FONT_SLANT_NORMAL,
			   CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, fontsiz);
//    cairo_set_line_width(cr,1./300);

    cairo_text_extents_t extents;
    if (dclock->priv->show_sec) {
	cairo_text_extents(cr, "00:00:00", &extents);
	if (dclock->priv->show_msec)
	    cairo_text_extents(cr, "00:00:00.000", &extents);
    } else
	cairo_text_extents(cr, "00:00", &extents);


    double x = width / 2 - extents.width / 2 - extents.x_bearing;
    double y = height / 2 - extents.height / 2 - extents.y_bearing;

    cairo_set_source_rgb(cr, 0, 0, 0);

    cairo_move_to(cr, x, y);
    cairo_show_text(cr, TimeStr);
//    cairo_stroke(cr);
//    g_free(timestr);

    return TRUE;
}






static void digital_clock_init(DigitalClock * dclock)
{

    DigitalClockPriv *priv;
    GtkWidget *w = GTK_WIDGET(dclock);

    dclock->priv = DIGITAL_CLOCK_GET_PRIV(dclock);
    priv = dclock->priv;
    priv->x = priv->y = 0;
    priv->width = priv->height = -1;
    priv->fsiz = 0;
    priv->mode12 = TRUE;


//  g_signal_connect (G_OBJECT (dclock), "draw", G_CALLBACK (_draw), dclock);
    dclock->priv->timeoutid =
	g_timeout_add(TimeOutVal, (GSourceFunc) flushtime_cb, dclock);

    GtkStyleContext *stylecontext = gtk_widget_get_style_context(w);
    gtk_style_context_add_class(stylecontext, GTK_STYLE_CLASS_BUTTON);
    gtk_style_context_add_class(stylecontext, "suggested-action");

}






static void
digital_clock_get_preferred_width(GtkWidget * w, gint * min, gint * nat)
{
    g_return_if_fail(w != NULL);
    g_return_if_fail(IS_DIGITAL_CLOCK(w));

    DigitalClock *dclock = DIGITAL_CLOCK(w);
    DigitalClockPriv *priv = dclock->priv;

    if (priv->show_msec) {

	if (min)
	    *min = priv->fsiz * 7;
	if (nat)
	    *nat = priv->fsiz * 7;
    } else if (priv->show_sec) {
	if (min)
	    *min = priv->fsiz * 5;
	if (nat)
	    *nat = priv->fsiz * 5;



    } else {
	if (min)
	    *min = priv->fsiz * 3;
	if (nat)
	    *nat = priv->fsiz * 3;



    }


}



static void
digital_clock_get_preferred_height(GtkWidget * w, gint * min, gint * nat)
{
    g_return_if_fail(w != NULL);
    g_return_if_fail(IS_DIGITAL_CLOCK(w));
    DigitalClock *dclock = DIGITAL_CLOCK(w);
    DigitalClockPriv *priv = dclock->priv;

    if (min)
	*min = priv->fsiz;
    if (nat)
	*nat = priv->fsiz;

}






static void
digital_clock_get_preferred_height_for_width(GtkWidget * w, gint width,
					     gint * min, gint * nat)
{
    g_return_if_fail(w != NULL);
    g_return_if_fail(IS_DIGITAL_CLOCK(w));
    if (min)
	*min = width / 2;
    if (nat)
	*nat = width / 2;

}


static void
digital_clock_get_preferred_width_for_height(GtkWidget * w, gint height,
					     gint * min, gint * nat)
{
    g_return_if_fail(w != NULL);
    g_return_if_fail(IS_DIGITAL_CLOCK(w));
    if (min)
	*min = height * 2;
    if (nat)
	*nat = height * 2;

}





static void
digital_clock_size_allocate(GtkWidget * w, GtkAllocation * allocation)
{

    g_return_if_fail(w != NULL || allocation != NULL);
    g_return_if_fail(IS_DIGITAL_CLOCK(w));

    DigitalClockPriv *priv = DIGITAL_CLOCK_GET_PRIV(w);
    gtk_widget_set_allocation(w, allocation);

    priv->width = allocation->width;
    priv->height = allocation->height;

    priv->x = allocation->x;
    priv->y = allocation->y;


    /*
       GtkWidgetClass*pclass=g_type_class_peek_parent(DIGITAL_CLOCK_GET_CLASS(w));
       pclass->size_allocate(w,allocation);
     */
    GTK_WIDGET_CLASS(digital_clock_parent_class)->size_allocate(w,
								allocation);

    GdkWindow *window = gtk_widget_get_window(w);
    if (gtk_widget_get_realized(w)) {


	if (priv->show_msec) {
	    priv->width = priv->fsiz * 7;
	} else if (priv->show_sec) {
	    priv->width = priv->fsiz * 5;
	} else {
	    priv->width = priv->fsiz * 3;
	}
	priv->height = priv->fsiz;

	int x, y;
	x = (allocation->width - priv->width) >> 1;
	y = (allocation->height - priv->height) >> 1;

	gdk_window_move_resize(window, x, y, priv->width, priv->height);

    }



}




static void
digital_clock_get_property(GObject * obj, guint prop_id, GValue * value,
			   GParamSpec * pspec)
{

    DigitalClock *dclock = DIGITAL_CLOCK(obj);

    DigitalClockPriv *priv = dclock->priv;

    switch (prop_id) {
    case PROP_FSIZ:
	g_value_set_int(value, priv->fsiz);
	break;
    case PROP_FONT:
	g_value_set_string(value, priv->font);
	break;
    case PROP_MODE12:
	g_value_set_boolean(value, priv->fsiz);
	break;
    case PROP_SHOWSEC:
	g_value_set_boolean(value, priv->show_sec);
	break;
    case PROP_SHOWMSEC:
	g_value_set_boolean(value, priv->show_msec);
	break;
    case PROP_BLINK:
	g_value_set_boolean(value, priv->blink);
	break;
    case PROP_SENSITIVE:
	g_value_set_boolean(value, priv->sensitive);
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	break;
    }

}




static void
digital_clock_set_property(GObject * obj, guint prop_id,
			   const GValue * value, GParamSpec * pspec)
{

    DigitalClock *dclock = DIGITAL_CLOCK(obj);

    DigitalClockPriv *priv = dclock->priv;

    switch (prop_id) {
    case PROP_FSIZ:
	priv->fsiz = g_value_get_int(value);
	break;
    case PROP_FONT:
	priv->font = g_value_dup_string(value);
	break;
    case PROP_MODE12:
	priv->mode12 = g_value_get_boolean(value);
	break;
    case PROP_SHOWSEC:
	priv->show_sec = g_value_get_boolean(value);
	break;
    case PROP_BLINK:
	priv->blink = g_value_get_boolean(value);
	break;
    case PROP_SHOWMSEC:
	priv->show_msec = g_value_get_boolean(value);
	if (priv->show_msec) {
	    priv->show_sec = TRUE;
	    g_source_remove(dclock->priv->timeoutid);
	    TimeOutVal = 1;
	    dclock->priv->timeoutid =
		g_timeout_add(TimeOutVal, (GSourceFunc) flushtime_cb,
			      dclock);
	} else {
	    g_source_remove(dclock->priv->timeoutid);
	    TimeOutVal = 200;
	    dclock->priv->timeoutid =
		g_timeout_add(TimeOutVal, (GSourceFunc) flushtime_cb,
			      dclock);
	}
	break;
    case PROP_SENSITIVE:
	priv->sensitive = g_value_get_boolean(value);
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	break;
    }
    gtk_widget_queue_draw(GTK_WIDGET(obj));

}




static void digital_clock_class_init(DigitalClockClass * klass)
{

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    widget_class->get_preferred_width = digital_clock_get_preferred_width;
    widget_class->get_preferred_height =
	digital_clock_get_preferred_height;
    widget_class->get_preferred_height_for_width =
	digital_clock_get_preferred_height_for_width;

    widget_class->size_allocate = digital_clock_size_allocate;
    widget_class->draw = digital_clock_draw;

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->set_property = digital_clock_set_property;
    gobject_class->get_property = digital_clock_get_property;


    g_object_class_install_property(gobject_class, PROP_FONT,
				    g_param_spec_string("font",
							"font name",
							"Font Name",
							"Sans",
							G_PARAM_READWRITE));


    g_object_class_install_property(gobject_class, PROP_FSIZ,
				    g_param_spec_int("fsiz", "font-size",
						     "Size of Font", 0,
						     100, 22,
						     G_PARAM_READWRITE));


    g_object_class_install_property(gobject_class, PROP_MODE12,
				    g_param_spec_boolean("mode12",
							 "time-display-mode12",
							 "Dispaly time by 12-hour system",
							 FALSE,
							 G_PARAM_READWRITE));


    g_object_class_install_property(gobject_class, PROP_SHOWSEC,
				    g_param_spec_boolean("show-sec",
							 "show second field",
							 "Dispaly time with second field",
							 FALSE,
							 G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_SHOWMSEC,
				    g_param_spec_boolean("show-msec",
							 "show milli-second field",
							 "Dispaly time with milli-second field",
							 FALSE,
							 G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_BLINK,
				    g_param_spec_boolean("blink",
							 "Blink",
							 "blink colon by second",
							 FALSE,
							 G_PARAM_READWRITE));



    g_object_class_install_property(gobject_class, PROP_SENSITIVE,
				    g_param_spec_boolean("sensitive",
							 "sensitivity of flush",
							 "Whether flush time",
							 TRUE,
							 G_PARAM_READWRITE));

    g_type_class_add_private(gobject_class, sizeof(DigitalClockPriv));

}

void digital_clock_flush(DigitalClock * dclock)
{
    gtk_widget_queue_draw(GTK_WIDGET(dclock));


}

void digital_clock_show_sec(DigitalClock * dclock, gboolean set)
{

    g_object_set(dclock, "show-sec", set, NULL);

}

void digital_clock_show_msec(DigitalClock * dclock, gboolean set)
{

    g_object_set(dclock, "show-msec", set, NULL);

}


void digital_clock_set_blink(DigitalClock * dclock, gboolean set)
{

    g_object_set(dclock, "blink", set, NULL);

}



void digital_clock_set_font(DigitalClock * dclock, gchar * desc)
{
    if (desc) {
	g_object_set(dclock, "font", desc, NULL);
    }

}

void digital_clock_set_font_size(DigitalClock * dclock, gint size)
{

    if (size != dclock->priv->fsiz)
	g_object_set(dclock, "fsiz", size, NULL);

}



void digital_clock_set_sensitive(DigitalClock * dclock, gboolean sensitive)
{
    g_object_set(dclock, "sensitive", sensitive, NULL);

}


DigitalClock *digital_clock_new()
{
    return DIGITAL_CLOCK(g_object_new(TYPE_DIGITAL_CLOCK, NULL));

}
